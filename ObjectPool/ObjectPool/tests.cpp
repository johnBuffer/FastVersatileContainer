#include <iostream>
#include "ObjectPool.hpp"

int main()
{
	ObjectPool<int> pool;
	std::cout << "Size: " << pool.size() << std::endl;
	for (int i(0); i < 100; ++i)
	{
		pool.add(i);
		std::cout << "Add: " << i << std::endl;
	}
	std::cout << "Size: " << pool.size() << std::endl;

	for (int i(0); i < pool.size(); ++i)
	{
		std::cout << *pool[i] << std::endl;
	}

	std::cout << "Done." << std::endl;
	return 0;
}