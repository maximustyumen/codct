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
	T* m_observer;		//	наблюдатель за классом

public:
	HasObserver();
	HasObserver(T* observer);
	virtual ~HasObserver();
public:
	///	установить наблюдатель за классом
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
//	установаить наблюдатель за классом
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
	observers_t m_observers;				//	контейнер наблюдателей

public:
	HasObservers();
	virtual ~HasObservers();
public:
	///	вставить наблюдатель за классом
	virtual void insertObserver(observer_t* observer);
	///	удалить наблюдатель за классом
	virtual void removeObserver(observer_t* observer);
	///	уведомить наблюдателей
	template<typename F> void notifyObservers(F functor);
	///	уведомить наблюдателей
	template<typename M,typename P1> 
	void notifyObservers(M method,P1 p1);
	///	уведомить наблюдателей
	template<typename M,typename P1,typename P2> 
	void notifyObservers(M method,P1 p1,P2 p2);
	///	уведомить наблюдателей
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
//	вставить наблюдатель за классом
template<typename T>
inline void HasObservers<T>::insertObserver(observer_t* observer)
{
	observers_t::iterator iobserver = m_observers.find(observer);
	if(iobserver == m_observers.end()) iobserver = m_observers.insert(observers_t::value_type(observer,Holder())).first;
	if(iobserver->second.removed) iobserver->second.removed = false;
}
//	удалить наблюдатель за классом
template<typename T>
inline void HasObservers<T>::removeObserver(observer_t* observer)
{
	observers_t::iterator iobserver = m_observers.find(observer);
	if(iobserver != m_observers.end()) iobserver->second.removed = true;
}
//	уведомить наблюдателей
template<typename T> 
template<typename F>
inline void HasObservers<T>::notifyObservers(F functor)
{
	//	пробегаемся по всем наблюдателяем
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
//	уведомить наблюдателей
template<typename T> 
template<typename M,typename P1>
inline void HasObservers<T>::notifyObservers(M method,P1 p1)
{
	//	пробегаемся по всем наблюдателяем
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
//	уведомить наблюдателей
template<typename T> 
template<typename M,typename P1,typename P2>
inline void HasObservers<T>::notifyObservers(M method,P1 p1,P2 p2)
{
	//	пробегаемся по всем наблюдателяем
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
//	уведомить наблюдателей
template<typename T> 
template<typename M,typename P1,typename P2,typename P3>
inline void HasObservers<T>::notifyObservers(M method,P1 p1,P2 p2,P3 p3)
{
	//	пробегаемся по всем наблюдателяем
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
