#pragma once
// bootstrapping class
#include <map>
#include <string>
#include <vector>

#include "stock.h" // stock.h header
#include "group.h" // group.h header
#include "csv.h"  
#include "global.h"


using namespace std;

//map <string, stock > global_stock; //global variable
//int N_days; //global variable

class Bootstrapping
{
private:
	int N_samples; // the number of samplings in bootstrapping
	
	map <string, vector<group>> my_map;// storage 3 groups results
	map<int, string> group_classification;   // three different groups
	int group_size;
	
	int N; // the number of days in stock;

	vector<Matrix> result_matrix;//
	//result_matrix[0]=Ave AAR
	//result_matrix[1]=Ave CAAR
	//result_matrix[2]=    AAR-STD
	//result_matrix[3]=    CAAR-STD

	vector<vector<double>> Ave_AAR;
	vector<vector<double>> Ave_CAAR;
	vector<vector<double>> AAR_STD;
	vector<vector<double>> CAAR_STD;
public:
	Bootstrapping();
	void Set_N_samples(int N_);
	void Set_N(global_constant &g);
	void set_group_size(int N_) { group_size = N_; }

	void Repeat(global_constant &g);
	void Get_stocks_from_global(const vector<string> & current_stock_tickers, map <string, stock> & current_stock,
		global_constant& g);
	// check whether the stock tickers are in the global stocks; set up the stock map

	void cal_get_average_AAR();
	vector<vector<double>>  get_average_AAR();

	void cal_get_average_CAAR();
	vector<vector<double>>  get_average_CAAR();

	vector<vector<double>>get_AARstd()
	{
		return AAR_STD;
	};
	void  cal_get_AARstd();

	vector<vector<double>>  get_CAARstd()
	{
		return CAAR_STD;
	};
	void cal_get_CAARstd();
	void cal_result_matrix();
	vector<Matrix> get_result_matrix() {
		return result_matrix;
	}
};