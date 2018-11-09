#pragma once
#include <iostream>

// Class representing a slot in the pool
/*
	m_object : the actual object of type T stored in the pool
	m_index  : the index of the slot in the array 
	m_prev   : the index of the previous non free slot in the pool
	m_next   : the index of the next non free slot in the pool
*/
template<class T>
class Slot
{
public:
	Slot() :
		m_object(),
		m_index(0),
		m_prev(0),
		m_next(0)
	{}

	Slot(uint32_t index, uint32_t prev, uint32_t next) :
		m_object(),
		m_index(index),
		m_prev(prev),
		m_next(next)
	{}

	Slot(const T& object, uint32_t index, uint32_t prev, uint32_t next) :
		m_object(object),
		m_index(index),
		m_prev(prev),
		m_next(next)
	{}

	uint32_t index()
	{
		return m_index;
	}

	uint32_t next() const
	{
		return m_next;
	}

	uint32_t prev() const
	{
		return m_prev;
	}

	T& object()
	{
		return m_object;
	}

	void linkAfter(Slot<T>& prev)
	{
		m_prev = prev.m_index;
		m_next = prev.m_next;
		prev.m_next = m_index;
	}

private:
	T m_object;
	uint32_t m_index;
	uint32_t m_prev;
	uint32_t m_next;

};




template<class T>
class PoolIterator
{
public:
	PoolIterator(uint32_t index, Slot<T>* data) :
		m_index(index),
		m_data_ptr(data),
		m_current_object(nullptr)
	{}

	bool getNext()
	{
		if (m_index)
		{
			m_current_object = &m_data_ptr[m_index - 1];
			m_index = m_current_object->next();
			return true;
		}

		return false;
	}

	T& operator*()
	{
		return m_current_object->object();
	}

private:
	uint32_t m_index;
	Slot<T>* m_data_ptr;
	Slot<T>* m_current_object;
};

// The pool itself holding data and various ADD, READ ans REMOVE operations
template<class T>
class ObjectPool
{
public:
	ObjectPool(uint32_t size = 1);

	void add(const T& object);

	T* operator[](uint32_t index);

	uint32_t size() const;

	void remove(PoolIterator<T>& it);

	PoolIterator<T> begin();
	PoolIterator<T> end();

private:
	uint32_t m_allocated_size;
	uint32_t m_size;
	Slot<T>* m_data;

	Slot<T>* m_begin;
	Slot<T>* m_end;

	uint32_t m_first_free_slot;
	uint32_t m_last_free_slot;

	void reserveMemory(uint32_t size);

	Slot<T>& getFirstSlot();

	friend PoolIterator<T>;
};

// Implementation of ObjectPool
template<class T>
inline ObjectPool<T>::ObjectPool(uint32_t size) :
	m_allocated_size(0),
	m_size(0),
	m_data(nullptr),
	m_begin(nullptr),
	m_end(nullptr),
	m_first_free_slot(0),
	m_last_free_slot(0)
{
	reserveMemory(size+2);

	m_begin = &m_data[0];
	m_end   = &m_data[size + 1];
}

template<class T>
inline void ObjectPool<T>::add(const T& object)
{
	Slot<T>& new_object = getFirstSlot();
	new_object.object() = object;

	if (m_first_object)
	{
		new_object.linkAfter(m_data[m_last_object - 1]);
	}
	else
	{
		m_first_object = new_object.index();
	}

	m_last_object = new_object.index();

	++m_size;
}

template<class T>
inline T* ObjectPool<T>::operator[](uint32_t index)
{
	if (index)
		return &m_data[index-1].object();
	else
		return nullptr;
}

template<class T>
inline uint32_t ObjectPool<T>::size() const
{
	return m_size;
}

template<class T>
inline void ObjectPool<T>::remove(PoolIterator<T>& it)
{
	Slot<T>* slot = it.m_current_object;
	// NEXT TODO
	slot->linkAfter(m_data[])
	m_last_free_slot = slot->index;
}

template<class T>
inline PoolIterator<T> ObjectPool<T>::begin()
{
	return PoolIterator<T>(m_first_object, m_data);
}

template<class T>
inline PoolIterator<T> ObjectPool<T>::end()
{
	return PoolIterator<T>();
}

template<class T>
inline void ObjectPool<T>::reserveMemory(uint32_t size)
{
	Slot<T>* new_data_ptr = static_cast<Slot<T>*>(realloc(m_data, size*sizeof(Slot<T>)));
	if (new_data_ptr)
	{
		m_data = new_data_ptr;

		// Link slots together
		// Init first slot and connect it with last free
		m_first_free_slot = m_allocated_size+1;
		m_data[m_allocated_size] = { m_first_free_slot, 0, 0};
		for (uint32_t i(m_allocated_size+1); i < size; ++i)
		{
			// Link new slots together
			m_data[i] = {i+1, i, 0};
			m_data[i].linkAfter(m_data[i-1]);
		}

		// Update las free
		m_last_free_slot  = size;

		// Update alloc size
		m_allocated_size = size;
		std::cout << "[REALLOC] new size: " << m_allocated_size << std::endl;
	}
	else
	{
		std::cout << "ERROR" << std::endl;
	}
}

template<class T>
inline Slot<T>& ObjectPool<T>::getFirstSlot()
{
	if (!m_first_free_slot)
	{
		reserveMemory(m_allocated_size << 1);
	}

	Slot<T>& slot = m_data[m_first_free_slot-1];
	m_first_free_slot = slot.next();

	return slot;
}
