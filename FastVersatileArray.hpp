#pragma once
#include <vector>
#include <list>

namespace fva
{

template<typename T> class SwapArray;

template<typename T>
class Handle
{
public:
	Handle() :
		m_index(0),
		m_source(nullptr)
	{}

	Handle(uint32_t index, SwapArray<T>& source) :
		m_index(index),
		m_source(&source)
	{}

	operator bool() const
	{
		return m_source;
	}

	T& operator*()
	{
		return (*m_source)[m_index];
	}

	const T& operator*() const
	{
		return (*m_source)[m_index];
	}

	T* operator->()
	{
		return &(this->operator*());
	}

	const T* operator->() const
	{
		return &(this->operator*());
	}

private:
	uint32_t m_index;
	SwapArray<T>* m_source;

	friend SwapArray<T>;
};



template<class T>
class SwapArray
{
public:
	SwapArray() = default;

	template<typename... Args>
	Handle<T> add(Args&&...);
	void remove(Handle<T>& handle);
	void remove(uint32_t index);

	T& operator[](uint32_t index);
	T& operator[](const Handle<T>& handle);

	typename std::vector<T>::iterator begin();
	typename std::vector<T>::iterator end();

	typename std::vector<T>::const_iterator begin() const;
	typename std::vector<T>::const_iterator end() const;

	uint32_t size() const;
	void     clear();

private:
	std::vector<T>        m_data;
	std::vector<uint32_t> m_index;
	std::vector<uint32_t> m_reverse_index;
	std::list<uint32_t>   m_free_indexes;
};

template<class T>
template<typename... Args>
inline Handle<T> SwapArray<T>::add(Args&&... args)
{
	// Compute data_index (index in _data) and init index (index for access from outside)
	uint32_t data_index = uint32_t(m_data.size());
	uint32_t index = data_index;

	// If empty slots in _index
	if (!m_free_indexes.empty())
	{
		// Get a free slot
		index = m_free_indexes.front();
		m_free_indexes.pop_front();
		// And link it to the new object
		m_index[index] = data_index;
	}
	else
	{
		// Else create a new one
		m_index.push_back(index);
	}
	
	// Add object and reverse index
	m_data.emplace_back(args...);
	m_reverse_index.push_back(index);

	return Handle<T>(index, *this);
}

template<class T>
inline void SwapArray<T>::remove(uint32_t index)
{
	uint32_t index_remove = m_index[index];
	// The object to remove
	T& removed_object = m_data[index_remove];
	// The current last object
	T& last_object = m_data.back();

	// The position of the last object in the index vector
	uint32_t last_object_index = m_reverse_index.back();
	// Update index vector
	m_index[last_object_index] = index;

	if (index_remove != m_data.size() - 1)
	{
		// Swap
		std::swap(m_reverse_index.back(), m_reverse_index[index_remove]);
		std::swap(removed_object, last_object);
	}

	// Add the free index in the list
	m_free_indexes.push_back(index);

	// Erase
	m_reverse_index.pop_back();
	m_data.pop_back();
}

template<class T>
inline void SwapArray<T>::remove(Handle<T>& handle)
{
	remove(handle.m_index);

	handle.m_index = 0;
	handle.m_source = nullptr;
}

template<class T>
inline T& SwapArray<T>::operator[](const Handle<T>& handle)
{
	return this->operator[](handle.m_index);
}

template<class T>
inline T& SwapArray<T>::operator[](uint32_t index)
{
	const uint32_t data_index(m_index[index]);
	return m_data[data_index];
}

template<class T>
inline typename std::vector<T>::iterator SwapArray<T>::begin()
{
	return m_data.begin();
}

template<class T>
inline typename std::vector<T>::iterator SwapArray<T>::end()
{
	return m_data.end();
}

template<class T>
inline typename std::vector<T>::const_iterator SwapArray<T>::begin() const
{
	return m_data.begin();
}

template<class T>
inline typename std::vector<T>::const_iterator SwapArray<T>::end() const
{
	return m_data.end();
}

template<class T>
inline uint32_t SwapArray<T>::size() const
{
	return uint32_t(m_data.size());
}

template<class T>
inline void SwapArray<T>::clear()
{
	m_data.clear();
	m_index.clear();
	m_reverse_index.clear();
	m_free_indexes.clear();
}

} // Namespace's end
