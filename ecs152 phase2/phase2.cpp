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
const double TRANSMISSIONRATE = 1;

//0 means psuedo infinite
const int BUFFERSIZE = 0;

//Multiplier for random back off
const double randomBackOffMult = 1;


double negative_exponenetially_distributed_time(double rate)
{
	double u;
	u = drand48();
	return ((-1 / rate)*log(1-u));
}

//-------------------------------------------------------------------------------


/*
//Initialize data structures, and statistics. Creates first arrival event and inserts it into GEL
void Initialize(GEL **gel, Buffer** queue, int buffersize, double* interArrivalRate, 
	double* transmissionRate, double* utilization, double* meanQueueLength, int* packetsdropped, double* time)
{
	*queue = new Buffer(buffersize);
	*transmissionRate = TRANSMISSIONRATE;
	*interArrivalRate = INTERARRIVALRATE;
	*utilization = 0;
	*meanQueueLength = 0;
	*packetsdropped = 0;
	*time = 0;


	Event* first = new Event(negative_exponenetially_distributed_time(*interArrivalRate), ARRIVAL);
	*gel = new GEL(first);

}

//-------------------------------------------------------------------------------

void arrivalEvent(GEL** gel, Buffer** queue, double arrivalRate, double serviceRate, 
	double* utilization, double* meanQueueLength, int* packetDrop, double *time)
{
	double timeDiff = *time;
	
	Event* e = (*gel)->remove();
	*time = e->getTime();
	timeDiff = *time - timeDiff;

	if ((*queue)->getLength() > 0){
		*utilization = *utilization + timeDiff;
	}
	double queueLength;
	queueLength = timeDiff * (*queue)->getLength();
	*meanQueueLength = *meanQueueLength + queueLength;

	Event* nextEvent = new Event(negative_exponenetially_distributed_time(arrivalRate) + *time, ARRIVAL);
	(*gel)->insert(nextEvent);

	if ((*queue)->getLength() == 0) {
		double serviceTime = negative_exponenetially_distributed_time(serviceRate);
		Packet* pkt = new Packet(serviceTime);
		Event* departureEvent = new Event(serviceTime + *time, DEPARTURE);
		(*gel)->insert(departureEvent);
		(*queue)->insert(*pkt);
		delete pkt;
	}
	else{
		double serviceTime = negative_exponenetially_distributed_time(serviceRate);
		Packet* pkt = new Packet(serviceTime);
		if ((*queue)->insert(*pkt) == 1)
			*packetDrop = *packetDrop + 1;
		delete pkt;
	}

	delete e;
}

//-------------------------------------------------------------------------------

void departureEvent(GEL** gel, Buffer** queue, double arrivalRate, double serviceRate, 
	double* utilization, double* meanQueueLength, double *time)
{
	double timeDiff = *time;

	Event* e = (*gel)->remove();
	*time = e->getTime();
	timeDiff = *time - timeDiff;

	if ((*queue)->getLength() > 0){
		*utilization = *utilization + timeDiff;
	}
	double queueLength;
	queueLength = timeDiff * (*queue)->getLength();
	*meanQueueLength = *meanQueueLength + queueLength;

	double nextDeparture = (*queue)->remove();
	if (nextDeparture > -1){
		Event* departure = new Event(nextDeparture + *time, DEPARTURE);
		(*gel)->insert(departure);
	}

	delete e;

}

//-------------------------------------------------------------------------------

*/

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
	//The rate for the transmission time of packets
	double transmissionRate;

	GEL* eventList = nullptr;
	Buffer* queue = nullptr;
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

	utilization = utilization / simTime;
	meanQueueLength = meanQueueLength / simTime;
	
	cout << "Mean queue length: " << meanQueueLength << endl;
	cout << "Server utilization: " << utilization << endl;
	cout << "Packets dropped: " << packetsDropped << endl;
	getchar();

}