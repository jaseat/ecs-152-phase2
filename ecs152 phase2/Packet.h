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
	int fail;
public:
	int wait;
	bool recievedAck;
	bool sent;
	Host() : Buffer(){
		totalDelay = 0;
	}
	Host(int buffersize) : Buffer(buffersize){
		totalDelay = 0;
		fail = 1;
		ackLength = 0;
		wait = 0;
		recievedAck = true;
		sent = false;
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
		acks[ackLength] = p;
		ackLength++;
	}
	int getFail(){
		return fail;
	}
	void incFail(){
		fail++;
	}
	void clearFail(){
		fail = 1;
	}

};
#endif