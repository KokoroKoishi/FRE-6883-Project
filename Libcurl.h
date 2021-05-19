#pragma once
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
#include"stock.h"
#include<map>
#include <stdio.h>

using namespace std;
int write_data(void* ptr, int size, int nmemb, FILE* stream)
{
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}
struct MemoryStruct {
	char* memory;
	size_t size;
};
void* myrealloc(void* ptr, size_t size)
{
	/* There might be a realloc() out there that doesn't like reallocting
	NULL pointers, so we take care of it here */
	if (ptr)
		return realloc(ptr, size);
	else
		return malloc(size);
}
int write_data2(void* ptr, size_t size, size_t nmemb, void* data)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*)data;
	mem->memory = (char*)myrealloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}
	return realsize;
}
string getTimeinSeconds(string Time)
{
	std::tm t = { 0 };
	std::istringstream ssTime(Time);
	char time[100];
	memset(time, 0, 100);
	if (ssTime >> std::get_time(&t, "%Y-%m-%d"))
	{
		sprintf_s(time, "%lld", mktime(&t));
		return string(time);
	}
	else
	{
		cout << "GetTimeInSeconds Parse failed\n";
		return "";
	}
}
string getTimeinSeconds(string Time, int nday)
{
	std::tm t = { 0 };
	std::istringstream ssTime(Time);
	char time[100];
	memset(time, 0, 100);
	if (ssTime >> std::get_time(&t, "%Y-%m-%d"))
	{
		sprintf_s(time, "%lld", mktime(&t));

		int ss = stoi(string(time));
		ss = ss + nday * 60 * 60 * 24;

		//struct tm newtime;
		//time_t now = ss;
		//localtime_s(&newtime, &now);
		//cout << asctime(&newtime) << endl;

		return to_string(ss);
	}
	else
	{
		cout << "GetTimeinSeconds with two parameter Parse failed\n";
		return "";
	}
}
string getDateAfterNday(string Time, int nday)
{
	string result;
	double pre_dDate = strtod(getTimeinSeconds(Time).c_str(), NULL) + 86400 * nday;

	struct tm newtime;
	time_t now = pre_dDate;
	localtime_s(&newtime, &now);

	char buffer[80];
	strftime(buffer, 80, "%F", &newtime);
	result = buffer;
	//cout << pre_sDate << endl;
	//cout << sDate << endl;
	return result;
}
void populateSymbolVector(map<string, stock>& symbols, string FileName)
{
	ifstream fin;
	fin.open(FileName, ios::in);
	string line, symbol, announce_date, period, estimated, reported, surprise, surprise_percentage;
	double destimated, dreported, dsurprise, dsurprise_percentage;
	while (!fin.eof())
	{
		getline(fin, line);
		//if (oldline.substr(0, 4) == "0xBF") {
		//	line = oldline.substr(4, oldline.length());
		//}
		//else {
		//	line = oldline;
		//}
		stringstream sin(line);
		cout << line << endl;
		getline(sin, symbol, '\t');
		getline(sin, announce_date, '\t');
		getline(sin, period, '\t');
		getline(sin, estimated, '\t');
		getline(sin, reported, '\t');
		getline(sin, surprise, '\t');
		getline(sin, surprise_percentage);
		destimated = strtod(estimated.c_str(), NULL);
		dreported = strtod(reported.c_str(), NULL);
		dsurprise = strtod(surprise.c_str(), NULL);
		dsurprise_percentage = strtod(surprise_percentage.c_str(), NULL);
		stock curr_stock = stock(symbol, announce_date, destimated, dreported, dsurprise, dsurprise_percentage);
		symbols[symbol] = curr_stock;
	}
}

int download_data(map<string, stock>& symbols, int N)
{
	map<string, stock>::iterator itr = symbols.begin();

	struct MemoryStruct data;
	data.memory = NULL;
	data.size = 0;

	// file pointers to create file that store the data  
	FILE* fp1, * fp2;

	// name of files  
	const char outfilename[FILENAME_MAX] = "Output.txt";
	const char resultfilename[FILENAME_MAX] = "Results.txt";

	// declaration of an object CURL 
	CURL* handle;

	CURLcode result;

	// set up the program environment that libcurl needs 
	curl_global_init(CURL_GLOBAL_ALL);

	// curl_easy_init() returns a CURL easy handle 
	handle = curl_easy_init();

	// if everything's all right with the easy handle... 
	if (handle)
	{
		string sCookies, sCrumb;
		fp1 = fopen(resultfilename, "w");

		if (sCookies.length() == 0 || sCrumb.length() == 0)
		{
			fp2 = fopen(outfilename, "w");
			curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
			curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");

			curl_easy_setopt(handle, CURLOPT_ENCODING, "");
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp2);
			result = curl_easy_perform(handle);
			fclose(fp2);

			if (result != CURLE_OK)
			{
				// if errors have occurred, tell us what is wrong with result
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

			// perform, then store the expected code in result
			result = curl_easy_perform(handle);

			if (result != CURLE_OK)
			{
				// if errors have occured, tell us what is wrong with result
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}

			char cKey[] = "CrumbStore\":{\"crumb\":\"";
			char* ptr1 = strstr(data.memory, cKey);
			char* ptr2 = ptr1 + strlen(cKey);
			char* ptr3 = strstr(ptr2, "\"}");
			if (ptr3 != NULL)
				*ptr3 = NULL;

			sCrumb = ptr2;

			fp2 = fopen("cookies.txt", "r");
			char cCookies[100];
			if (fp2) {
				while (fscanf(fp2, "%s", cCookies) != EOF);
				fclose(fp2);
			}
			else
				cerr << "cookies.txt does not exists" << endl;

			sCookies = cCookies;
			free(data.memory);
			data.memory = NULL;
			data.size = 0;
		}

		while (true)
		{
			data.memory = NULL;
			data.size = 0;


			if (itr == symbols.end())
				break;
			string startTime = getTimeinSeconds(itr->second.get_annouce_date(), -N);
			string endTime = getTimeinSeconds(itr->second.get_annouce_date(), N + 1);
			string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
			string symbol = itr->first;
			string urlB = "?period1=";
			string urlC = "&period2=";
			string urlD = "&interval=1d&events=history&crumb=";
			string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
			const char* cURL = url.c_str();
			const char* cookies = sCookies.c_str();
			curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);
			curl_easy_setopt(handle, CURLOPT_URL, cURL);

			cout << url << endl;

			//fprintf(fp1, "%s\n", symbol.c_str());
			//curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
			//curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp1);
			//result = curl_easy_perform(handle);
			//fprintf(fp1, "%c", '\n');

			//// Check for errors */
			//if (result != CURLE_OK)
			//{
			//	// if errors have occurred, tell us what is wrong with 'result'
			//	fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
			//	return 1;
			//}
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
			result = curl_easy_perform(handle);

			if (result != CURLE_OK)
			{
				// if errors have occurred, tell us what is wrong with result
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}

			stringstream sData;
			sData.str(data.memory);
			string sValue, sDate;
			string pre_sDate;
			double dValue = 0;
			double pre_dValue = 0;
			string line;
			getline(sData, line);
			map<string, double> price;

			getline(sData, line);
			sDate = line.substr(0, line.find_first_of(','));
			line.erase(line.find_last_of(','));
			sValue = line.substr(line.find_last_of(',') + 1);
			dValue = strtod(sValue.c_str(), NULL);
			//cout << sDate << ":::::::::" << std::fixed << ::setprecision(6) << dValue << endl;
			price[sDate] = dValue;
			pre_sDate = sDate;
			pre_dValue = dValue;
			/*cout << line << endl;*/

			while (getDateAfterNday(itr->second.get_annouce_date(), -N) != pre_sDate) {
				double pre_dDate = strtod(getTimeinSeconds(pre_sDate).c_str(), NULL) - 86400;

				struct tm newtime;
				time_t now = pre_dDate;
				localtime_s(&newtime, &now);

				char buffer[80];
				strftime(buffer, 80, "%F", &newtime);
				pre_sDate = buffer;
				price[pre_sDate] = pre_dValue;
			}
			pre_sDate = sDate;
			pre_dValue = dValue;
			while (getline(sData, line)) {
				sDate = line.substr(0, line.find_first_of(','));
				line.erase(line.find_last_of(','));
				sValue = line.substr(line.find_last_of(',') + 1);
				dValue = strtod(sValue.c_str(), NULL);
				//cout << sDate << ":::::::::" << std::fixed << ::setprecision(6) << dValue << endl;

				while (sDate != getDateAfterNday(pre_sDate, 1)) {
					double pre_dDate = strtod(getTimeinSeconds(pre_sDate).c_str(), NULL) + 86400;

					struct tm newtime;
					time_t now = pre_dDate;
					localtime_s(&newtime, &now);

					char buffer[80];
					strftime(buffer, 80, "%F", &newtime);
					pre_sDate = buffer;
					//cout << pre_sDate << endl;
					//cout << sDate << endl;
					price[pre_sDate] = pre_dValue;

				}
				pre_sDate = sDate;
				pre_dValue = dValue;
				price[sDate] = dValue;
			}

			while (getDateAfterNday(itr->second.get_annouce_date(), N) != pre_sDate) {
				double pre_dDate = strtod(getTimeinSeconds(pre_sDate).c_str(), NULL) + 86400;

				struct tm newtime;
				time_t now = pre_dDate;
				localtime_s(&newtime, &now);

				char buffer[80];
				strftime(buffer, 80, "%F", &newtime);
				pre_sDate = buffer;
				price[pre_sDate] = pre_dValue;
			}
			pre_sDate = sDate;
			pre_dValue = dValue;

			itr->second.update_price(price);
			itr->second.set_N(N);
			itr++;

		}
		fclose(fp1);
		free(data.memory);
		data.size = 0;
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return 1;
	}

	// cleanup since you've used curl_easy_init  
	curl_easy_cleanup(handle);

	// release resources acquired by curl_global_init() 
	curl_global_cleanup();

	return 0;
}

int download_data(stock &stock_, string start, string end)
{
	struct MemoryStruct data;
	data.memory = NULL;
	data.size = 0;

	// file pointers to create file that store the data  
	FILE* fp1, * fp2;

	// name of files  
	const char outfilename[FILENAME_MAX] = "Output.txt";
	const char resultfilename[FILENAME_MAX] = "Results.txt";

	// declaration of an object CURL 
	CURL* handle;

	CURLcode result;

	// set up the program environment that libcurl needs 
	curl_global_init(CURL_GLOBAL_ALL);

	// curl_easy_init() returns a CURL easy handle 
	handle = curl_easy_init();

	// if everything's all right with the easy handle... 
	if (handle)
	{
		string sCookies, sCrumb;
		fp1 = fopen(resultfilename, "w");

		if (sCookies.length() == 0 || sCrumb.length() == 0)
		{
			fp2 = fopen(outfilename, "w");
			curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
			curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
			curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
			curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");

			curl_easy_setopt(handle, CURLOPT_ENCODING, "");
			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp2);
			result = curl_easy_perform(handle);
			fclose(fp2);

			if (result != CURLE_OK)
			{
				// if errors have occurred, tell us what is wrong with result
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}

			curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
			curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);

			// perform, then store the expected code in result
			result = curl_easy_perform(handle);

			if (result != CURLE_OK)
			{
				// if errors have occured, tell us what is wrong with result
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
				return 1;
			}

			char cKey[] = "CrumbStore\":{\"crumb\":\"";
			char* ptr1 = strstr(data.memory, cKey);
			char* ptr2 = ptr1 + strlen(cKey);
			char* ptr3 = strstr(ptr2, "\"}");
			if (ptr3 != NULL)
				*ptr3 = NULL;

			sCrumb = ptr2;

			fp2 = fopen("cookies.txt", "r");
			char cCookies[100];
			if (fp2) {
				while (fscanf(fp2, "%s", cCookies) != EOF);
				fclose(fp2);
			}
			else
				cerr << "cookies.txt does not exists" << endl;

			sCookies = cCookies;
			free(data.memory);
			data.memory = NULL;
			data.size = 0;
		}

		data.memory = NULL;
		data.size = 0;

		string startTime = getTimeinSeconds(start);
		string endTime = getTimeinSeconds(end);
		string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
		string symbol = stock_.get_ticker();
		string urlB = "?period1=";
		string urlC = "&period2=";
		string urlD = "&interval=1d&events=history&crumb=";
		string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
		const char* cURL = url.c_str();
		const char* cookies = sCookies.c_str();
		curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);
		curl_easy_setopt(handle, CURLOPT_URL, cURL);

		cout << url << endl;

		//fprintf(fp1, "%s\n", symbol.c_str());
		//curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);
		//curl_easy_setopt(handle, CURLOPT_WRITEDATA, fp1);
		//result = curl_easy_perform(handle);
		//fprintf(fp1, "%c", '\n');

		//// Check for errors */
		//if (result != CURLE_OK)
		//{
		//	// if errors have occurred, tell us what is wrong with 'result'
		//	fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
		//	return 1;
		//}
		curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
		curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void*)&data);
		result = curl_easy_perform(handle);

		if (result != CURLE_OK)
		{
			// if errors have occurred, tell us what is wrong with result
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
			return 1;
		}

		stringstream sData;
		sData.str(data.memory);
		string sValue, sDate;
		string pre_sDate;
		double dValue = 0;
		double pre_dValue = 0;
		string line;
		getline(sData, line);
		map<string, double> price;

		getline(sData, line);
		sDate = line.substr(0, line.find_first_of(','));
		line.erase(line.find_last_of(','));
		sValue = line.substr(line.find_last_of(',') + 1);
		dValue = strtod(sValue.c_str(), NULL);
		//cout << sDate << ":::::::::" << std::fixed << ::setprecision(6) << dValue << endl;
		price[sDate] = dValue;
		pre_sDate = sDate;
		pre_dValue = dValue;
		/*cout << line << endl;*/

		while (start != pre_sDate) {
			double pre_dDate = strtod(getTimeinSeconds(pre_sDate).c_str(), NULL) - 86400;

			struct tm newtime;
			time_t now = pre_dDate;
			localtime_s(&newtime, &now);

			char buffer[80];
			strftime(buffer, 80, "%F", &newtime);
			pre_sDate = buffer;
			price[pre_sDate] = pre_dValue;
		}
		pre_sDate = sDate;
		pre_dValue = dValue;

		while (getline(sData, line)) {
			sDate = line.substr(0, line.find_first_of(','));
			line.erase(line.find_last_of(','));
			sValue = line.substr(line.find_last_of(',') + 1);
			dValue = strtod(sValue.c_str(), NULL);
			/*cout << sDate << ":::::::::" << std::fixed << ::setprecision(6) << dValue << endl;*/

			while (strtod(getTimeinSeconds(sDate).c_str(), NULL) != strtod(getTimeinSeconds(pre_sDate).c_str(), NULL) + 86400) {
				double pre_dDate = strtod(getTimeinSeconds(pre_sDate).c_str(), NULL) + 86400;

				struct tm newtime;
				time_t now = pre_dDate;
				localtime_s(&newtime, &now);

				char buffer[80];
				strftime(buffer, 80, "%F", &newtime);
				pre_sDate = buffer;
				//cout << pre_sDate << endl;
				//cout << sDate << endl;
				price[pre_sDate] = pre_dValue;

			}
			pre_sDate = sDate;
			pre_dValue = dValue;
			price[sDate] = dValue;
		}

		while (end != pre_sDate) {
			double pre_dDate = strtod(getTimeinSeconds(pre_sDate).c_str(), NULL) + 86400;

			struct tm newtime;
			time_t now = pre_dDate;
			localtime_s(&newtime, &now);

			char buffer[80];
			strftime(buffer, 80, "%F", &newtime);
			pre_sDate = buffer;
			price[pre_sDate] = pre_dValue;
		}
		pre_sDate = sDate;
		pre_dValue = dValue;

		stock_.update_price(price);

		fclose(fp1);
		free(data.memory);
		data.size = 0;
	}
	else
	{
		fprintf(stderr, "Curl init failed!\n");
		return 1;
	}

	// cleanup since you've used curl_easy_init  
	curl_easy_cleanup(handle);

	// release resources acquired by curl_global_init() 
	curl_global_cleanup();

	return 0;
}

void ParseIntoMap(map<string, stock>& symbols, string FileName, int Nday) {
	populateSymbolVector(symbols, FileName);
	download_data(symbols, Nday);
}


