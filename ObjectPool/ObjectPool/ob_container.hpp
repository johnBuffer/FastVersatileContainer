#pragma once
#include <limits>

// Debug
#include <iostream>
#include <string>

namespace clstr
{


class SlotCluster
{
public:
	template<class>
	friend class Iterator;
	template<class>
	friend class OffsetBasedContainer;

	SlotCluster(uint32_t first, uint32_t last, bool free = true) :
		m_free(free),
		m_frst(first),
		m_last(last),
		m_next(nullptr),
		m_prev(nullptr)
	{}

private:
	bool m_free;

	uint32_t m_frst;
	uint32_t m_last;

	SlotCluster* m_next;
	SlotCluster* m_prev;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////


template<class T>
class Slot
{
public:
	template<class>
	friend class OffsetBasedContainer;
	template<class>
	friend class Iterator;

	Slot();

private:
	T m_object;
	SlotCluster* m_cluster;
};

template<class T>
inline Slot<T>::Slot()
{
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////



template<class T>
class Iterator
{
public:
	template<class T>
	friend bool operator!=(const Iterator<T>&, const Iterator<T>&);

	Iterator(uint32_t index, Slot<T>* data_ptr):
		m_index(index),
		m_data_ptr(data_ptr)
	{
		if (m_index != std::numeric_limits<uint32_t>::max())
			m_cluster_end = data_ptr[index].m_cluster->m_last;
	}

	T& operator*()
	{
		return m_data_ptr[m_index].m_object;
	}

	void operator++()
	{
		if (m_index < m_cluster_end)
		{
			++m_index;
		}
		else
		{
			SlotCluster* next = m_data_ptr[m_index].m_cluster->m_next;
			if (next && next->m_next)
			{
				m_index       = next->m_next->m_frst;
				m_cluster_end = next->m_next->m_last;
			}
			else
			{
				m_index = std::numeric_limits<uint32_t>::max();
			}
		}
	}

private:
	uint32_t m_index;
	uint32_t m_cluster_end;
	Slot<T>* m_data_ptr;
};


template<class T>
bool operator!=(const Iterator<T>& it1, const Iterator<T>& it2)
{
	return it1.m_index != it2.m_index;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////


template<class T>
class OffsetBasedContainer
{
public:
	OffsetBasedContainer();

	void add(const T& object);

	Iterator<T> begin();
	Iterator<T> end();

private:
	// Data info
	uint32_t m_size;
	Slot<T>* m_data; 

	SlotCluster* m_slots_head;
	SlotCluster* m_first_free;
	SlotCluster* m_first_objt;

	void createFreeCluster();
	void createObjtCluster();
	void removeCluster(SlotCluster* cluster);
};

template<class T>
inline OffsetBasedContainer<T>::OffsetBasedContainer() :
	m_size(0),
	m_slots_head(nullptr),
	m_first_free(nullptr),
	m_first_objt(nullptr)
{
	createFreeCluster();
	m_slots_head = m_first_free;
}

template<class T>
inline void OffsetBasedContainer<T>::add(const T& object)
{
	// No free slots available
	if (!m_first_free)
	{
		createFreeCluster();
	}

	Slot<T>&     slot = m_data[m_first_free->m_frst];
	SlotCluster* prev = m_first_free->m_prev;

	// If cluster before just add the new slot in it
	if (prev)
	{
		// Update slot's cluster
		slot.m_cluster = prev;
		// Update ranges of the object cluster
		++(prev->m_last);
	}
	// If not create a new cluster and add
	else  
	{
		createObjtCluster();
		slot.m_cluster = m_first_objt;
	}

	// Update ranges of the free cluster
	++(m_first_free->m_frst);
	// Check if the free cluster is now empty
	if (m_first_free->m_frst > m_first_free->m_last)
	{
		removeCluster(m_first_free);
	}

	slot.m_object = object;
}

template<class T>
inline Iterator<T> OffsetBasedContainer<T>::begin()
{
	return Iterator<T>(m_first_objt->m_frst, m_data);
}

template<class T>
inline Iterator<T> OffsetBasedContainer<T>::end()
{
	return Iterator<T>(std::numeric_limits<uint32_t>::max(), nullptr);
}

template<class T>
inline void OffsetBasedContainer<T>::createFreeCluster()
{
	// Compute new size
	uint32_t new_size = m_size ? (m_size << 1) : 1;

	// Allocate memory
	m_data = static_cast<Slot<T>*>(realloc(m_data, new_size * sizeof(Slot<T>)));

	// Create the cluster
	m_first_free = new SlotCluster(m_size, new_size - 1);
	m_first_free->m_prev = m_first_objt;

	// Initialize new cluster
	for (uint32_t i(m_size); i < new_size; ++i)
	{
		m_data[i].m_cluster = m_first_free;
	}

	// Debug
#ifdef DEBUG
	std::cout << "[+] Create new free cluster [" << m_first_free->m_frst << ", " << m_first_free->m_last << "]" << std::endl;
#endif

	// Update size
	m_size = new_size;
}

template<class T>
inline void OffsetBasedContainer<T>::createObjtCluster()
{
	uint32_t first = m_first_free->m_frst;
	m_first_objt = new SlotCluster(first, first, false);
	m_first_objt->m_next = m_first_free;
	m_first_free->m_prev = m_first_objt;

	// Debug
#ifdef DEBUG
	std::cout << "[+] Create new objt cluster [" << m_first_objt->m_frst << ", " << m_first_objt->m_last << "]" << std::endl;
#endif
}

template<class T>
inline void OffsetBasedContainer<T>::removeCluster(SlotCluster* cluster)
{
#ifdef DEBUG
	std::cout << "[-] Removed cluster [" << cluster->m_frst << ", " << cluster->m_last << "]" << std::endl;
#endif

	if (cluster->m_prev)
	{
		cluster->m_prev->m_next = cluster->m_next;
	}

	if (cluster->m_next)
	{
		cluster->m_next->m_prev = cluster->m_prev;
	}

	if (cluster == m_first_free)
	{
		m_first_free = nullptr;
	}

	delete cluster;
}


} // Namespace's end

