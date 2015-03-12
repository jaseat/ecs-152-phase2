#ifndef PACKET_H
#define PACKET_H
const int MAXBUFFER = 10000;

//Packet class: includes service time for each packet and a pointer to the next packet
class Packet{
private:
	int length;
	int destination;
	bool ack;
public:
	Packet(){
		//nothing
		length = 666;
	}
	Packet(int length, int destination, bool ack){
		this->length = length;
		this->destination = destination;
		this->ack = ack;
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
	bool isAck(){
		return ack;
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
	Packet peek(){
		if (length < 1)
			exit(1);
		return buffer[0];
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
	Packet acks[256];
	int ackLength;
	double totalDelay;
	double startDelayTime;
public:
	int fail;
	int wait;
	int waitAck;
	bool recievedAck;
	bool sent;
	bool sensed;
	bool departing;
	Host() : Buffer(){
		totalDelay = 0;
	}
	Host(int buffersize) : Buffer(buffersize){
		totalDelay = 0;
		fail = 1;
		ackLength = 0;
		wait = 0;
		waitAck = 0;
		recievedAck = true;
		sent = false;
		sensed = false;
		departing = false;
	}
	void startDelay(double delay){
		startDelayTime = delay;
	}
	void endDelay(double delay){
		//if (delay - startDelayTime > 100)
			//std::cout << "Woah" << std::endl;
		totalDelay = totalDelay + (delay - startDelayTime);
	}
	double getTotalDeay(){
		return totalDelay;
	}
	void insertAck(Packet p){
		acks[ackLength] = p;
		ackLength++;
	}
	Packet removeAck(){
		Packet pckt = acks[0];
		for (int i = 0; i < ackLength; i++){
			acks[i] = acks[i + 1];
		}
		ackLength--;
		return pckt;
	}
	Packet peekAck(){
		if (ackLength < 1)
			exit(1);
		return acks[0];
	}
	int getAckLength(){
		return ackLength;
	}
};
#endif