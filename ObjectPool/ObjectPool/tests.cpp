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

	while (it.getNext())
	{
		std::cout << *it << std::endl;
	}

	for (int& i : pool)
	{

	}
	
	return 0;
}