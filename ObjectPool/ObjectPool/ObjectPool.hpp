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
	template<class> friend class ObjectPool;

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
	template<class> friend class ObjectPool;

	PoolIterator(uint32_t index, Slot<T>* data) :
		m_index(index),
		m_data_ptr(data)
	{}

	uint32_t index() const
	{
		return m_index;
	}

	T& operator*()
	{
		return m_data_ptr[m_index-1].object();
	}

	T* operator->()
	{
		return &m_data_ptr[m_index - 1].object();
	}

	void operator++()
	{
		if (m_index == 2)
			std::cout << "STOP" << std::endl;
		m_index = m_data_ptr[m_index - 1].next();
	}

	void operator--()
	{
		m_index = m_data_ptr[m_index - 1].prev();
	}

	template<class T>
	friend bool operator!=(const PoolIterator<T>&, const PoolIterator<T>&);

private:
	uint32_t m_index;
	Slot<T>* m_data_ptr;
};

template<class T>
bool operator!=(const PoolIterator<T>& it1, const PoolIterator<T>& it2)
{
	return it1.m_index != it2.m_index;
}





// The pool itself holding data and various ADD, READ ans REMOVE operations
template<class T>
class ObjectPool
{
public:
	ObjectPool(uint32_t size = 0);

	uint32_t add(const T& object);

	T* operator[](uint32_t index);

	uint32_t size() const;

	void remove(PoolIterator<T>& it);

	PoolIterator<T> begin();
	PoolIterator<T> end();

private:
	uint32_t m_allocated_size;
	uint32_t m_size;
	Slot<T>* m_data;
	Slot<T>* m_end;
	uint32_t m_first_free_slot;
	uint32_t m_last_free_slot;

	PoolIterator<T> m_end_cache;

	void reserveMemory(uint32_t size);

	Slot<T>& getFirstSlot();
	void freeSlot(Slot<T>& slot);

	void insertAfter(Slot<T>& new_slot, Slot<T>& slot);
	void insertBefore(Slot<T>& new_slot, Slot<T>& slot);
};

// Implementation of ObjectPool
template<class T>
inline ObjectPool<T>::ObjectPool(uint32_t size) :
	m_allocated_size(0),
	m_size(0),
	m_data(nullptr),
	m_end(nullptr),
	m_end_cache(2, nullptr),
	m_first_free_slot(0),
	m_last_free_slot(0)
{
	reserveMemory(size+2);

	m_first_free_slot = m_end->m_next;
	m_end->m_next = 0;
}

template<class T>
inline uint32_t ObjectPool<T>::add(const T& object)
{
	// Allocate new object
	Slot<T>& new_object = getFirstSlot();
	new_object.object() = object;

	// Insert new
	insertBefore(new_object, *m_end);

	++m_size;

	return new_object.m_index;
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
	// Cannot remove BEGIN or END
	if (it.m_index < 3) return;

	Slot<T>& slot = m_data[it.m_index - 1];
	it.m_index = slot.m_prev;

	freeSlot(slot);
}

template<class T>
inline void ObjectPool<T>::insertAfter(Slot<T>& new_slot, Slot<T>& slot)
{
	// Update new slot
	new_slot.m_prev = slot.m_index;
	new_slot.m_next = slot.m_next;

	// Update prev slot
	slot.m_next = new_slot.m_index;

	// Update next slot
	m_data[slot.m_next-1].m_prev = new_slot.m_index;
}

template<class T>
inline void ObjectPool<T>::insertBefore(Slot<T>& new_slot, Slot<T>& slot)
{
	// Update new slot
	new_slot.m_prev = slot.m_prev;
	new_slot.m_next = slot.m_index;

	// Update prev slot
	m_data[slot.m_prev - 1].m_next = new_slot.m_index;

	// Update next slot
	slot.m_prev = new_slot.m_index;
}

template<class T>
inline PoolIterator<T> ObjectPool<T>::begin()
{
	return PoolIterator<T>(m_data->m_next, m_data);
}

template<class T>
inline PoolIterator<T> ObjectPool<T>::end()
{
	return PoolIterator<T>(m_end->m_index, m_data);
}

template<class T>
inline void ObjectPool<T>::reserveMemory(uint32_t size)
{
	Slot<T>* new_data_ptr = static_cast<Slot<T>*>(realloc(m_data, size*sizeof(Slot<T>)));
	if (new_data_ptr)
	{
		m_data = new_data_ptr;
		m_end = &m_data[1];

		// Link slots together
		// Init first slot and connect it with last free
		m_first_free_slot = m_allocated_size+1;
		m_data[m_allocated_size] = { m_first_free_slot, 0, 0};
		for (uint32_t i(m_first_free_slot); i < size; ++i)
		{
			// Link new slots together
			m_data[i] = {i+1, i, 0};
			m_data[i - 1].m_next = i+1;
		}

		// Update las free
		m_last_free_slot  = size;

		// Update alloc size
		m_allocated_size = size;
		//std::cout << "[REALLOC] new size: " << m_allocated_size << std::endl;
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

template<class T>
inline void ObjectPool<T>::freeSlot(Slot<T>& slot)
{
	m_data[slot.m_prev - 1].m_next = slot.m_next;
	m_data[slot.m_next - 1].m_prev = slot.m_prev;

	// Destroy object
	slot.m_object.~T();

	if (!m_last_free_slot)
	{
		m_first_free_slot = slot.m_index;
		slot.m_next = 0;
	}
	else
	{
		insertAfter(slot, m_data[m_last_free_slot - 1]);
	}

	m_last_free_slot = slot.m_index;
}
