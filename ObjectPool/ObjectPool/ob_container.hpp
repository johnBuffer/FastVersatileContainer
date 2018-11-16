#pragma once

namespace clstr
{


class SlotCluster
{
public:
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
class OffsetBasedContainer
{
public:
	OffsetBasedContainer();

	void add(T& object);

private:
	// Data info
	uint32_t      m_size;
	Slot<T> m_data[10];

	//Chain info
	uint32_t m_off_free;
	uint32_t m_off_objt;

	SlotCluster* m_slots_head;
	SlotCluster* m_first_free;
	SlotCluster* m_first_objt;

	void createCluster();
	void removeCluster(SlotCluster* cluster);
};

template<class T>
inline OffsetBasedContainer<T>::OffsetBasedContainer() :
	m_off_free(1),
	m_off_objt(0)
{
	m_size = 10;
	initializeMemory();
}

template<class T>
inline void OffsetBasedContainer<T>::add(T& object)
{
	if (m_first_free) // Free slots still available
	{
		Slot<T>&     slot = m_data[m_first_free->m_frst];
		SlotCluster* prev = m_first_free->m_prev;

		if (prev) // If cluster before just add the new slot in it
		{
			// Update slot's cluster
			slot.m_cluster = prev;

			// Update ranges of the clusters
			++(prev->m_last);
			++(m_first_free->m_frst);

			// Check if the free cluster is now empty
			if (m_first_free->m_frst == m_first_free->m_last) 
			{
				removeCluster(m_first_free);
			}
		}
		else // If not create a new cluster and add 
		{
			uint32_t first = m_first_free->m_frst;
			m_first_free->m_prev = new SlotCluster();
		}
	}
	else // Need more space and a new slot
	{

	}
}

template<class T>
inline void OffsetBasedContainer<T>::createCluster()
{
	uint32_t new_size = m_size ? m_size << 1 : 1;

	m_data = realloc(m_data, new_size);
	m_first_free = new SlotCluster(m_size, new_size - 1);

	/*m_slots_head = new SlotCluster(0, m_size - 1);
	m_first_free = m_slots_head;
	m_first_objt = nullptr;*/

	for (uint32_t i(m_size); i < m_size; ++i)
	{
		m_data[i].m_cluster = m_slots_head;
	}
}

template<class T>
inline void OffsetBasedContainer<T>::removeCluster(SlotCluster * cluster)
{
}


} // Namespace's end

