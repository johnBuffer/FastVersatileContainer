#pragma once
#include <iostream>

// Class representing a slot in the pool
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
		if (prev.m_index)
		{
			prev.m_next = m_index;
			m_next = prev.m_next;
		}
		else
		{
			m_next = 0;
			m_prev = 0;
		}
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
		m_data_ptr(data)
	{
		if (index)
	}

	T* getNext()
	{
		if (m_index)
		{
			uint32_t next_index = m_data_ptr[m_index].next();
			if (next_index)
			{
				m_index = next_index;
				return &m_data_ptr[m_index];
			}
			else
			{
				return nullptr;
			}
		}
		else
		{

		}
	}

private:
	uint32_t m_index;
	Slot<T>* m_current_object, m_data_ptr;
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

private:
	uint32_t m_allocated_size;
	uint32_t m_size;
	Slot<T>* m_data;

	uint32_t m_first_object;
	uint32_t m_last_object;
	uint32_t m_first_free_slot;
	uint32_t m_last_free_slot;

	void reserveMemory(uint32_t size);

	Slot<T>& getFirstSlot();
};

// Implementation of ObjectPool
template<class T>
inline ObjectPool<T>::ObjectPool(uint32_t size) :
	m_allocated_size(size),
	m_size(0),
	m_data(nullptr),
	m_first_object(0),
	m_last_object(0),
	m_first_free_slot(0),
	m_last_free_slot(0)
{
	reserveMemory(m_allocated_size);

	if (size)
	{
		m_first_object = 0;
		m_last_object  = 0;
		m_first_free_slot = 1;
		m_last_free_slot  = 1;
	}
}

template<class T>
inline void ObjectPool<T>::add(const T& object)
{
	Slot<T>& new_object = getFirstSlot();
	m_first_free_slot = new_object.next()+1;

	new_object.object() = object;
	new_object.linkAfter(m_data[m_last_object]);

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
inline void ObjectPool<T>::reserveMemory(uint32_t size)
{
	Slot<T>* new_data_ptr = static_cast<Slot<T>*>(realloc(m_data, size*sizeof(Slot<T>)));
	if (new_data_ptr)
	{
		m_data = new_data_ptr;

		// Link slots together
		// Init first slot and connect it with last free
		m_first_free_slot = m_allocated_size + 1;
		m_data[m_allocated_size] = { m_first_free_slot, 0, 0};
		for (uint32_t i(m_allocated_size+1); i < size; ++i)
		{
			// Link new slots together
			m_data[i] = {i, i, 0};
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
