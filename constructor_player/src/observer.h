#pragma once
#include <map>

namespace Base
{

//=====================================================================================//
//                                  class HasObserver                                  //
//=====================================================================================//
template<typename T>
class HasObserver
{
protected:
	T* m_observer;		//	����������� �� �������

public:
	HasObserver();
	HasObserver(T* observer);
	virtual ~HasObserver();
public:
	///	���������� ����������� �� �������
	virtual void set_observer(T* observer);

};

template<typename T>
HasObserver<T>::HasObserver()
: m_observer(0)
{
}

template<typename T>
HasObserver<T>::HasObserver(T* observer)
: m_observer(observer)
{
}

template<typename T>
HasObserver<T>::~HasObserver()
{
}
//	����������� ����������� �� �������
template<typename T>
void HasObserver<T>::set_observer(T* observer)
{
	m_observer = observer;
}

/*
//=====================================================================================//
//                                 class HasObservers                                  //
//=====================================================================================//
template<typename T>
class HasObservers
{
public:

	typedef T observer_t;

private:

	//=====================================================================================//
	//                                    struct Holder                                    //
	//=====================================================================================//
	struct Holder
	{
		bool removed;
		Holder() : removed(false) {}
	};
	//=====================================================================================//

	typedef std::map<T*,Holder> observers_t;
	observers_t m_observers;				//	��������� ������������

public:
	HasObservers();
	virtual ~HasObservers();
public:
	///	�������� ����������� �� �������
	virtual void insertObserver(observer_t* observer);
	///	������� ����������� �� �������
	virtual void removeObserver(observer_t* observer);
	///	��������� ������������
	template<typename F> void notifyObservers(F functor);
	///	��������� ������������
	template<typename M,typename P1> 
	void notifyObservers(M method,P1 p1);
	///	��������� ������������
	template<typename M,typename P1,typename P2> 
	void notifyObservers(M method,P1 p1,P2 p2);
	///	��������� ������������
	template<typename M,typename P1,typename P2,typename P3> 
	void notifyObservers(M method,P1 p1,P2 p2,P3 p3);

};

template<typename T>
inline HasObservers<T>::HasObservers()
{
}

template<typename T>
inline HasObservers<T>::~HasObservers()
{
}
//	�������� ����������� �� �������
template<typename T>
inline void HasObservers<T>::insertObserver(observer_t* observer)
{
	observers_t::iterator iobserver = m_observers.find(observer);
	if(iobserver == m_observers.end()) iobserver = m_observers.insert(observers_t::value_type(observer,Holder())).first;
	if(iobserver->second.removed) iobserver->second.removed = false;
}
//	������� ����������� �� �������
template<typename T>
inline void HasObservers<T>::removeObserver(observer_t* observer)
{
	observers_t::iterator iobserver = m_observers.find(observer);
	if(iobserver != m_observers.end()) iobserver->second.removed = true;
}
//	��������� ������������
template<typename T> 
template<typename F>
inline void HasObservers<T>::notifyObservers(F functor)
{
	//	����������� �� ���� �������������
	for(observers_t::iterator ivalue=m_observers.begin();ivalue!=m_observers.end();)
	{
		if(!ivalue->second.removed)
		{
			functor(ivalue->first);
			++ivalue;
		}
		else m_observers.erase(ivalue++);
	}
}
//	��������� ������������
template<typename T> 
template<typename M,typename P1>
inline void HasObservers<T>::notifyObservers(M method,P1 p1)
{
	//	����������� �� ���� �������������
	for(observers_t::iterator ivalue=m_observers.begin();ivalue!=m_observers.end();)
	{
		if(!ivalue->second.removed)
		{
			(ivalue->first->*method)(p1);
			++ivalue;
		}
		else m_observers.erase(ivalue++);
	}
}
//	��������� ������������
template<typename T> 
template<typename M,typename P1,typename P2>
inline void HasObservers<T>::notifyObservers(M method,P1 p1,P2 p2)
{
	//	����������� �� ���� �������������
	for(observers_t::iterator ivalue=m_observers.begin();ivalue!=m_observers.end();)
	{
		if(!ivalue->second.removed)
		{
			(ivalue->first->*method)(p1,p2);
			++ivalue;
		}
		else m_observers.erase(ivalue++);
	}
}
//	��������� ������������
template<typename T> 
template<typename M,typename P1,typename P2,typename P3>
inline void HasObservers<T>::notifyObservers(M method,P1 p1,P2 p2,P3 p3)
{
	//	����������� �� ���� �������������
	for(observers_t::iterator ivalue=m_observers.begin();ivalue!=m_observers.end();)
	{
		if(!ivalue->second.removed)
		{
			(ivalue->first->*method)(p1,p2,p3);
			++ivalue;
		}
		else m_observers.erase(ivalue++);
	}
}*/

}	//	namespace Base
