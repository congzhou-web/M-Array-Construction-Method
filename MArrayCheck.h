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

	vector<vector<double>> _symbolsPercentage;				//局部窗口 元素占比  （最大值-最小值）/ 窗口大小
	vector<vector<double>>  _neighborsRectangle_double;		//局部最大矩形矩形面积  / 窗口面积
	vector<vector<double>>  _neighborsConnected_double;				//邻域连通元素				/ 窗口面积
	vector<vector<double>>  _edgesPercentage;				//局部窗口边沿数量			/ 最大可能数量
	vector<vector<double>>  _neighborsHanming_double;					//与周边四个窗口的平均汉明距离    /  窗口大小

	void symbolsPercentageCal();
	void neighborsRectangleCal();
	void neighborsConnectedCal();
	void edgesPercentageCal();
	void neighborsHanmingCal();


	vector<vector<int>>  _neighborsRectangle;
	vector<vector<int>>  _neighborsConnected;
	vector<vector<int>>  _neighborsHanming;

	double _symbolsPercentage_max;			//局部窗口 元素占比
	double  _neighborsRectangle_max;		//局部连通矩形面积
	double  _neighborsConnected_max;				//邻域相同元素
	double  _edgesPercentage_max;		//局部连通矩形面积
	double  _neighborsHanming_max;				//邻域相同元素

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

	int _winSum;				// 窗口总数
	double _RC_ratio;           // 行数 / 列数
	double _CR_ratio;			// 列数 / 行数	
	bool _winProperty;			// 是否满足窗口属性 0不满足 1满足
	map<int, int> _winMap;      // 窗口 map

	double _symGlbBalance;			// 元素均衡性 0/1 比例
	
	//汉明距离， 临近窗口相似性
	double _hanmingAverage;
	int _hanmingMinum;
	
	int neighborsExpandOut(int row , int col , vector<vector<bool>>& visited);
	void rectangleArea(const vector<int>& heights, int begin, int end, vector<int>& area);

	//卷积核
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