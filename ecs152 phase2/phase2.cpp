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
const double INTERARRIVALRATE = 0.90;
const double TRANSMISSIONRATE = 1.0;
const int NUMHOSTS = 10;
const double MBPS = 11000000.0;
const int MAXDATALENGTH = 1544;
const int ACKLENGTH = 64;

//0 means psuedo infinite
const int BUFFERSIZE = 0;

//Multiplier for random back off
const double randomBackOffMult = 1.0;

const double SIFS = 0.05;
const double DIFS = 0.1;


double negative_exponenetially_distributed_time(double rate)
{
	double u;
	u = drand48();
	return ((-1 / rate)*log(1-u)) * 1000;
}

//-------------------------------------------------------------------------------

void Initiliaze(GEL **gel, double* interArrivalRate, double* throuput, double* avgNetworkDelay, double* time)
{
	*interArrivalRate = INTERARRIVALRATE;
	*throuput = 0;
	*avgNetworkDelay = 0;
	*time = 0;

	Event* firsts[NUMHOSTS];
	for (int i = 0; i < NUMHOSTS; i++){
		firsts[i] = new Event(negative_exponenetially_distributed_time(*interArrivalRate), ARRIVAL, i);
	}
	*gel = new GEL(firsts[0]);
	for (int i = 1; i < NUMHOSTS; i++)
		(*gel)->insert(firsts[i]);
}

//-------------------------------------------------------------------------------

int generate_data_transmission_length()
{
	return (rand() % MAXDATALENGTH);
}

//-------------------------------------------------------------------------------


void arrivalEvent(GEL** gel, Host* host, double arrivalRate, double *time, bool* isUsed, double wait)
{
	int source;
	int destination = rand() % NUMHOSTS;
	double timeDiff = *time;
	
	Event* e = (*gel)->remove();
	*time = e->getTime();
	timeDiff = *time - timeDiff;
	source = e->getSource();
	

	Event* nextEvent = new Event(negative_exponenetially_distributed_time(arrivalRate) + *time, ARRIVAL, source);
	(*gel)->insert(nextEvent);

	int length = generate_data_transmission_length();
	Packet* pkt = new Packet(length, destination);
	host->insert(*pkt);
	delete pkt;

	if (host->getLength == 0){
		host->startDelay(*time);
		if (isUsed) {
			double backoff = drand48() * randomBackOffMult;
			Event* backoffEvent = new Event(backoff + *time + wait, BACKOFF, source);
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

void departureEvent(GEL** gel, Host** host, double arrivalRate, double *time, bool* isUsed, double wait)
{
	int source;
	int destination;
	double timeDiff = *time;

	*isUsed = false;

	Event* e = (*gel)->remove();
	*time = e->getTime();
	timeDiff = *time - timeDiff;
	source = e->getSource();
	host[source]->endDelay(*time);
	Packet pckt = host[source]->remove();

	destination = pckt.getDestination();
	Packet* nw = new Packet(ACKLENGTH, source);
	host[destination]->insertAck(*nw);
	delete nw;
	Event* waitSIFSEvent = new Event(SIFS + *time, WAIT_SIFS, destination);

	(*gel)->insert(waitSIFSEvent);
	

	if (host[source]->getLength > 0){
		Event* waitDIFSEvent = new Event(DIFS + *time, WAIT_DIFS, source);
		(*gel)->insert(waitDIFSEvent);
	}

	delete e;
}

//-------------------------------------------------------------------------------

void waitSIFSEvent(GEL** gel, Host** host, double arrivalRate, double *time, bool* isUsed, double* wait, double* throughput){

}

//-------------------------------------------------------------------------------

void waitSIFSEvent(GEL** gel, Host** host, double arrivalRate, double *time, bool* isUsed, double* wait, double* throughput){

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

	//Wait is set whenever a data frame is being submitted. It is set to the total time needed for the frame to finish transmission.
	//Since the backoff must stop counting when the channel is busy, the value of wait is added to the backoff time.
	double wait = 0;
	
	GEL* eventList = nullptr;
	Host* hosts[NUMHOSTS];
	for (int i = 0; i < NUMHOSTS; i++)
		hosts[i] = new Host(buffersize);

	//Keeps track of the total time;
	double simTime;

	Initialize(&eventList, &queue, buffersize, &interArrivalRate, &transmissionRate, 
		&utilization, &meanQueueLength, &packetsDropped, &simTime);

	for (int i = 0; i < 100000; i++)
	{
		if (eventList->first()->getType() == ARRIVAL) 
			arrivalEvent(&eventList, &queue, interArrivalRate, transmissionRate, &utilization, 
			&meanQueueLength, &packetsDropped, &simTime);
		else
			departureEvent(&eventList, &queue, interArrivalRate, transmissionRate, 
			&utilization, &meanQueueLength, &simTime);
		//cout << "TIME: " << simTime << endl;
	}

	simTime = simTime / 1000;

	utilization = utilization / simTime;
	meanQueueLength = meanQueueLength / simTime;
	
	cout << "Mean queue length: " << meanQueueLength << endl;
	cout << "Server utilization: " << utilization << endl;
	cout << "Packets dropped: " << packetsDropped << endl;
	getchar();

}