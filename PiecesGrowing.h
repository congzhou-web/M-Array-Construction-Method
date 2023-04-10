#pragma once

#include "PublicFun.h"
#include "tinyxml\tinystr.h"
#include "tinyxml\tinyxml.h"
#include <typeinfo.h>
#include <opencv2\opencv.hpp>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
#include <string>
#include <map>
#include <limits>

using namespace std;
using namespace cv;

class PiecesGrowing
{
public:
	PiecesGrowing(int element, int winrow, int wincol);
	bool algorithmRun();
	vector<vector<int>> _MArray;

	int _win_symbols;
	int _win_row;
	int _win_col;
	int _array_row;
	int _array_col;

	bool _errFlag;
	string _errInfo;

	string _url;
	string _folderName;

	void MArrayFileSave();

private:
	int _step;
	int _win_sum;
	vector<int> _m_sequence;
	vector<int> _sub_m_sequence;
	vector<vector<int>> _dp_height;
	vector<vector<int>> _dp_codeword;
	vector<bool> _codewordMap;
	map<int, int> _tempMap;
	vector<vector<int>> _backCnt;
	vector<vector<int>> _deep_backCnt;
	Mat _debugImg;
	int _thresholdArea;
	int _thresholdEdge;

	vector<vector<int>> _conv_kernel;
	void convKernelInit();
	int convWindow(int rowInd, int colInd);
	int calArea(vector<int>& heights);
	bool areaCheck(int& rowInd, int& colInd);
	bool edgeCheck(int& rowInd, int& colInd);
	bool winPropertyCheck(int& rowInd, int& colInd);

	vector<int> xmlParse(int step);
	bool m_sequenceGenerating();
	inline int getOneValue();
	void dataInit();
	bool rollbackOnRow(int& rowInd, int& colInd, int cnt);
};