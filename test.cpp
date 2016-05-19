#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

int main()
{
	int * arr = new int[10];
	int * arr1 = new int[10];

	for (int i = 0; i<10; i++)
	{
		arr[i] = i;
	}
	arr1 = arr;

	arr1[1] = 99999;

	for (int i= 0; i< 10; i++)
	{
		std::cout << arr[i] << "  " << arr1[i] << std::endl;
	}

}