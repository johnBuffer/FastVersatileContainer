#include <iostream>
#include "ObjectPool.hpp"
#include <chrono>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <string>

struct TestStruct
{
	double ddd[20];

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

		for (Competitor& competitor : competitors)
		{
			std::cout << "  " << competitor.name << ": " << competitor.score * 0.000001 << "ms" << std::endl;
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



// Functions
void poolAdd(ObjectPool<TestStruct>& pool, uint32_t add_count)
{
	for (int i(add_count); i--;)
	{
		pool.add(TestStruct()); 
	}
}

void vecAdd(std::vector<TestStruct>& vec, uint32_t add_count)
{
	for (int i(add_count); i--;)
	{
		vec.push_back(TestStruct());
	}
}

void listAdd(std::list<TestStruct>& list, uint32_t add_count)
{
	for (int i(add_count); i--;)
	{
		list.push_back(TestStruct());
	}
}

void poolIter(ObjectPool<TestStruct>& pool)
{
	for (TestStruct& ts : pool)
	{
		ts.increaseA();
	}
}

void vecIter(std::vector<TestStruct>& pool)
{
	for (TestStruct& ts : pool)
	{
		ts.increaseA();
	}
}

void listIter(std::list <TestStruct>& pool)
{
	for (TestStruct& ts : pool)
	{
		ts.increaseA();
	}
}

int main()
{	
	ObjectPool<TestStruct> pool;
	std::vector<TestStruct> vec;
	std::list<TestStruct> list;
	
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
	double duration;
	
	Benchmark bench {
		1,
		{
			{"Insertion",
				{
					{"Pool", std::bind(poolAdd, pool, 1000000)},
					{"Vector", std::bind(vecAdd, vec, 1000000)},
					{"List", std::bind(listAdd, list, 1000000)}
				}
			},
			{"Iteration",
				{
					{"Pool", std::bind(poolIter, pool)},
					{"Vector", std::bind(vecIter, vec)},
					{"List", std::bind(listIter, list)}
				}
			}
		}
	};

	bench.run();

	/*duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	// Insertion
	std::cout << "Insertion" << std::endl;
	begin = std::chrono::steady_clock::now();


	for (int i(1000000); i--;)
	{
		pool.add(TestStruct());
	}
	end = std::chrono::steady_clock::now();

	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	/*begin = std::chrono::steady_clock::now();
	for (int i(10000000); i--;)
	{
		vec.push_back(TestStruct());
	}
	end = std::chrono::steady_clock::now();*/

	//duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	//std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	/*begin = std::chrono::steady_clock::now();
	for (int i(10000000); i--;)
	{
		list.push_back(TestStruct());
	}
	end = std::chrono::steady_clock::now();*/

	/*duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	// Deletion
	std::cout << "\nDeletion" << std::endl;
	uint32_t del = 0;
	begin = std::chrono::steady_clock::now();
	PoolIterator<TestStruct> it_pool(pool.end());
	--it_pool;

	for (it_pool; it_pool != pool.end(); ++it_pool)
	{
		std::cout << it_pool.index() << std::endl;
		if (del++ % 1000 == 0)
		{
 			pool.remove(it_pool);
			std::cout << it_pool.index() << std::endl << std::endl;
		}
	}
	end = std::chrono::steady_clock::now();

	duration = std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count();
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;

	del = 1;
	begin = std::chrono::steady_clock::now();
	std::vector<TestStruct>::iterator it_vec(vec.begin());
	for (it_vec; it_vec != vec.end(); ++it_vec)
	{
		if (del++ % 10000 == 0)
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
	std::cout << "Time difference = " << duration / 1000000.0 << std::endl;*/
	
	return 0;
}