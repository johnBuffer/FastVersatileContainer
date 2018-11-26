#pragma once
#include <vector>
#include <list>

template<class T>
class FastArray
{
public:
	FastArray() = default;

	uint32_t add(const T& object);
	void remove(uint32_t index);

	T& operator[](uint32_t index);

	typename std::vector<T>::iterator begin();
	typename std::vector<T>::iterator end();

	typename std::vector<T>::const_iterator cbegin() const;
	typename std::vector<T>::const_iterator cend() const;

	uint32_t size() const;
	void     clear();

private:
	std::vector<T> _data;
	std::vector<uint32_t> _index;
	std::vector<uint32_t> _reverse_index;

	std::list<uint32_t> _free_indexes;
};

template<class T>
inline uint32_t FastArray<T>::add(const T& object)
{
	// Compute data_index (index in _data) and init index (index for access from outside)
	uint32_t data_index = uint32_t(_data.size());
	uint32_t index = data_index;

	// If empty slots in _index
	if (!_free_indexes.empty())
	{
		// Get a free slot
		index = _free_indexes.front();
		_free_indexes.pop_front();
		// And link it to the new object
		_index[index] = data_index;
	}
	else
	{
		// Else create a new one
		_index.push_back(index);
	}
	
	// Add object and reverse index
	_data.push_back(object);
	_reverse_index.push_back(index);

	return index;
}

template<class T>
inline void FastArray<T>::remove(uint32_t index)
{
	uint32_t index_remove = _index[index];
	// The object to remove
	T& removed_object = _data[index_remove];
	// The current last object
	T& last_object = _data.back();
	// The position of the last object in the index vector
	uint32_t last_object_index = _reverse_index.back();
	// Update index vector
	_index[last_object_index] = index;

	// Swap
	std::swap(_reverse_index.back(), _reverse_index[index_remove]);
	std::swap(removed_object, last_object);

	// Erase
	_reverse_index.pop_back();
	_data.pop_back();

	// Add the free index in the list
	_free_indexes.push_back(index);
}

template<class T>
inline T& FastArray<T>::operator[](uint32_t index)
{
	const uint32_t data_index = _index[index];
	return _data[data_index];
}

template<class T>
inline typename std::vector<T>::iterator FastArray<T>::begin()
{
	return _data.begin();
}

template<class T>
inline typename std::vector<T>::iterator FastArray<T>::end()
{
	return _data.end();
}

template<class T>
inline typename std::vector<T>::const_iterator FastArray<T>::cbegin() const
{
	return _data.cbegin();
}

template<class T>
inline typename std::vector<T>::const_iterator FastArray<T>::cend() const
{
	return _data.cend();
}

template<class T>
inline uint32_t FastArray<T>::size() const
{
	return uint32_t(_data.size());
}

template<class T>
inline void FastArray<T>::clear()
{
	_data.clear();
	_index.clear();
	_reverse_index.clear();
	_free_indexes.clear();
}
