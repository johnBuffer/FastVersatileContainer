#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <string>
#include "FastVersatileArray.hpp"
#include "fast_versatile_container.hpp"
#include "ezbench.hpp"

struct TestStruct
{
	double ddd[20];

	TestStruct()
	{
	}

	TestStruct(int i)
	{
		ddd[1] = rand() % 10000;
		ddd[2] = i;
	}

	void increaseA()
	{
		ddd[0]++;
	}
};

void cbcAdd(fvc::Container<TestStruct>* pool, uint32_t add_count)
{
	srand(0);
	for (int i(add_count); i--;)
	{
		pool->add(TestStruct(i));
	}
}

void vecAdd(std::vector<TestStruct>* vec, uint32_t add_count)
{
	srand(0);
	for (int i(add_count); i--;)
	{
		vec->push_back(TestStruct(i));
	}
}

void listAdd(std::list<TestStruct>* list, uint32_t add_count)
{
	srand(0);
	for (int i(add_count); i--;)
	{
		list->push_back(TestStruct(i));
	}
}

void cbcIter(fvc::Container<TestStruct>* container)
{
	uint32_t i(0);
	for (TestStruct& ts : *container)
	{
		++i;
		ts.increaseA();
	}
}

void vecIter(std::vector<TestStruct>* container)
{
	for (TestStruct& ts : *container)
	{
		ts.increaseA();
	}
}

void listIter(std::list <TestStruct>* container)
{
	for (TestStruct& ts : *container)
	{
		ts.increaseA();
	}
}

void cbcDel(fvc::Container<TestStruct>* container)
{
	for (auto it(container->begin()); it != container->end(); ) {
		if (it->ddd[1] == 0) {
			container->remove(it);
		}
		else {
			++it;
		}
	}
}

void vecDel(std::vector<TestStruct>* container)
{
	for (auto it(container->begin()); it != container->end(); ) {
		if (it->ddd[1] == 0) {
			it = container->erase(it);
		}
		else {
			++it;
		}
	}
}

void listDel(std::list<TestStruct>* container)
{
	for (auto it(container->begin()); it != container->end(); ) {
		if (it->ddd[1] == 0) {
			it = container->erase(it);
		}
		else {
			++it;
		}
	}
}

int main()
{	
	fvc::Container<TestStruct> cbc;
	std::vector<TestStruct> vec;
	std::list<TestStruct> list;

	uint32_t size = 1000000;
	
	ezb::Benchmark bench {
		1,
		{
			{"Insertion",
				{
					{"Cluster", std::bind(cbcAdd,  &cbc, size)},
					{"Vector", std::bind(vecAdd, &vec,  size)},
					{"List", std::bind(listAdd,  &list, size)}
				}
			},
			{"Iteration",
				{
					{"Cluster", std::bind(cbcIter,  &cbc)},
					{"Vector", std::bind(vecIter, &vec)},
					{"List", std::bind(listIter,  &list)}
				}
			},
			{"Deletion",
				{
					{"Cluster", std::bind(cbcDel,  &cbc)},
					{"Vector", std::bind(vecDel, &vec)},
					{"List", std::bind(listDel,  &list)}
				}
			},
			{"Iteration 2",
				{
					{"Cluster", std::bind(cbcIter,  &cbc)},
					{"Vector", std::bind(vecIter, &vec)},
					{"List", std::bind(listIter,  &list)}
				}
			},
		}
	};

	//bench.run();

	FastArray<int> fa;

	for (int i(0); i < 10; ++i)
	{
		fa.add(i);
	}

	std::cout << "9 :" << fa[9] << std::endl;

	for (int i(0); i < 5; ++i)
	{
		fa.remove(i);
	}

	std::cout << "9 :" << fa[9] << std::endl;

	for (int i : fa)
	{
		std::cout << i << std::endl;
	}

	
	return 0;
}