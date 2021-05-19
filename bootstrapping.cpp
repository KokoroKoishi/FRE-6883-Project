
#include "bootstrapping.h" 
#include <iostream>
#include <cmath>
Bootstrapping::Bootstrapping()
{
	group_classification[0] = "miss";
	group_classification[1] = "target";
	group_classification[2] = "beat";
	N_samples = 0;
	N = 0;
	
}
void Bootstrapping::Set_N(global_constant& g)
{
	N = g.N_days;
}

void Bootstrapping::Set_N_samples(int N_)
{
	N_samples = N_;
	my_map["miss"].resize(N_samples);
	my_map["target"].resize(N_samples);
	my_map["beat"].resize(N_samples);
	
}

 void  Bootstrapping::Repeat(global_constant& global)
{
	if (N_samples == 0)
	{
		cout << "Warning: Did not set the number of samplings N_samples" << endl;
	}// make sure initialize N

	csv csv_object;
	csv_object.set_group_size(group_size);
	csv_object.store_ticker();
	for (int i = 0; i < N_samples; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			map <string, stock> current_stock; //  storage the the stock return  we need for each group in one loop
			vector<string> current_stock_tickers;      // storage the the stock tickers we need for each group in one loop
			cout << "Sample: "<< i << ", Group: " << j << endl;
			string group_name = group_classification[j];
			current_stock_tickers = csv_object.random(group_name);
			Get_stocks_from_global(current_stock_tickers, current_stock, global);
			group g(N);
			g.set_map(current_stock);// set the group with the current stock map
			my_map[group_name][i] = g;
		}
		
		
	}
}

void Bootstrapping::Get_stocks_from_global(const vector<string>& current_stock_tickers, map <string, stock>& current_stock,
	global_constant& g)
{
	string stock_name;
	map <string, stock>::iterator itr;
	for (int i = 0; i < current_stock_tickers.size(); i++)
	{
		stock_name = current_stock_tickers[i];
		/*cout << "stock_name = current_stock_tickers[i]::" << stock_name << endl;*/
		itr = g.global_stock.find(stock_name);
		if (itr != g.global_stock.end())
		{
			current_stock[stock_name] = g.global_stock[stock_name];
		}
		else
		{
			cout << "Stocks Not In Global" << endl;
			current_stock[stock_name] = g.global_stock["SGEN"];
			//s.get_return_from_libcurl(stock_name);
			//
           //function: using libcurl function to get all return information we need for stock(stock_name)
           //also after this function, the private member of this stock object has been initialized

			//global_stock[stock_name] = s;
			//current_stock[stock_name] = s;
		}
	}
}
void Bootstrapping::cal_get_average_AAR()
{
	vector<vector<double>> result(3);

	for (int j = 0; j < 3; j++)
	{
		vector<double> sum(2*N+1);
		for (int i = 0; i < sum.size(); i++)
		{
			sum[i]=0.0;
		}
		string group_name = group_classification[j];

		for (int i = 0; i < N_samples; i++)
		{
			my_map[group_name][i].cal_GetAAR(N);
			sum = sum + my_map[group_name][i].GetAAR();
		}
		result[j] = sum / N_samples;
	}
	Ave_AAR = result;
}
vector<vector<double>> Bootstrapping::get_average_AAR()
{
	return Ave_AAR;
}

void Bootstrapping::cal_get_average_CAAR()
{
	vector<vector<double>> result(3);
	for (int j = 0; j < 3; j++)
	{
		vector<double> sum(2 * N + 1);
		for (int i = 0; i < sum.size(); i++)
		{
			sum[i] = 0;
		}
		string group_name = group_classification[j];
		for (int i = 0; i < N_samples; i++)
		{

			my_map[group_name][i].cal_GetCAAR();
			sum = sum + my_map[group_name][i].GetCAAR();
		}
		result[j] = sum / N_samples;
	}
	Ave_CAAR = result;

}
vector<vector<double>>  Bootstrapping::get_average_CAAR()
{
	return Ave_CAAR;
}

void  Bootstrapping::cal_get_CAARstd()
{
	vector<vector<double>>  result(3);
	//vector<vector<double>> AVE_CAAR = get_average_CAAR(g);
	for (int j = 0; j < 3; j++)
	{
		vector<double> ave = Ave_CAAR[j];
		vector<double> sum_square(2*N+1);
		for (int i = 0; i < sum_square.size(); i++)
		{
			sum_square[i] = 0;
		}
		string group_name = group_classification[j];
		for (int i = 0; i < N_samples; i++)
		{
			sum_square = sum_square + (my_map[group_name][i].GetCAAR()- ave)^(2.0);
		}
		result[j] = (sum_square / N_samples) ^ (0.5);
	}
	CAAR_STD=result;
}

void  Bootstrapping::cal_get_AARstd()
{
	vector<vector<double>>  result(3);
	
	for (int j = 0; j < 3; j++)
	{
		vector<double> ave = Ave_AAR[j];
		vector<double> sum_square(2*N+1);
		for (int i = 0; i < sum_square.size(); i++)
		{
			sum_square[i] = 0;
		}
		string group_name = group_classification[j];
		for (int i = 0; i < N_samples; i++)
		{
			sum_square = sum_square + (my_map[group_name][i].GetAAR() - ave) ^ (2.0);
		}
		result[j] = (sum_square / N_samples) ^ (0.5);
	}
	AAR_STD=result;
}

void Bootstrapping::cal_result_matrix()
{
	result_matrix.resize(4);

	cal_get_average_AAR();
	result_matrix[0] = get_average_AAR();

	cal_get_average_CAAR();
	result_matrix[1] = get_average_CAAR();

	cal_get_AARstd();
	result_matrix[2] = get_AARstd();

	cal_get_CAARstd();
	result_matrix[3] = get_CAARstd();
}