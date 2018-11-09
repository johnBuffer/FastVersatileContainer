#include <iostream>
#include "ObjectPool.hpp"

int main()
{
	ObjectPool<int> pool;
	
	for (int i(10); i--;)
	{
		pool.add(i);
	}

	PoolIterator<int> it(pool.begin());
	do
	{
		std::cout << *it << std::endl;
	} while (it.getNext());

	for (int& i : pool)
	{
		std::cout << i << std::endl;
	}
	
	return 0;
}