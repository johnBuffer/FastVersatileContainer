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
		m_frst(first),
		m_last(last),
		m_next(nullptr),
		m_prev(nullptr)
	{}

	uint32_t size() const
	{
		return m_last - m_frst + 1;
	}

private:
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
	template<class>
	friend class OffsetBasedContainer;

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

	T* operator->()
	{
		return &m_data_ptr[m_index].m_object;
	}

	void operator++()
	{
		if (m_index < m_cluster_end)
		{
			++m_index;
		}
		else
		{
			SlotCluster* curt = m_data_ptr[m_index].m_cluster;
			SlotCluster* next = curt->m_next;
			if (next)
			{
				if (next->m_next)
				{
					m_index = next->m_next->m_frst;
					m_cluster_end = next->m_next->m_last;
					return;
				}
			}
			
			m_index = std::numeric_limits<uint32_t>::max();
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
	void remove(Iterator<T>& it);

	Iterator<T> begin();
	Iterator<T> end();

	uint32_t size() const;

private:
	// Data info
	uint32_t m_size;
	uint32_t m_allocated;
	Slot<T>* m_data; 

	SlotCluster* m_slots_head;
	SlotCluster* m_first_free;
	SlotCluster* m_first_objt;

	void allocateMemory();
	void createObjtCluster();
	void removeCluster(SlotCluster* cluster);
	void fuseClusters(SlotCluster* host_cluster, SlotCluster* cluster);
	void splitCluster(SlotCluster* cluster, uint32_t around_index);
	void insertClusterBefore(SlotCluster* cluster, SlotCluster* before);
	void insertClusterAfter(SlotCluster* cluster, SlotCluster* after);
};

template<class T>
inline OffsetBasedContainer<T>::OffsetBasedContainer() :
	m_size(0),
	m_allocated(0),
	m_slots_head(nullptr),
	m_first_free(nullptr),
	m_first_objt(nullptr)
{}

template<class T>
inline void OffsetBasedContainer<T>::add(const T& object)
{
	// No free slots available
	if (!m_first_free)
	{
		allocateMemory();
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

	++m_size;
}

template<class T>
inline void OffsetBasedContainer<T>::remove(Iterator<T>& it)
{
	uint32_t index = it.m_index;

	if (index == std::numeric_limits<uint32_t>::max())
		return;

	++it;

	Slot<T>& slot        = m_data[index];
	SlotCluster& cluster = *slot.m_cluster;

	// Destroy the object
	slot.m_object.~T();

	// The slot is the first 
	if (index == cluster.m_frst)
	{
		// If previous cluster, add to it
		if (cluster.m_prev)
		{
			++(cluster.m_prev->m_last);
			++(cluster.m_frst);

			slot.m_cluster = cluster.m_prev;
		}
		// Else create new free cluster
		else
		{
			SlotCluster* new_cluster = new SlotCluster(index, index);
			// Connect it with the previous cluster
			new_cluster->m_next = &cluster;
			cluster.m_prev      = new_cluster;
			m_first_free        = new_cluster;
		}
	}
	else if (index == cluster.m_last)
	{
		// If previous cluster, add to it
		if (cluster.m_next)
		{
			--(cluster.m_next->m_frst);
			--(cluster.m_last);
			slot.m_cluster = cluster.m_prev;
		}
		// Else create new free cluster
		else
		{
			SlotCluster* new_cluster = new SlotCluster(index, index);
			// Connect it with the previous cluster
			new_cluster->m_prev = &cluster;
			cluster.m_next = new_cluster;

			if (!m_first_free)
				m_first_free = new_cluster;
		}
	}
	else
	{
		splitCluster(&cluster, index);
		SlotCluster* new_cluster = m_data[index].m_cluster;
		if (!m_first_free)
		{
			m_first_free = new_cluster;
		}
		else if (m_first_free->m_frst > index)
		{
			m_first_free = new_cluster;
		}
	}

	// Remove cluster if empty
	if (cluster.m_frst > cluster.m_last)
	{
		removeCluster(&cluster);
	}

	--m_size;
}

template<class T>
inline Iterator<T> OffsetBasedContainer<T>::begin()
{
	if (m_first_objt)
		return Iterator<T>(m_first_objt->m_frst, m_data);
	return end();
}

template<class T>
inline Iterator<T> OffsetBasedContainer<T>::end()
{
	return Iterator<T>(std::numeric_limits<uint32_t>::max(), nullptr);
}

template<class T>
inline uint32_t OffsetBasedContainer<T>::size() const
{
	return m_size;
}

template<class T>
inline void OffsetBasedContainer<T>::allocateMemory()
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
	//std::cout << "[+] Create new free cluster [" << m_first_free->m_frst << ", " << m_first_free->m_last << "]" << std::endl;

	// Update size
	m_allocated = new_size;
}

template<class T>
inline void OffsetBasedContainer<T>::createObjtCluster()
{
	uint32_t first = m_first_free->m_frst;
	m_first_objt = new SlotCluster(first, first, false);
	m_first_objt->m_next = m_first_free;
	m_first_free->m_prev = m_first_objt;

	// Debug
	//std::cout << "[+] Create new objt cluster [" << m_first_objt->m_frst << ", " << m_first_objt->m_last << "]" << std::endl;
}

template<class T>
inline void OffsetBasedContainer<T>::removeCluster(SlotCluster* cluster)
{
	//std::cout << "[-] Removed cluster [" << cluster->m_frst << ", " << cluster->m_last << "]" << std::endl;

	// If cluster has two neighbours
	if (cluster->m_prev && cluster->m_next)
	{
		// The biggest cluster will be the host
		if (cluster->m_prev->size() > cluster->m_next->size())
		{
			fuseClusters(cluster->m_prev, cluster->m_next);
		}
		else
		{
			fuseClusters(cluster->m_next, cluster->m_prev);
		}
	}
	// Check for prev
	else if (cluster->m_prev)
	{
		cluster->m_prev->m_next = cluster->m_next;
	}
	// check
	else if (cluster->m_next)
	{
		cluster->m_next->m_prev = cluster->m_prev;
	}

	if (cluster == m_first_free)
	{
		if (cluster->m_next)
			m_first_free = cluster->m_next->m_next;
		else
			m_first_free = nullptr;
	}
	
	else if (cluster == m_first_objt)
	{
		if (cluster->m_next)
			m_first_objt = cluster->m_next->m_next;
		else
			m_first_objt = nullptr;
	}

	delete cluster;
}

template<class T>
inline void OffsetBasedContainer<T>::fuseClusters(SlotCluster* host_cluster, SlotCluster* cluster)
{
	// Add all cluster's slots to host_cluster
	for (uint32_t i(cluster->m_frst); i<=cluster->m_last; ++i)
	{
		m_data[i].m_cluster = host_cluster;
	}

	// Check if cluster is before host_cluster
	if (cluster->m_frst < host_cluster->m_frst)
	{
		// Update host_cluster's prev
		host_cluster->m_prev = cluster->m_prev;
	}
	// If not
	else
	{
		// Update host_cluster's next
		host_cluster->m_next = cluster->m_next;
	}

	// Updates heads if needed
	if (cluster == m_first_free)
	{
		m_first_free = host_cluster;
	}
	else if (cluster == m_first_objt)
	{
		m_first_objt = host_cluster;
	}

	delete cluster;
}

template<class T>
inline void OffsetBasedContainer<T>::splitCluster(SlotCluster* cluster, uint32_t around_index)
{
	uint32_t frst = cluster->m_frst;
	uint32_t last = cluster->m_last;

	SlotCluster* new_objt_cluster = new SlotCluster(0, 0);
	SlotCluster* new_free_cluster = new SlotCluster(around_index, around_index);

	if (around_index - frst < last - around_index)
	{
		new_objt_cluster->m_frst = frst;
		new_objt_cluster->m_last = around_index - 1;
		cluster->m_frst = around_index + 1;

		/*new_objt_cluster->m_next = new_free_cluster;
		new_objt_cluster->m_prev = cluster->m_prev;

		new_free_cluster->m_next = cluster;
		new_free_cluster->m_prev = new_objt_cluster;

		cluster->m_prev = new_free_cluster;*/

		insertClusterBefore(new_objt_cluster, cluster);
		insertClusterBefore(new_free_cluster, cluster);

		if (cluster == m_first_objt)
			m_first_objt = new_objt_cluster;
	}
	else
	{
		new_objt_cluster->m_frst = around_index + 1;
		new_objt_cluster->m_last = last;

		/*new_objt_cluster->m_next = cluster->m_next;
		new_objt_cluster->m_prev = new_free_cluster;

		new_free_cluster->m_next = new_objt_cluster;
		new_free_cluster->m_prev = cluster;

		cluster->m_next = new_free_cluster;*/

		insertClusterAfter(new_objt_cluster, cluster);
		insertClusterAfter(new_free_cluster, cluster);

		cluster->m_last = around_index - 1;
	}

	m_data[around_index].m_cluster = new_free_cluster;

	for (uint32_t i(new_objt_cluster->m_frst); i <= new_objt_cluster->m_last; ++i)
	{
		m_data[i].m_cluster = new_objt_cluster;
	}
}

template<class T>
inline void OffsetBasedContainer<T>::insertClusterBefore(SlotCluster* cluster, SlotCluster* before)
{
	if (before->m_prev)
	{
		before->m_prev->m_next = cluster;
	}

	cluster->m_prev = before->m_prev;
	cluster->m_next = before;

	before->m_prev = cluster;
}

template<class T>
inline void OffsetBasedContainer<T>::insertClusterAfter(SlotCluster* cluster, SlotCluster* after)
{
	if (after->m_next)
	{
		after->m_next->m_prev = cluster;
	}

	cluster->m_prev = after;
	cluster->m_next = after->m_next;

	after->m_next = cluster;
}


} // Namespace's end

