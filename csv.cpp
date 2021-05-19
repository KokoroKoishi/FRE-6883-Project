#include"csv.h"

#include<time.h>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <locale>
#include <iomanip>
#include <fstream>
#include<map>
using namespace std;

void csv::store_ticker() {
	s_t.resize(3);
	ifstream fin0;
	fin0.open("miss_stocks.txt", ios::in);
	string line, symbol;

	while (!fin0.eof())
	{
		getline(fin0, line);
		stringstream sin(line);
		getline(sin, symbol, '\t');
		s_t[0].push_back(symbol);
	}
	ifstream fin1;
	fin1.open("beat_stocks.txt", ios::in);
	string line2, symbol2;

	while (!fin1.eof())
	{
		getline(fin1, line2);
		stringstream sin(line2);
		getline(sin, symbol2, '\t');
		s_t[1].push_back(symbol2);
	}
	ifstream fin2;
	fin2.open("meet_stocks.txt", ios::in);
	string line3, symbol3;

	while (!fin2.eof())
	{
		getline(fin2, line3);
		stringstream sin(line3);
		getline(sin, symbol3, '\t');
		s_t[2].push_back(symbol3);
	}

}

vector<string> csv::random(string key)
{
	vector<string> required_symbols;
	vector<string> keys(3);
	keys[0] = "miss";
	keys[1] = "beat";
	keys[2] = "target";
	//srand(time(NULL));
	for (int i = 0; i < 3; i++)
	{
		if (key == keys[i])
		{
			
			int size = s_t[i].size();
			vector<int> temp;
			for (int k = 0; k < size; k++)
			{
				temp.push_back(k);
			}

			random_shuffle(temp.begin(), temp.end());

			for (int j = 0; j < group_size; j++)
			{
				required_symbols.push_back(s_t[i][temp[j]]);
			}
		}
	}
	cout << "required_symbols size: "<< required_symbols.size() << endl;
	return required_symbols;
}
