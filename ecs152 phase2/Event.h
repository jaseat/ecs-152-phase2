#ifndef HEADER_H
#define HEADER_H
//For types of events: Arrival, departue, backoff, and wait. For backoff the events are cleared when
//the channel becomes busy and reentered when the channel is clear. When the events are cleared the
//hosts should update their wait time
enum EventType { ARRIVAL, DEPARTURE, DEPARTURE_ACK, BACKOFF, WAIT_DIFS, WAIT_SIFS, UPDATE };


//Event class: Includes time of the event and type of the event and pointers to next and previous event
class Event
{
private:
	double eventTime;
	EventType type;
	Event* next;
	Event* previous;
	int source;
public:
	Event(){
		//nothing
	}
	Event(double time, EventType type, int source){
		eventTime = time;
		this->type = type;
		this->source = source;
		next = nullptr;
		previous = nullptr;
	}
	double getTime(){
		return eventTime;
	}
	void setTime(double time){
		eventTime = time;
	}
	EventType getType(){
		return type;
	}
	int getSource(){
		return source;
	}
	void setNext(Event* next){
		this->next = next;
	}
	void setPrevious(Event* previous){
		this->previous = previous;
	}
	Event* getNext(){
		return next;
	}
	Event* getPrevious(){
		return previous;
	}
};

//A list of all events. Include methods to add and remove events. Adds events based on time and removes the first event
class GEL
{
private:
	Event* head;
public:
	GEL(){
		head = nullptr;
	}
	GEL(Event* head){
		this->head = head;
	}
	//inserts events by order of time
	void insert(Event* e){
		Event* cur = head;
		if (head == nullptr){
			head = e;
			return;
		}
		else{
			if (cur->getTime() > e->getTime()){
				e->setNext(cur);
				cur->setPrevious(e);
				head = e;
				return;
			}
			while (e->getTime() >= cur->getTime()){
				if (cur->getNext() == nullptr){
					cur->setNext(e);
					e->setPrevious(cur);
					e->setNext(nullptr);
					return;
				}
				cur = cur->getNext();
			}
			Event* s = cur;
			if (cur->getPrevious() == nullptr)
				std::cout << "HEY LISTEN!";
			cur = cur->getPrevious();
			cur->setNext(e);
			e->setNext(s);
			e->setPrevious(cur);
			s->setPrevious(e);
		}

	}
	Event* remove(){
		Event* ptr = head;
		if (head->getNext() != nullptr){
			head = head->getNext();
			head->setPrevious(nullptr);
		}
		else 
			head = nullptr;
		return ptr;
	}

	Event* first(){
		return head;
	}
	//Removes all events from a certain host
	void removeHost(int hostNum){
		while (head->getSource() == hostNum){
			Event* h = head;
			head = head->getNext();
			head->setPrevious(nullptr);
			delete h;
		}
		Event* cur = head;
		while (cur->getNext != nullptr){
			if (cur->getSource() == hostNum){
				Event* e = cur;
				Event* prev = cur->getPrevious();
				Event* next = cur->getNext();
				prev->setNext(next);
				next->setPrevious(prev);
				cur = next;
				delete e;
			}
		}
		if (cur->getSource == hostNum){
			Event* prev = cur->getPrevious();
			prev->setNext(nullptr);
			delete cur;
		}
	}
};
#endif