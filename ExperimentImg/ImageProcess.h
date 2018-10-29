#pragma once
#pragma once
#define NOISE 0.2
#define PI 3.141592653
struct ThreadParam
{
	CImage * src;
	CImage * dst;
	int startIndex;
	int endIndex;
	int maxSpan;//为模板中心到边缘的距离
	double nzoom;
	double angle;
	int Gaussin;
	double variance;
	double means;
	void(*callback)(ThreadParam*);

};

static bool GetValue(int p[], int size, int &value);

namespace ImageProcess
{
	UINT medianFilter(LPVOID  param);
	UINT addNoise(LPVOID param);
	UINT zoom_WIN(LPVOID param);
	UINT rorate_WIN(LPVOID param);
	UINT addGaussian(LPVOID param);
	UINT meanFilter_WIN(LPVOID param);
	UINT GaussianFilter_WIN(LPVOID param);
	UINT wienerFilter(LPVOID param);
	UINT Dft(LPVOID param);
	double getMeans(LPVOID param,int);
	double getVariances(LPVOID param,double means,int i);
	float getW(float x);
	double generateGaussian(double means,double variance);
	double generateGaussianKernel(const int size, const double sigma);
};