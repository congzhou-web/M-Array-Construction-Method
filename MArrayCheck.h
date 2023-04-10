#pragma once
#include <vector>
#include <iostream>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc.hpp>
#include <algorithm>
#include <limits>
#include <stack>
#include "PublicFun.h"

using namespace std;
using namespace cv;

class MArrayCheck
{
	enum checkItem
	{
		symbolsPercentage = 0,
		neighborsRectangle,
		neighborsConnected,
		edgesPercentage,
		neighborsHanming

	};
public:
	MArrayCheck(const vector<vector<int>>& arrayCode , int arrayRow , int arrayCol , int winRow , int winCol , int symbols );

	void checkRun();
	void printInfo();

	vector<vector<double>> _symbolsPercentage;				//�ֲ����� Ԫ��ռ��  �����ֵ-��Сֵ��/ ���ڴ�С
	vector<vector<double>>  _neighborsRectangle_double;		//�ֲ������ξ������  / �������
	vector<vector<double>>  _neighborsConnected_double;				//������ͨԪ��				/ �������
	vector<vector<double>>  _edgesPercentage;				//�ֲ����ڱ�������			/ ����������
	vector<vector<double>>  _neighborsHanming_double;					//���ܱ��ĸ����ڵ�ƽ����������    /  ���ڴ�С

	void symbolsPercentageCal();
	void neighborsRectangleCal();
	void neighborsConnectedCal();
	void edgesPercentageCal();
	void neighborsHanmingCal();


	vector<vector<int>>  _neighborsRectangle;
	vector<vector<int>>  _neighborsConnected;
	vector<vector<int>>  _neighborsHanming;

	double _symbolsPercentage_max;			//�ֲ����� Ԫ��ռ��
	double  _neighborsRectangle_max;		//�ֲ���ͨ�������
	double  _neighborsConnected_max;				//������ͬԪ��
	double  _edgesPercentage_max;		//�ֲ���ͨ�������
	double  _neighborsHanming_max;				//������ͬԪ��

	template<typename T>
	Mat imageColorMap(const vector<vector<T>> &inputArr, checkItem item);

	void paraFileSave();

	void winPropertyCal();
	void hanmingCal();
private:

	bool _errFlag;
	string _errInfo;

	string _url;
	string _folderName;

	vector<vector<int>> _arrayCode;
	int _arrayRow;
	int _arrayCol;
	int _winRow;
	int _winCol;
	int _symbols;

	int _winSum;				// ��������
	double _RC_ratio;           // ���� / ����
	double _CR_ratio;			// ���� / ����	
	bool _winProperty;			// �Ƿ����㴰������ 0������ 1����
	map<int, int> _winMap;      // ���� map

	double _symGlbBalance;			// Ԫ�ؾ����� 0/1 ����
	
	//�������룬 �ٽ�����������
	double _hanmingAverage;
	int _hanmingMinum;
	
	int neighborsExpandOut(int row , int col , vector<vector<bool>>& visited);
	void rectangleArea(const vector<int>& heights, int begin, int end, vector<int>& area);

	//�����
	vector<vector<int>> _convKernel;
	void convKernelInit();
	int convolution(int rowInd, int colInd);
	int hanmingCount(int left, int right);

	//image
	Mat _imageDemo;

	Mat _symbolsPerImg;
	Mat _neighborsConnectedImg ;
	Mat _neighborsRectangleImg ;
	Mat _edgesPercentageImg;
	Mat _neighborsHanmingImg;

	Mat _symbolsPerImg_hot;
	Mat _neighborsConnectedImg_hot;
	Mat _neighborsRectangleImg_hot;
	Mat _edgesPercentageImg_hot;
	Mat _neighborsHanmingImg_hot;

};