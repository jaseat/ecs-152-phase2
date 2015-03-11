#ifndef HEADER_H
#define HEADER_H
//Two types of events: arrival and departure
enum EventType { ARRIVAL, DEPARTURE };


//Event class: Includes time of the event and type of the event and pointers to next and previous event
class Event
{
private:
	double eventTime;
	EventType type;
	Event* next;
	Event* previous;
public:
	Event(){
		//nothing
	}
	Event(double time, EventType type){
		eventTime = time;
		this->type = type;
		next = nullptr;
		previous = nullptr;
	}
	double getTime(){
		return eventTime;
	}
	EventType getType(){
		return type;
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
};
#endif