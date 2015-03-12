#include <iostream>
#include <Event.h>
#include <Packet.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
double drand48(){
	return (double)(rand()) / (RAND_MAX+1);
}
void srand48(long time){
	srand(time);
}
#endif

using namespace std;
const double INTERARRIVALRATE = 1;
const double TRANSMISSIONRATE = 1.0;
const int NUMHOSTS = 10;
const double MBPS = 11000000.0;
const int MAXDATALENGTH = 1544;
const int ACKLENGTH = 64;
const double SENSE = 0.1;

//0 means psuedo infinite
const int BUFFERSIZE = 0;

//Multiplier for random back off
const int randomBackOffMult = 5;

const double SIFS = 0.05;
const double DIFS = 0.1;


double negative_exponenetially_distributed_time(double rate)
{
	double u;
	u = drand48();
	return ((-1 / rate)*log(1-u)) * 1000;
}

//-------------------------------------------------------------------------------

bool conflict(Host** hosts)
{
	int t = 0;
	for (int i = 0; i < NUMHOSTS; i++){
		if (hosts[i]->departing == true)
			t++;
	}

	return t > 1;
}

void Initiliaze(GEL **gel, double* interArrivalRate, double* throughput, double* avgNetworkDelay, double* time)
{
	*interArrivalRate = INTERARRIVALRATE;
	*throughput = 0;
	*avgNetworkDelay = 0;
	*time = 0;

	Event* firsts[NUMHOSTS];
	for (int i = 0; i < NUMHOSTS; i++){
		firsts[i] = new Event(negative_exponenetially_distributed_time(*interArrivalRate), ARRIVAL, i);
	}
	*gel = new GEL(firsts[0]);
	for (int i = 1; i < NUMHOSTS; i++)
		(*gel)->insert(firsts[i]);
	Event* updateEvent = new Event(*time + SENSE, UPDATE, NUMHOSTS+1);
	(*gel)->insert(updateEvent);
}

//-------------------------------------------------------------------------------

int generate_data_transmission_length()
{
	return (rand() % MAXDATALENGTH);
}

//-------------------------------------------------------------------------------

void arrivalEvent(GEL** gel, Host* host, double arrivalRate, double *time)
{
	int source;
	int destination = (rand() % NUMHOSTS);
	
	double timeDiff = *time;
	
	Event* e = (*gel)->remove();
	*time = e->getTime();
	timeDiff = *time - timeDiff;
	source = e->getSource();
	
	while (source == destination){
		destination = (rand() % NUMHOSTS);
	}

	Event* nextEvent = new Event(negative_exponenetially_distributed_time(arrivalRate) + *time, ARRIVAL, source);
	(*gel)->insert(nextEvent);

	int length = generate_data_transmission_length();
	Packet* pkt = new Packet(length, destination, false);
	host->insert(*pkt);
	delete pkt;

	if (host->getLength() == 1){
		host->startDelay(*time);
		if (host->sensed) {
			double backoff = drand48() * randomBackOffMult;
			Event* backoffEvent = new Event(backoff + *time, BACKOFF, source);
			(*gel)->insert(backoffEvent);
		}
		else{
			Event* waitDIFSEvent = new Event(DIFS + *time, WAIT_DIFS, source);
			(*gel)->insert(waitDIFSEvent);
		}
	}

	delete e;
}

//-------------------------------------------------------------------------------

void departureEvent(GEL** gel, Host** host, double arrivalRate, double *time, bool* isUsed, double* throughput, bool* confs)
{
	int source;
	int destination;
	double timeDiff = *time;

	bool conf = conflict(host);

	Event* e = (*gel)->remove();
	*time = e->getTime();
	timeDiff = *time - timeDiff;
	source = e->getSource();

	host[source]->sent = true;
	host[source]->recievedAck = false;
	host[source]->waitAck = 100;
	host[source]->departing = false;


	if (conf || *confs){
		if (!conf){
			*confs = false;
			*isUsed = false;
		}
		host[source]->fail = host[source]->fail + 1;
		host[source]->wait = drand48() * randomBackOffMult * host[source]->fail;
		return;
	}
	*confs = false;
	*isUsed = false;

	host[source]->fail = 1;

	Packet pckt = host[source]->peek();

	*throughput = *throughput + pckt.getLength();

	host[source]->endDelay(*time);
	destination = pckt.getDestination();

	Packet* nw = new Packet(ACKLENGTH, source, true);
	host[destination]->insertAck(*nw);

	if (host[destination]->getLength() > 0)
		host[destination]->endDelay(*time);

	
	delete nw;

	if (host[destination]->getAckLength() == 1){
		host[destination]->startDelay(*time);
		(*gel)->removeHost(destination);
		Event* waitSIFSEvent = new Event(SIFS + *time, WAIT_SIFS, destination);
		(*gel)->insert(waitSIFSEvent);
	}


	delete e;
}

//-------------------------------------------------------------------------------

void departureAckEvent(GEL** gel, Host** host, double arrivalRate, double *time, bool* isUsed, double* throughput, bool* confs)
{
	int source;
	int destination;
	double timeDiff = *time;

	bool conf = conflict(host);

	Event* e = (*gel)->remove();
	*time = e->getTime();
	timeDiff = *time - timeDiff;
	source = e->getSource();

	host[source]->sent = false;
	host[source]->recievedAck = true;
	host[source]->waitAck = 0;
	host[source]->departing = false;


	if (conf || *confs){
		if (!conf){
			*confs = false;
			*isUsed = false;
		}
		host[source]->fail = host[source]->fail + 1;
		host[source]->wait = drand48() * randomBackOffMult * host[source]->fail;
		return;
	}
	*confs = false;
	*isUsed = false;

	host[source]->fail = 1;

	Packet pckt = host[source]->removeAck();

	*throughput = *throughput + pckt.getLength();

	host[source]->endDelay(*time);
	destination = pckt.getDestination();

	host[destination]->recievedAck = true;


	if (host[source]->getAckLength() > 0){
			host[source]->startDelay(*time);
			Event* waitSIFSEvent = new Event(SIFS + *time, WAIT_SIFS, source);
			(*gel)->insert(waitSIFSEvent);
	}
	else if (host[source]->getLength() > 0){
		host[source]->startDelay(*time);
		Event* waitDIFSEvent = new Event(DIFS + *time, WAIT_DIFS, source);
		(*gel)->insert(waitDIFSEvent);
	}

	delete e;
}

//-------------------------------------------------------------------------------

void createDepartureEvent(GEL** gel, Host** host, double *time, bool* isUsed, bool* confs, int source)
{
	Packet pckt = host[source]->peek();
	host[source]->sent = false;
	int length = pckt.getLength();
	double serviceTime = (length * 8) / MBPS;
	*isUsed = true;
	host[source]->departing = true;
	host[source]->sensed = true;
	*confs = conflict(host);
	Event* departureEvent = new Event(serviceTime + *time, DEPARTURE, source);
	(*gel)->insert(departureEvent);
}

//-------------------------------------------------------------------------------

void waitEventDFIS(GEL** gel, Host** host, double *time, bool* isUsed, bool* confs){
	int source;
	int destination;
	double timeDiff = *time;

	Event* e = (*gel)->remove();
	*time = e->getTime();
	timeDiff = *time - timeDiff;
	source = e->getSource();

	if (host[source]->sensed){
		int backoff = drand48() * randomBackOffMult * host[source]->fail;
		host[source]->wait = backoff;
	}
	else{
		createDepartureEvent(gel, host, time, isUsed, confs, source);
	}

	delete e;
}

//-------------------------------------------------------------------------------

void waitEventSFIS(GEL** gel, Host** host, double *time, bool* isUsed, bool* confs){
	int source;
	int destination;
	double timeDiff = *time;

	Event* e = (*gel)->remove();
	*time = e->getTime();
	timeDiff = *time - timeDiff;
	source = e->getSource();

	if (host[source]->sensed){
		int backoff = drand48() * randomBackOffMult * host[source]->fail;
		host[source]->wait = backoff;
	}
	else{
		Packet pckt = host[source]->peekAck();
		host[source]->sent = false;
		int length = pckt.getLength();
		double serviceTime = (length * 8) / MBPS;
		*isUsed = true;
		host[source]->departing = true;
		host[source]->sensed = true;
		*confs = conflict(host);
		Event* departureEvent = new Event(serviceTime + *time, DEPARTURE_ACK, source);
		(*gel)->insert(departureEvent);
	}

	delete e;
}

//-------------------------------------------------------------------------------

void updateEvent(GEL **gel, Host** hosts, bool* isUsed, double* time, bool* confs, bool* change)
{
	*change = false;
	for (int i = 0; i < NUMHOSTS; i++){
		hosts[i]->sensed = *isUsed;
	}

	Event* e = (*gel)->remove();
	*time = e->getTime();

	Event* updateEvent = new Event(*time + SENSE, UPDATE, NUMHOSTS+1);
	(*gel)->insert(updateEvent);
	
	if (!(*isUsed)){
		for (int i = 0; i < NUMHOSTS; i++){
			if (hosts[i]->wait > 1){
				*change = true;
				if (hosts[i]->wait == 1){
					hosts[i]->wait = 0;
					createDepartureEvent(gel, hosts, time, isUsed, confs, i);
				}
				else
					hosts[i]->wait = hosts[i]->wait - 1;
			}
		}//for (int i = 0; i < NUMHOSTS; i++)
	}

	for (int i = 0; i < NUMHOSTS; i++){
		if (hosts[i]->sent){
			if (hosts[i]->recievedAck){
				hosts[i]->remove();
				hosts[i]->sent = false;
				*change = true;
				if (hosts[i]->getLength() > 0){
					hosts[i]->startDelay(*time);
					Event* waitDIFSEvent = new Event(DIFS + *time, WAIT_DIFS, i);
					(*gel)->insert(waitDIFSEvent);
					*change = true;
				}
			}//if (hosts[i]->recievedAck)
			else if (hosts[i]->waitAck > 0){
				*change = true;
				hosts[i]->waitAck = hosts[i]->waitAck - 1;
			}
			else if (hosts[i]->waitAck == 0){
				*change = true;
				if (*isUsed){
					int backoff = drand48() * randomBackOffMult * hosts[i]->fail;
					hosts[i]->wait = backoff;
				}
				else{
					createDepartureEvent(gel, hosts, time, isUsed, confs, i);
				}
			}
		}//if (hosts[i]->sent)
	}//for (int i = 0; i < NUMHOSTS; i++)

	delete e;
}


//-------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	int buffersize = BUFFERSIZE;
	srand48(time(NULL));
	drand48();

	//Number of bytes successfully transferred divided by total time
	double throughput;
	//Total delay for all hosts divided by throughput
	double avgNetworkDelay;

	//The rate for the arrival of packets
	double interArrivalRate;

	//True if channel is being used, false if channel is free
	bool isUsed = false;

	bool confs = false;
	bool change = false;
	
	GEL* eventList = nullptr;
	Host* hosts[NUMHOSTS];
	for (int i = 0; i < NUMHOSTS; i++)
		hosts[i] = new Host(buffersize);

	//Keeps track of the total time;
	double simTime;

	Initiliaze(&eventList, &interArrivalRate, &throughput, &avgNetworkDelay, &simTime);

	for (int i = 0; i < 100000; i++)
	{
		/*for (int j = 0; j < NUMHOSTS; j++)
			cout << "HOST " << j << ": " << hosts[j]->getLength() << endl;*/
		Event* curEvent = eventList->first();
		int source = curEvent->getSource();
		cout << "I: " << i << endl;
		switch (curEvent->getType())
		{
		case ARRIVAL:
			change = true;
			arrivalEvent(&eventList, hosts[source], interArrivalRate, &simTime);
			break;
		case DEPARTURE:
			change = true;
			departureEvent(&eventList, hosts, interArrivalRate, &simTime, &isUsed, &throughput, &confs);
			break;
		case DEPARTURE_ACK:
			change = true;
			departureAckEvent(&eventList, hosts, interArrivalRate, &simTime, &isUsed, &throughput, &confs);
			break;
		case WAIT_DIFS:
			change = true;
			waitEventDFIS(&eventList, hosts, &simTime, &isUsed, &confs);
			break;
		case WAIT_SIFS:
			change = true;
			waitEventSFIS(&eventList, hosts, &simTime, &isUsed, &confs);
			break;
		case BACKOFF:
			break;
		case UPDATE:
			if (change == true){
				updateEvent(&eventList, hosts, &isUsed, &simTime, &confs, &change);
			}
			else{
				Event* e = eventList->remove();
				delete e;
				Event* updateEvent = new Event(eventList->first()->getTime() + SENSE, UPDATE, NUMHOSTS + 1);
				eventList->insert(updateEvent);
			}
			i--;
			break;
		}
		//cout << "TIME: " << simTime << endl;
	}

	simTime = simTime / 1000;

	throughput = throughput / simTime;
	for (int i = 0; i < NUMHOSTS; i++){
		avgNetworkDelay = avgNetworkDelay + hosts[i]->getTotalDeay();
	}
	avgNetworkDelay = avgNetworkDelay / 1000;
	avgNetworkDelay = avgNetworkDelay / throughput;
	
	cout << "Throughput: " << throughput << endl;
	cout << "Average Network Delay: " << avgNetworkDelay << endl;
	cout << "Simulation Time: " << simTime << endl;
	getchar();

}