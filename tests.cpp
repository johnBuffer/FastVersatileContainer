#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <string>
#include "fast_versatile_container.hpp"

struct TestStruct
{
	double ddd[20];

	TestStruct()
	{
	}

	TestStruct(int i)
	{
		ddd[1] = rand() % 100;
		ddd[2] = i;
	}

	void increaseA()
	{
		ddd[0]++;
	}
};

struct Competitor
{
	std::string name;
	std::function<void()> function;
	double score;

	template<class T>
	Competitor(const std::string& name_, const T& function_) :
		name(name_),
		function(function_),
		score(0.0)
	{}

	void run()
	{
		std::chrono::steady_clock::time_point begin(std::chrono::steady_clock::now());
		std::chrono::steady_clock::time_point end;

		function();
		
		end = std::chrono::steady_clock::now();
		score = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	}
};

struct Challenge
{
	std::string name;
	std::vector<Competitor>	competitors;

	Challenge(const std::string& name_, std::vector<Competitor>	competitors_) :
		name(name_),
		competitors(competitors_)
	{}

	void run()
	{
		std::cout << "-----[ " << name << " ]-----" << std::endl;
		for (Competitor& competitor : competitors)
		{
			competitor.run();
		}

		std::sort(competitors.begin(), competitors.end(), [=](const Competitor& c1, const Competitor& c2) {return c1.score < c2.score; });
		double min = competitors[0].score;

		for (Competitor& competitor : competitors)
		{
			std::cout << "  " << competitor.name << ": " << competitor.score * 0.000001 << "ms (" << int(competitor.score / min * 100) << " %)" << std::endl;
		}
	}
};

struct Benchmark
{
	uint32_t iteration_count = 1;
	std::vector<Challenge> challenges;
	std::vector<double> durations;

	Benchmark(uint32_t iteration_count_, const std::vector<Challenge>& challenges_) :
		iteration_count(iteration_count_),
		challenges(challenges_)
	{}

	void run()
	{
		for (uint32_t i(iteration_count); i--;)
		{
			for (Challenge& challenge : challenges)
			{
				challenge.run();
				std::cout << std::endl;
			}
		}
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
	for (TestStruct& ts : *container)
	{
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
	
	Benchmark bench {
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

	bench.run();
	
	return 0;
}