#pragma once

#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "PublicFun.h"

using namespace std;
using namespace cv;

class NoneZeroMaps
{
public:
	 NoneZeroMaps(int symbols ,int winrow , int wincol);
	 void algorithmRun();
	 vector<vector<int>> _MArray;

	 bool _errFlag;
	 string _errInfo;

	 string _url;
	 string _folderName;

	 int _array_row;
	 int _array_col;
	 int _win_row;
	 int _win_col;
	 int _win_symbols;

	 void MArrayFileSave();

private:
	int _step;
	int _win_sum;
	vector<int> _m_sequence;
	map<int, bool> hash_zeroIndex;
	Mat _debugImage;

	vector<int> xmlParse();
	int getShiftSize();
	void dataInit();
	bool m_sequenceGenerating();
};
