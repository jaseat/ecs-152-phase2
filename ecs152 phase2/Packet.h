#ifndef PACKET_H
#define PACKET_H
const int MAXBUFFER = 100;

//Packet class: includes service time for each packet and a pointer to the next packet
class Packet{
private:
	int length;
	int destination;
public:
	Packet(){
		//nothing
	}
	Packet(int length, int destination){
		this->length = length;
		this->destination = destination;
	}
	int getLength(){
		return length;
	}
	void setLength(int length){
		this->length = length;
	}
	int getDestination(){
		return destination;
	}

};

//Buffer class: holds a list of all packets to be serviced
class Buffer{
protected:
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
	Packet remove(){
		Packet pckt;
		if (length < 1)
			exit(1);
		else
			pckt = buffer[0];
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

class Host : public Buffer{
private:
	double totalDelay;
	double startDelayTime;
public:
	Host() : Buffer(){
		totalDelay = 0;
	}
	Host(int buffersize) : Buffer(buffersize){
		totalDelay = 0;
	}
	void startDelay(double delay){
		startDelayTime = delay;
	}
	void endDelay(double delay){
		totalDelay = totalDelay + (delay - startDelayTime);
	}
	double getTotalDeay(){
		return totalDelay;
	}
	void insertAck(Packet p){
		for (int i = 0; i < length; i++){
			buffer[length - i] = buffer[length - (i + 1)];
		}
		buffer[0] = p;
		length++;
	}

};
#endif