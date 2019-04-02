#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <functional>
#include <algorithm>
#include <string>
#include "FastVersatileArray.hpp"
#include "ezbench.hpp"

int main()
{	
	fva::SwapArray<int> fa;

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