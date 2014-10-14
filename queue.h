/*
 * queue.h
 *
 *  Created on: 2014-7-21
 *      Author: wisdomlin
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <iostream>
#include <stdexcept>
#include <memory>

using namespace std;

template<class Type>
class CacheObj
{
public:
	void* operator new(size_t);
	void operator delete(void *,size_t);
	virtual ~CacheObj(){}
protected:
	Type * next;
private:
	static allocator<Type> alloc_mem;
	static void add_to_freelist(Type*);
	static Type* freeStore;
	static const size_t chunk = 20;

};
template<class Type>
allocator<Type> CacheObj<Type>::alloc_mem;

template<class Type>
Type* CacheObj<Type>::freeStore;


template<class Type>
const size_t CacheObj<Type>::chunk;


template<class Type>
void CacheObj<Type>::add_to_freelist(Type * pt)
{
	pt->CacheObj<Type>::next = freeStore;
	freeStore = pt;
}


template<class Type>
void * CacheObj<Type>::operator new(size_t sz)
{
	cout << "new" << endl;
	if(sz != sizeof(Type))
	{
		throw std::runtime_error("wrong size object");
	}

	if(!freeStore)
	{
		Type* array = alloc_mem.allocate(chunk);
		for(size_t i = 0;i<chunk;i++)
			add_to_freelist(&array[i]);
	}

	Type * p = freeStore;
	freeStore = freeStore->CacheObj<Type>::next;

	return p;
}

template<class Type>
void CacheObj<Type>::operator delete(void *p,size_t n)
{
	cout << "delete" << endl;
	if(p)
		add_to_freelist(static_cast<Type*>(p));
}



template <class Type>
class Queue
{
	template<class _Type>
	friend std::ostream& operator<< (std::ostream&,const Queue<_Type> &);
private:
	class QueueItem : public CacheObj< QueueItem >
	{
	public:
		QueueItem(const Type& t):item(t),next(0){}

		Type item;
		QueueItem* next;
	};
public:
	Queue():head(0),tail(0){}
	template<class It>
	Queue(It beg,It end):head(0),tail(0){ copy_elems(beg,end);}
	Queue(const Queue& q):head(0),tail(0){copy_elems(q);}

	Queue& operator=(const Queue&);
	~Queue() {destroy();}
	template <class Iter> void assign(Iter beg,Iter end);
	Type& front() {return head->item;}
	const Type& front() const {return head->item;}
	void push(const Type& );
	void pop();
	bool empty() const {return head == 0;}

private:
	QueueItem * head;
	QueueItem* tail;
	void destroy();
	void copy_elems(const Queue&);
	template <class Iter> void copy_elems(Iter,Iter);
};

template<class Type>
void Queue<Type>::push(const Type& t)
{
	QueueItem* pt = new QueueItem(t);
	if(empty())
	{
		head = pt;
		tail = pt;
	}
	else
	{
		tail->next = pt;
		tail = pt;
	}
}

template<class Type>
void Queue<Type>::pop()
{
	QueueItem* pt = head;
	head = head->next;
	delete pt;
}


template<class Type>
template<class Iter>
void Queue<Type>::copy_elems(Iter beg,Iter end)
{
	for(;beg!=end;beg++)
	{
		push(*beg);
	}
}

template<class Type>
void Queue<Type>::destroy()
{
	while(!empty())
		pop();

}

template<class Type>
void Queue<Type>::copy_elems(const Queue& q)
{
	for(QueueItem* qt = q.head;qt;qt = qt->next)
		push(qt->item);
}

template<class Type>
Queue<Type>& Queue<Type>::operator=(const Queue<Type>& q)
{
	destroy();
	head = 0;
	tail = 0;
	copy_elems(q);
	return *this;
}

template<class Type>
std::ostream& operator<< (std::ostream& os,const Queue<Type> & q)
{

	const typename Queue<Type>::QueueItem* pt = q.head;
	os << "[";
	while(pt!=0)
	{
		os << pt->item << ",";
		pt = pt->next;
	}
	os << "]";
	return os;
}

#endif /* QUEUE_H_ */

