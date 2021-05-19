#pragma once

#include<iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include<vector>
#include<string>
#include<map>
#include<time.h>
#include"stock.h"


using namespace std;
class csv
{
private:
	vector<vector<string>> s_t;
	int group_size;
public:
	void store_ticker();
	void set_group_size(int n) { group_size = n; }
	vector<string> random(string key);
	// based on key, output 50 random stock tickers from key vectors
	//key is one of ["miss","target", "beat"]

};