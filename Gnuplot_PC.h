#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<vector>
#include<iostream>

using namespace std;

class plot {
public:
    void plotResults(vector<double> xData, vector<double> yData, vector<double> yData2, vector<double> yData3, int dataSize) {
        FILE* gnuplotPipe, * tempDataFile;
        const char* tempDataFileName1 = "Miss";
        const char* tempDataFileName2 = "Meet";
        const char* tempDataFileName3 = "Beat";
        double x, y, x2, y2, x3, y3;
        int i;

        tempDataFile = fopen(tempDataFileName1, "w");
        for (i = 0; i < dataSize; i++) {
            x = xData[i];
            y = yData[i];

            fprintf(tempDataFile, "%lf %lf\n", x, y);
        }
        fclose(tempDataFile);
        tempDataFile = fopen(tempDataFileName2, "w");
        for (i = 0; i < dataSize; i++) {
            x2 = xData[i];
            y2 = yData2[i];
            fprintf(tempDataFile, "%lf %lf\n", x2, y2);
        }
        fclose(tempDataFile);
        tempDataFile = fopen(tempDataFileName3, "w");
        for (i = 0; i < dataSize; i++) {
            x3 = xData[i];
            y3 = yData3[i];
            fprintf(tempDataFile, "%lf %lf\n", x3, y3);
        }
        fclose(tempDataFile);

        // gnuplotPipe = popen("/opt/local/bin/gnuplot","w");
        gnuplotPipe = _popen("C:\\gnuplot\\bin\\gnuplot.exe", "w");
        if (gnuplotPipe) {
            /*fprintf(gnuplotPipe, "set yrange [%f:%f]\n", -0.08, 0.08);*/
            fprintf(gnuplotPipe, "plot \"%s\" with lines, \"%s\" with lines,\"%s\" with lines\n", tempDataFileName1, tempDataFileName2, tempDataFileName3);
            fflush(gnuplotPipe);

            string N_;
            cout << "Press enter to continue..." << endl;
            cin >> N_;
            remove(tempDataFileName1);
            remove(tempDataFileName2);
            remove(tempDataFileName3);
            fprintf(gnuplotPipe, "exit\n");
        }
        else {
            printf("gnuplot not found...");
        }
    }

};









