#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <locale>
#include <thread>
#include <iomanip>
#include "curl/curl.h"
#include <fstream>
#include <istream>
#include"stock.h"
#include<map>
#include "bootstrapping.h"
#include "Libcurl.h"  
#include "operator.h"  
#include"csv.h"
#include"group.h"
#include"Gnuplot_PC.h"

using namespace std;
typedef vector<double> Vector;
typedef vector<Vector> Matrix;

void input_N(global_constant& g)
{


	int N_;
	cout << "Please input N" << endl;
	cin >> N_;
	while ((N_ < 30) || ((N_ > 60)))
	{
		cout << "N should be greater than 30 and less than 60" << endl;
		cout << "Please continue to input N" << endl;
		cin >> N_;
	}
	g.N_days = N_;
}
vector<Matrix> transpose(vector<Matrix> amatrix) {
	vector<Matrix> result;

	for (int j = 0; j < amatrix[0].size(); j++) {
		Matrix vectors;
		for (int i = 0; i < amatrix.size(); i++) {
			Matrix curr = amatrix[i];
			vectors.push_back(curr[j]);
		}
		result.push_back(vectors);
	}

	//Matrix missvector;
	//for (int i = 0; i < amatrix.size(); i++) {
	//	Matrix curr = amatrix[i];
	//	missvector.push_back(curr[0]);
	//}
	//result.push_back(missvector);
	//Matrix meatvector;
	//for (int i = 0; i < amatrix.size(); i++) {
	//	Matrix curr = amatrix[i];
	//	meatvector.push_back(curr[1]);
	//}
	//result.push_back(meatvector);
	//Matrix beatvector;
	//for (int i = 0; i < amatrix.size(); i++) {
	//	Matrix curr = amatrix[i];
	//	beatvector.push_back(curr[2]);
	//}
	//result.push_back(beatvector);
	return result;
}

int main(void)

{
	char selection = NULL;
	Bootstrapping boot;
	global_constant g;
	bool runA = FALSE;
	bool runB = FALSE;

	string min_annouce;
	string max_annouce;
	while (TRUE)
	{
		cout << endl;
		cout << "Menu" << endl;
		cout << "======" << endl;
		cout << "A - Retrive Historical Price Data for All Stocks" << endl;
		cout << "B - Retrieve Benchmark Data and Calculate AAR and CAAR" << endl;
		cout << "C - Show AAR, AAR-SD, CAAR and CAAR-STD For Each Group" << endl;
		cout << "D - Show Graph with AAR, AAR-SD, CAAR and CAAR-STD" << endl;
		cout << "E - Pull Informations for One Stock" << endl;
		cout << "F - Exit" << endl;
		cin >> selection;
		switch (selection) {
			case 'A':
			{
				input_N(g);// let user unput the number of days
				map<string, stock> MissSymbols;
				map<string, stock> MeatSymbols;
				map<string, stock> BeatSymbols;

				thread counter1(ParseIntoMap, ref(MissSymbols), "miss_stocks.txt", g.N_days);
				thread counter2(ParseIntoMap, ref(MeatSymbols), "meet_stocks.txt", g.N_days);
				thread counter3(ParseIntoMap, ref(BeatSymbols), "beat_stocks.txt", g.N_days);
				counter1.join();
				counter2.join();
				counter3.join();
				for (map<string, stock>::iterator itr = MissSymbols.begin(); itr != MissSymbols.end(); itr++) {
					g.global_stock[itr->first] = itr->second;
				}

				for (map<string, stock>::iterator itr = MeatSymbols.begin(); itr != MeatSymbols.end(); itr++) {
					g.global_stock[itr->first] = itr->second;
				}

				for (map<string, stock>::iterator itr = BeatSymbols.begin(); itr != BeatSymbols.end(); itr++) {
					g.global_stock[itr->first] = itr->second;
				}
				for (map<string, stock>::iterator itr = g.global_stock.begin(); itr != g.global_stock.end(); itr++) {
					if (itr == g.global_stock.begin()) {
						min_annouce = itr->second.get_annouce_date();
						max_annouce = itr->second.get_annouce_date();
					}
					if (itr->second.get_annouce_date() < min_annouce) {
						min_annouce = itr->second.get_annouce_date();
					}
					if (itr->second.get_annouce_date() > max_annouce) {
						max_annouce = itr->second.get_annouce_date();
					}
				}
				cout << getDateAfterNday(min_annouce, -61) << endl;
				cout << getDateAfterNday(max_annouce, 61) << endl;
				cout << "Retrive data finished" << endl;
				runA = TRUE;
				break;
			}
			case 'B':
			{
				if (!runA){
					cout << "Please run step A first" << endl;
					break;
				}
				bool pass = TRUE;
				string benchmark = "IWB";
				int Nsamples;
				int group_size;
				cout << "Enter Ticker for Benchmark Stock:" << endl;
				cin >> benchmark;

				for (map<string, stock>::iterator i = g.global_stock.begin(); i != g.global_stock.end(); i++) {
					if (i->first == benchmark) {
						pass = FALSE;
					}
				}
				if (pass == FALSE) {
					cout << "Invalid Benchmark Name." << endl;
					cout << "Back to main menu." << endl;
					break;
				}

				cout << "Enter Sample Size:" << endl;
				cin >> Nsamples;
				cout << "Enter size for each group:" << endl;
				cin >> group_size;

				stock stock_ = stock(benchmark);

				download_data(stock_, getDateAfterNday(min_annouce, -61), getDateAfterNday(max_annouce, 61));

				g.global_stock[benchmark] = stock_;

				for (map<string, stock>::iterator itr = g.global_stock.begin(); itr != g.global_stock.end(); itr++) {
					itr->second.cal_return();
				}

				for (map<string, stock>::iterator itr = g.global_stock.begin(); itr != g.global_stock.end(); itr++) {
					itr->second.cal_ARIT(g.global_stock[benchmark]);
				}
				boot.Set_N(g);
				boot.Set_N_samples(Nsamples);
				boot.set_group_size(group_size);
				boot.Repeat(g);
				boot.cal_result_matrix();
				cout << "Calculation finished" << endl;
				runB = TRUE;
				break;
			}
			case 'C':
			{
				if (!runA) {
					cout << "Please run step A first" << endl;
					break;
				}
				if (!runB) {
					cout << "Please run step B first" << endl;
					break;
				}
				//string key_name;
				string parameter_name;
				cout << "input parameter name: choice of [AAR, AAR_STD, CAAR, CAAR_STD, MISS, MEET, BEAT]" << endl;
				cin >> parameter_name;
				vector<Matrix> group = transpose(boot.get_result_matrix());
				if (parameter_name == "AAR"){
					vector<vector<double>> result = boot.get_average_AAR();
					cout << "AAR" << endl;
					cout << "N" << "      " << "Miss" << "        " << "Meet" << "        " << "Beat" << endl;
					cout << result << endl;
				}
				else if (parameter_name == "AAR_STD") {
					vector<vector<double>> result = boot.get_AARstd();
					cout << "AAR_STD" << endl;
					cout << "N" << "      " << "Miss" << "        " << "Meet" << "        " << "Beat" << endl;
					cout << result << endl;
				}
				else if (parameter_name == "CAAR") {
					vector<vector<double>> result = boot.get_average_CAAR();
					cout << "CAAR" << endl;
					cout << "N" << "      " << "Miss" << "        " << "Meet" << "        " << "Beat" << endl;
					cout << result << endl;
				}
				else if (parameter_name == "CAAR_STD") {
					vector<vector<double>> result = boot.get_CAARstd();
					cout << "CAAR_STD" << endl;
					cout << "N" << "      " << "Miss" << "        " << "Meet" << "        " << "Beat" << endl;
					cout << result << endl;
				}
				else if (parameter_name == "MISS") {
					cout << "MISS" << endl;
					cout << "N" << "      " << "AAR" << "        " << "CAAR" << "        " << "AAR_STD" << "        " << "CAAR_STD" << endl;
					cout << group[0] << endl;
				}
				else if (parameter_name == "MEET") {
					cout << "MEET" << endl;
					cout << "N" << "      " << "AAR" << "        " << "CAAR" << "        " << "AAR_STD" << "        " << "CAAR_STD" << endl;
					cout << group[1] << endl;
				}
				else if (parameter_name == "BEAT") {
					cout << "BEAT" << endl;
					cout << "N" << "      " << "AAR" << "        " << "CAAR" << "        " << "AAR_STD" << "        " << "CAAR_STD" << endl;
					cout << group[2] << endl;
				}
				else {
					cout << "Invalid Parameter Name." << endl;
					cout << "Back to main menu." << endl;
					break;
				}

				break;
			}
			case 'D':
			{
				if (!runA) {
					cout << "Please run step A first" << endl;
					break;
				}
				if (!runB) {
					cout << "Please run step B first" << endl;
					break;
				}
				string parameter_name;
				vector<vector<double>> result;
				cout << "input parameter name: choice of [AAR, AAR_STD, CAAR, CAAR_STD]" << endl;
				cin >> parameter_name;
				
				if (parameter_name == "AAR") {
					result = boot.get_average_AAR();
					cout << "AAR" << endl;
				}
				else if (parameter_name == "AAR_STD") {
					result = boot.get_AARstd();
					cout << "AAR_STD" << endl;
				}
				else if (parameter_name == "CAAR") {
					result = boot.get_average_CAAR();
					cout << "CAAR" << endl;
				}
				else if (parameter_name == "CAAR_STD") {
					result = boot.get_CAARstd();
					cout << "CAAR_STD" << endl;
				}
				else {
					cout << "Invalid Parameter Name." << endl;
					cout << "Back to main menu." << endl;
					break;
				}
				vector<double> days;
				for (int i = 0; i < 2 * g.N_days + 1; i++) {
					days.push_back(i - g.N_days);
				}
				plot plot_result;
				plot_result.plotResults(days, result[0], result[1], result[2], result[0].size());
				break;
			}

			case 'E': {
				if (!runA) {
					cout << "Please run step A first" << endl;
					break;
				}
				if (!runB) {
					cout << "Please run step B first" << endl;
					break;
				}
				string stockinput;
				bool pass = FALSE;
				cout << " Input Stock Name(Upper Case): " << endl;
				cin >> stockinput;
				for (map<string, stock>::iterator i = g.global_stock.begin(); i != g.global_stock.end(); i++) {
					if (i->first == stockinput) {
						pass = TRUE;
					}
				}
				if (pass == FALSE){
					cout << "Invalid Stock Name." << endl;
					cout << "Back to main menu." << endl;
					break;
				}

				stock info = g.global_stock[stockinput];
				map<string, double> input_price = info.get_price();
				info.cal_return();
				map<string, double> stockinput_return = info.get_return();

				cout << "announce date: " << info.get_annouce_date() << endl;
				cout << "estimated earning: " << info.get_estimated() << endl;
				cout << "reported earning: " << info.get_reported() << endl;
				cout << "surprise: " << info.get_surprise() << endl;
				cout << "surprise %: " << info.get_surprise_percentage() << endl;

				cout << "======" << endl;
				cout << "Date" << "          " << "price" << "   " << "cumulative_return" << endl;
				double cumulative_return = 0;
				for (map<string, double>::iterator i = stockinput_return.begin(); i != stockinput_return.end(); i++) {
					cumulative_return += i->second;
					cout << i->first << "~~~" << input_price[i->first] << "  " << cumulative_return << endl;
				}
				break;
			}
			case 'F': {
				exit(10);
			}
		}
	}
}