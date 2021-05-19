#pragma once


#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

typedef vector<double> Vector;
typedef vector<Vector> Matrix;

Vector operator+(const Vector& V, const Vector& W);
Vector operator-(const Vector& V, const Vector& W);
double operator*(const Vector& V, const Vector& W);

Vector operator*(const Vector& W, const double& a);
Vector operator/(const Vector& W, const double& a);
Vector operator/(const Vector& W, const int& a);
Vector operator*( const Vector& W, const int& a);
Vector operator^(const Vector& V, const double& a);


ostream& operator<<(ostream& out, Vector& V);
ostream& operator<<(ostream& out, Matrix& W);
ostream& operator<<(ostream& out, vector<Matrix> & W);


