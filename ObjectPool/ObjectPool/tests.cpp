#include <iostream>
#include "ObjectPool.hpp"
#include <chrono>
#include <vector>
#include <list>

struct TestStruct
{
	uint32_t a, b, c, d, e, f, g, h, i, j, k, l;
	double ddd[10];

	void increaseA()
	{
		a++;
	}
};

int main()
{
	ObjectPool<TestStruct> pool;
	std::vector<TestStruct> vec;
	std::list<TestStruct> list;
	
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
	double duration;
	
	// Insertion
	std::cout << "Insertion" << std::endl;
	begin = std::chrono::steady_clock::now();
	for (int i(10000000); i--;)
	{
		pool.add(TestStruct());
	}
	end = std::chrono::steady_clock::now();

	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	begin = std::chrono::steady_clock::now();
	for (int i(10000000); i--;)
	{
		vec.push_back(TestStruct());
	}
	end = std::chrono::steady_clock::now();

	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	begin = std::chrono::steady_clock::now();
	for (int i(10000000); i--;)
	{
		list.push_back(TestStruct());
	}
	end = std::chrono::steady_clock::now();

	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	// Deletion
	std::cout << "\nDeletion" << std::endl;
	uint32_t del = 0;
	begin = std::chrono::steady_clock::now();
	PoolIterator<TestStruct> it_pool(pool.begin());
	for (it_pool; it_pool != pool.end(); ++it_pool)
	{
		if (del++ % 1000 == 0)
		{
			pool.remove(it_pool);
		}
	}
	end = std::chrono::steady_clock::now();

	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	del = 0;
	begin = std::chrono::steady_clock::now();
	std::vector<TestStruct>::iterator it_vec(vec.begin());
	for (it_vec; it_vec != vec.end(); ++it_vec)
	{
		if (del++ % 1000 == 0)
		{
			vec.erase(it_vec);
		}
	}
	end = std::chrono::steady_clock::now();

	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	// Iteration
	std::cout << "\nIteration" << std::endl;
	begin = std::chrono::steady_clock::now();
	for (TestStruct& i : pool)
	{
		i.increaseA();
	}
	end = std::chrono::steady_clock::now();
	
	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;


	begin = std::chrono::steady_clock::now();
	for (TestStruct& i : vec)
	{
		i.increaseA();
	}
	end = std::chrono::steady_clock::now();

	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	begin = std::chrono::steady_clock::now();
	for (TestStruct& i : list)
	{
		i.increaseA();
	}
	end = std::chrono::steady_clock::now();

	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;
	
	return 0;
}