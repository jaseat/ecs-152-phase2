#ifndef PACKET_H
#define PACKET_H
const int MAXBUFFER = 100;

//Packet class: includes service time for each packet and a pointer to the next packet
class Packet{
private:
	double serviceTime;
	int destination;
public:
	Packet(){
		//nothing
	}
	Packet(double time, int destination){
		serviceTime = time;
		this->destination = destination;
	}
	double getServiceTime(){
		return serviceTime;
	}
	void setSeviceTime(double time){
		serviceTime = time;
	}
};

//Buffer class: holds a list of all packets to be serviced
class Buffer{
private:
	Packet* buffer;
	int maxSize;
	int length;
public:
	Buffer(){
		maxSize = MAXBUFFER;
		buffer = new Packet[MAXBUFFER];
		length = 0;
	}
	Buffer(int buffersize){
		if (buffersize == 0){
			maxSize = 10000;
		}
		else{
			maxSize = buffersize;
		}
		buffer = new Packet[maxSize];
		length = 0;
	}

	//returns 0 if no packet is dropped; 1 if packet is dropped
	int insert(Packet p){
		if (length + 1 > maxSize)
			return 1;
		buffer[length] = p;
		length++;
		return 0;
	}
	double remove(){
		double pckt;
		if (length > 1)
			pckt = buffer[1].getServiceTime();
		else
			pckt = -1;
		for (int i = 0; i < length; i++){
			buffer[i] = buffer[i + 1];
		}
		length--;
		return pckt;
	}
	int getLength(){
		return length;
	}
	int getMaxSize(){
		return maxSize;
	}
};
#endif