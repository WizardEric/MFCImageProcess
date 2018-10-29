#include "stdafx.h"
#include "ImageProcess.h"
#include <vector>
#include <algorithm>
#include<math.h>
#include<complex>

static double noise;
static bool GetValue(int p[], int size, int &value)
{
	//数组中间的值
	int zxy = p[(size - 1) / 2];
	//用于记录原数组的下标
	int *a = new int[size];
	int index = 0;
	for (int i = 0; i<size; ++i)
		a[index++] = i;

	for (int i = 0; i<size - 1; i++)
		for (int j = i + 1; j<size; j++)
			if (p[i]>p[j]) {
				int tempA = a[i];
				a[i] = a[j];
				a[j] = tempA;
				int temp = p[i];
				p[i] = p[j];
				p[j] = temp;

			}
	int zmax = p[size - 1];
	int zmin = p[0];
	int zmed = p[(size - 1) / 2];

	if (zmax>zmed&&zmin<zmed) {
		if (zxy>zmin&&zxy<zmax)
			value = (size - 1) / 2;
		else
			value = a[(size - 1) / 2];
		delete[]a;
		return true;
	}
	else {
		delete[]a;
		return false;
	}

}


UINT ImageProcess::medianFilter(LPVOID  p)
{
	ThreadParam* param = (ThreadParam*)p;

	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	int startIndex = param->startIndex;
	int endIndex = param->endIndex;
	int maxSpan = param->maxSpan;
	int maxLength = (maxSpan * 2 + 1) * (maxSpan * 2 + 1);

	byte* pRealData = (byte*)param->src->GetBits();
	int pit = param->src->GetPitch();
	int bitCount = param->src->GetBPP() / 8;

	int *pixel = new int[maxLength];//存储每个像素点的灰度
	int *pixelR = new int[maxLength];
	int *pixelB = new int[maxLength];
	int *pixelG = new int[maxLength];
	int index = 0;
	for (int i = startIndex; i <= endIndex; ++i)
	{
		int Sxy = 1;
		int med = 0;
		int state = 0;
		int x = i % maxWidth;
		int y = i / maxWidth;
		while (Sxy <= maxSpan)
		{
			index = 0;
			for (int tmpY = y - Sxy; tmpY <= y + Sxy && tmpY <maxHeight; tmpY++)
			{
				if (tmpY < 0) continue;
				for (int tmpX = x - Sxy; tmpX <= x + Sxy && tmpX<maxWidth; tmpX++)
				{
					if (tmpX < 0) continue;
					if (bitCount == 1)
					{
						pixel[index] = *(pRealData + pit*(tmpY)+(tmpX)*bitCount);
						pixelR[index++] = pixel[index];

					}
					else
					{
						pixelR[index] = *(pRealData + pit*(tmpY)+(tmpX)*bitCount + 2);
						pixelG[index] = *(pRealData + pit*(tmpY)+(tmpX)*bitCount + 1);
						pixelB[index] = *(pRealData + pit*(tmpY)+(tmpX)*bitCount);
						pixel[index++] = int(pixelR[index] * 0.299 + 0.587*pixelG[index] + pixelB[index] * 0.144);

					}
				}

			}
			if (index <= 0)
				break;
			if ((state = GetValue(pixel, index, med)) == 1)
				break;

			Sxy++;
		};

		if (state)
		{
			if (bitCount == 1)
			{
				*(pRealData + pit*y + x*bitCount) = pixel[med];

			}
			else
			{
				*(pRealData + pit*y + x*bitCount + 2) = pixelR[med];
				*(pRealData + pit*y + x*bitCount + 1) = pixelG[med];
				*(pRealData + pit*y + x*bitCount) = pixelB[med];

			}
		}

	}



	delete[]pixel;
	delete[]pixelR;
	delete[]pixelG;
	delete[]pixelB;

	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MESSAGE, 1, NULL);
	return 0;
}

UINT ImageProcess::addNoise(LPVOID  p)
{
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();

	int startIndex = param->startIndex;
	int endIndex = param->endIndex;
	byte* pRealData = (byte*)param->src->GetBits();
	//byte* dstRealData = (byte*)param->dst->GetBits();
	//int dstpit = param->dst->GetPitch();
	int bitCount = param->src->GetBPP() / 8;
	int pit = param->src->GetPitch();

	for (int i = startIndex; i <= endIndex; ++i)
	{
		int x = i % maxWidth;
		int y = i / maxWidth;
		if ((rand() % 1000) * 0.001 < NOISE)
		{
			int value = 0;
			if (rand() % 1000 < 500)
			{
				value = 0;
			}
			else
			{
				value = 255;
			}
			if (bitCount == 1)
			{
				*(pRealData + pit * y + x * bitCount) = value;
			}
			else
			{
				*(pRealData + pit * y + x * bitCount) = value;
				*(pRealData + pit * y + x * bitCount + 1) = value;
				*(pRealData + pit * y + x * bitCount + 2) = value;
			}
		}
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MESSAGE, 1, NULL);
	return 0;
}

UINT ImageProcess::zoom_WIN(LPVOID p)
{
	//float WX[4], WY[4];
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	int dstWidth = param->dst->GetWidth();
	int dstHeight = param->dst->GetHeight();
	byte* pRealData = (byte*)param->src->GetBits();
	byte* dstRealData = (byte*)param->dst->GetBits();
	int dstpit = param->dst->GetPitch();
	int bitCount = param->dst->GetBPP() / 8;
	int pit = param->src->GetPitch();
	 

	for (int idx = param->startIndex; idx <= param->endIndex; ++idx)
	{
		int x = idx % dstWidth;
		int y = idx / dstWidth;
		double xx = x / param->nzoom;
		double yy = y / param->nzoom;
		double value = 0;
		int v, w;
		//双三次插值
		for (int i = 0; i < bitCount; i++) {
			v = floor(xx);
			w = floor(yy);
			if (xx <= 0 || xx >= maxWidth || yy <= 0 || yy >= maxHeight)
				value = 0;
			//取W[]的W[1+x],W[x],W[1-x],W[2-x],x为小数部分
			else {
				if (v >= 2 && v < maxWidth - 2 && w >= 2 && w < maxHeight - 2) {
					value = 0;
					for (int Wx = -1; Wx < 3; Wx++) {
						for (int Wy = -1; Wy < 3; Wy++) {
							value += (*(pRealData + pit * (w + Wx) + (v + Wy)*bitCount + i))*getW(xx - v - Wy)*getW(yy - w - Wx);
						}
					}
				}
			}
			if (value > 255)
				value = 255;
			if (value < 0)
				value = 0;
			*(dstRealData + dstpit * y + x * bitCount +i ) = value;
		}
		
		
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MESSAGE, 1, NULL);
	return 0;
}

UINT ImageProcess::rorate_WIN(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;
	double angle = param->angle * PI / 180.0;
	int srcWidth = param->src->GetWidth();
	int srcHeight = param->src->GetHeight();
	int maxWidth = param->dst->GetWidth();
	int maxHeight = param->dst->GetHeight();
	byte* pRealData = (byte*)param->src->GetBits();
	byte* dstRealData = (byte*)param->dst->GetBits();
	int dstpit = param->dst->GetPitch();
	int bitCount = param->dst->GetBPP() / 8;
	int pit = param->src->GetPitch();

	int moveY = maxHeight / 2 - srcHeight / 2;
	int moveX = maxWidth / 2 - srcWidth / 2;

	for (int idx = param->startIndex; idx <= param->endIndex; ++idx) {
		int x = idx % maxWidth;
		int y = idx / maxWidth;
		//要变换坐标系xx= (x-W/2),yy=(y-H/2),然后旋转后新坐标x=rcos(a+b),然后三角函数分解，y同理
		double xx = (x - maxWidth / 2)*cos(angle) - (y - maxHeight / 2)*sin(angle) + maxWidth/2 - moveX;
		double yy = (x - maxWidth / 2)*sin(angle) + (y - maxHeight / 2)*cos(angle) + maxHeight/2 - moveY;
		double value = 0;
		int v, w;
		//双三次插值
		for (int i = 0; i < bitCount; i++) {
			v = floor(xx);
			w = floor(yy);
			if (xx <= 0 || xx >= srcWidth || yy <= 0 || yy >= srcHeight)
				value = 0;
			//取W[]的W[1+x],W[x],W[1-x],W[2-x],x为小数部分
			else {
				if (v >= 2 && v < srcWidth - 2 && w >= 2 && w < srcHeight - 2) {
					value = 0;
					for (int Wx = -1; Wx < 3; Wx++) {
						for (int Wy = -1; Wy < 3; Wy++) {
							value += (*(pRealData + pit * (w + Wx) + (v + Wy)*bitCount + i))*getW(xx - v - Wy)*getW(yy - w - Wx);
						}
					}
				}
			}
			if (value > 255)
				value = 255;
			if (value < 0)
				value = 0;
			*(dstRealData + dstpit * y + x * bitCount + i) = value;
		}

	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MESSAGE, 1, NULL);
	return 0;
}

UINT ImageProcess::addGaussian(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->dst->GetWidth();
	int maxHeight = param->dst->GetHeight();
	byte* pRealData = (byte*)param->src->GetBits();
	byte* dstRealData = (byte*)param->dst->GetBits();
	int dstpit = param->dst->GetPitch();
	int bitCount = param->src->GetBPP() / 8;
	int pit = param->src->GetPitch();
	//int Gaussian = param->Gaussin;
	int means = param->means;
	int variances = param->variance;
	for (int idx = param->startIndex; idx <= param->endIndex; ++idx) {
		int x = idx % maxWidth;
		int y = idx / maxWidth;

		for (int i = 0; i < bitCount; i++) {
			double value = 0;
			value = *(pRealData + pit * y + x * bitCount +i) + (means + generateGaussian(means, variances) * sqrt(variances));
			//value = *(pRealData + pit * y + x * bitCount+t) + (param->means+generateGaussian(param->means, param->variance)*sqrt(param->variance));
			if (value > 255)
				value = 255;
			if (value < 0)
				value = 0;
			*(dstRealData + dstpit * y + x * bitCount + i) = value;
			//*(pRealData + pit * y + x * bitCount+i) = value;

		}
		
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MESSAGE, 1, NULL);
	return 0;
}

UINT ImageProcess::meanFilter_WIN(LPVOID p)
{
	
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	byte* pRealData = (byte*)param->src->GetBits();
	byte* dstRealData = (byte*)param->dst->GetBits();
	int dstpit = param->dst->GetPitch();
	int bitCount = param->dst->GetBPP() / 8;
	int pit = param->src->GetPitch();
	//int Gaussian = param->Gaussin;
	for (int idx = param->startIndex; idx <= param->endIndex; ++idx) {
		int x = idx % maxWidth;
		int y = idx / maxWidth;
		for (int i = 0; i < bitCount; i++) {
			double value = 0;
			if (x >= 1 && x < maxWidth-1&&y>=1 && y < maxHeight-1) {
				
				for (int s = -1; s < 2; s++) {
					for (int v = -1; v < 2; v++) {
						value += (*(pRealData + pit * (y + s) + (x + v)*bitCount + i));
					}
				}
				value = value / 9.0;
				if (value > 255)
					value = 255;
				if (value < 0)
					value = 0;
				*(dstRealData + dstpit * y + x * bitCount + i) = value;
			}
			
		}
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MESSAGE, 1, NULL);
	return 0;
}

UINT ImageProcess::GaussianFilter_WIN(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	byte* pRealData = (byte*)param->src->GetBits();
	byte* dstRealData = (byte*)param->dst->GetBits();
	int dstpit = param->dst->GetPitch();
	int bitCount = param->dst->GetBPP() / 8;
	int pit = param->src->GetPitch();
	int G = param->Gaussin;
	//double sum = generateGaussianKernel(G, (double)sqrt(param->variance));
	//int Gaussian = param->Gaussin;
	for (int idx = param->startIndex; idx < param->endIndex; ++idx) {
		int x = idx % maxWidth;
		int y = idx / maxWidth;
		for (int i = 0; i < bitCount; i++) {
			double value = 0;
			
			if (x >= 2 && x <= maxWidth - 2 && y >= 2 && y <= maxHeight - 2) {

				for (int s = -1; s <G-1 ; s++) {
					for (int v = -1; v < G-1; v++) {
						double k = (double)(1 / (2 * PI*param->variance))*exp(-((s - (G-2) / 2)*(s - (G - 2) / 2) + (v - (G - 2) / 2)*(v - (G - 2) / 2)) / (2 * param->variance));
						value += *(pRealData + pit * (y + s) + (x + v)*bitCount + i)*(k);
					}
				}
				//value = value / 9.0;
				if (value > 255)
					value = 255;
				if (value < 0)
					value = 0;
				*(dstRealData + dstpit * y + x * bitCount + i) = value;
			}

		}
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MESSAGE, 1, NULL);
	return 0;
}

UINT ImageProcess::wienerFilter(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	byte* pRealData = (byte*)param->src->GetBits();
	byte* dstRealData = (byte*)param->dst->GetBits();
	int dstpit = param->dst->GetPitch();
	int bitCount = param->dst->GetBPP() / 8;
	int pit = param->src->GetPitch();
	double meanR = getMeans(param,0);
	double meanG = getMeans(param, 1);
	double meanB = getMeans(param, 2);
	
	double variancesR = getVariances(param,meanR,0);
	double variancesG = getVariances(param, meanG,1);
	double variancesB = getVariances(param, meanB, 2);
	double valueR = 0;
	double valueB = 0;
	double valueG = 0;
	for (int idx = param->startIndex; idx <= param->endIndex; ++idx) {
		int x = idx % maxWidth;
		int y = idx / maxHeight;
		double value = 0;
		valueR = meanR+max(0,variancesR-10)*(*(pRealData + pit * y + x * bitCount ) - meanR) / max(variancesR, 10);
		valueG = meanG + max(0, variancesG - 10)*(*(pRealData + pit * y + x * bitCount+1) - meanG) / max(variancesG, 10);
		valueB = meanB + max(0, variancesB - 10)*(*(pRealData + pit * y + x * bitCount+2) - meanB) / max(variancesB, 10);
		*(dstRealData + dstpit * y + x * bitCount) = valueR;
		*(dstRealData + dstpit * y + x * bitCount+1) = valueG;
		*(dstRealData + dstpit * y + x * bitCount+2) = valueB;
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MESSAGE, 1, NULL);
	return 0;
}

UINT ImageProcess::Dft(LPVOID p)
{
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	byte* pRealData = (byte*)param->src->GetBits();
	byte* dstRealData = (byte*)param->dst->GetBits();
	int dstpit = param->dst->GetPitch();
	int bitCount = param->dst->GetBPP() / 8;
	int pit = param->src->GetPitch();
	int numData = maxWidth * maxHeight;
	
	for (int idx = param->startIndex; idx <= param->endIndex; ++idx) {
		int x = idx % maxWidth;
		int y = idx / maxWidth;
		
			std::complex<double> value;
			for(int s=0;s<maxHeight;s++)
				for (int v = 0; v < maxWidth; v++) {
					auto ex = 2 * PI*(((double)x*v / maxWidth) + ((double)y*s) / maxHeight);
					auto w = std::complex<double>(-cos(ex),-sin(ex));
					double gray = (*(pRealData + pit * v + s * bitCount))*0.299 + *(pRealData + pit * v + s * bitCount + 1)*0.587 +
						*(pRealData + pit * v + s * bitCount + 2)*0.114;
					gray *= pow(-1, s + v);

					std::complex<double> dp(gray, gray);
					value += dp*w;
				}
			auto v0 = std::abs(value);	
			byte v1 = 14 * log(v0 + 1);
			if (v1 > 255)v1 = 255;
			else if (v1 < 0)v1 = 0;
			*(dstRealData + dstpit * y + x * bitCount ) = v1;
			*(dstRealData + dstpit * y + x * bitCount + 1) = v1;
			*(dstRealData + dstpit * y + x * bitCount + 2) = v1;
			
	}
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), WM_MESSAGE, 1, NULL);
	return 0;
}

double ImageProcess::getMeans(LPVOID p,int i)
{
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	byte* pRealData = (byte*)param->src->GetBits();
	//byte* dstRealData = (byte*)param->dst->GetBits();
	int bitCount = param->src->GetBPP()/8;
	int pit = param->src->GetPitch();
	int num = maxWidth * maxHeight;

	double means = 0;
	for (int idx = param->startIndex; idx <= param->endIndex; ++idx) {
		int x = idx % maxWidth;
		int y = idx / maxWidth;
		
			double value = 0;
			if (x >= 1 && x < maxWidth - 1 && y >= 1 && y < maxHeight - 1) {

				for (int s = -1; s < 2; s++) {
					for (int v = -1; v < 2; v++) {
						value += (*(pRealData + pit * (y + s) + (x + v)*bitCount+i ));
					}
				}
				value = value / 9.0;
				means+=value;
				//*(dstRealData + dstpit * y + x * bitCount + i) = value;
			}	
	}
	//平均
	return means / num;
}

double ImageProcess::getVariances(LPVOID p, double means, int i)
{
	ThreadParam* param = (ThreadParam*)p;
	int maxWidth = param->src->GetWidth();
	int maxHeight = param->src->GetHeight();
	byte* pRealData = (byte*)param->src->GetBits();
	//byte* dstRealData = (byte*)param->dst->GetBits();
	int bitCount = param->src->GetBPP() / 8;
	int pit = param->src->GetPitch();
	int num = maxWidth * maxHeight;
	double variances = 0;
	for (int idx = param->startIndex; idx <= param->endIndex; ++idx) {
		int x = idx % maxWidth;
		int y = idx / maxWidth;

		double value = 0;
		if (x >= 1 && x < maxWidth - 1 && y >= 1 && y < maxHeight - 1) {

			for (int s = -1; s < 2; s++) {
				for (int v = -1; v < 2; v++) {
					value += pow(*(pRealData + pit * (y + s) + (x + v)*bitCount + i)-means, 2);
				}
			}
			value = value / 9.0;
			variances += value;
			//*(dstRealData + dstpit * y + x * bitCount + i) = value;
		}
	}
	
	return variances / num;
}

float ImageProcess::getW( float x)
{
	float a = -0.5;
	float temp = abs(x);
	if (temp <= 1.0) {
		return (a + 2)*pow(temp, 3) - (a + 3)*pow(temp, 2) + 1;
	}
	else if (temp<2.0) {
		return a * pow(temp, 3) - 5 * a*pow(temp, 2) + 8 * a*temp - 4 * a;
	}
	else
		return 0.0;
	/*float tempx[4];
	tempx[0] = (x - temp) + 1;
	tempx[1] = x - temp;
	tempx[2] = 1 - (x - temp);
	tempx[3] = 2 - (x - temp);

	W[0] = a * pow(abs(tempx[0]),3) - 5 * a*pow(abs(tempx[0]), 2) + 8 * a*abs(tempx[0]) - 4 * a;
	W[1] = (a + 2)*pow(abs(tempx[1]), 3) - (a + 3)*pow(abs(tempx[1]), 2) + 1;
	W[2] = (a + 2)*pow(abs(tempx[2]), 3) - (a + 3)*pow(abs(tempx[2]), 2) + 1;
	W[3] = a * pow(abs(tempx[3]), 3) - 5 * a*pow(abs(tempx[3]), 2) + 8 * a*abs(tempx[3]) - 4 * a;*/
}

double ImageProcess::generateGaussian(double means, double variance)
{
	double v1, v2;
	v1 = rand() / ((double)RAND_MAX);
	v2 = rand() / ((double)RAND_MAX);
	if (v1 < 1e-100)
		v1 = 1e-100;
	v1 = (-2)*log(v1);
	v2 = v2 * 2 * PI;
	return means + (sqrt(v1)*cos(v2))*sqrt(variance);
}

double ImageProcess::generateGaussianKernel(const int size, const double sigma)
{
	double value = 0;
	for(int i =0;i<size;i++)
		for (int j = 0; j < size; j++) {
			value += (1 / (2 * PI*sigma))*exp(-((i - size/2)*(i - size / 2) + (j - size / 2)*(j - size / 2)) / (2 * sigma));
		}
	return value;
}
