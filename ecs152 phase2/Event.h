#ifndef HEADER_H
#define HEADER_H
//For types of events: Arrival, departue, backoff, and wait. For backoff the events are cleared when
//the channel becomes busy and reentered when the channel is clear. When the events are cleared the
//hosts should update their wait time
enum EventType { ARRIVAL, DEPARTURE, BACKOFF, WAIT_DIFS, WAIT_SIFS };


//Event class: Includes time of the event and type of the event and pointers to next and previous event
class Event
{
private:
	double eventTime;
	EventType type;
	Event* next;
	Event* previous;
	int source;
	int destination;
public:
	Event(){
		//nothing
	}
	Event(double time, EventType type, int source, int destination){
		eventTime = time;
		this->type = type;
		this->source = source;
		this->destination = destination;
		next = nullptr;
		previous = nullptr;
	}
	double getTime(){
		return eventTime;
	}
	EventType getType(){
		return type;
	}
	int getSource(){
		return source;
	}
	int getDestination(){
		return destination;
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
		if (head == nullptr)
			head = e;
		else{
			if (cur->getTime() > e->getTime()){
				e->setNext(cur);
				cur->setPrevious(e);
				head = e;
			}
			while (e->getTime() > cur->getTime()){
				if (cur->getNext() == nullptr){
					cur->setNext(e);
					e->setPrevious(cur);
					e->setNext(nullptr);
					return;
				}
				cur = cur->getNext();
			}
			Event* s = cur;
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
	//Removes all events of type Backoff from the event list
	void clearBackoff(){
		if (head == nullptr)
			return;
		Event* e = head;
		while (head->getType() == BACKOFF){
			if (head->getNext() != nullptr){
				head = e->getNext();
				head->setPrevious(nullptr);
				delete e;
				e = head;
			}
		}
		while (e->getNext() != nullptr)
		{
			if (e->getType() == BACKOFF){
				Event* prev = e->getPrevious();
				Event* next = e->getNext();
				prev->setNext(next);
				next->setPrevious(prev);
				delete e;
				e = next;

			}
		}
		if (e->getType() == BACKOFF){
			Event* prev = e->getPrevious();
			prev->setNext(nullptr);
			delete e;
		}
	}
};
#endif