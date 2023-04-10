#pragma once

#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include "tinyxml/tinystr.h"
#include "tinyxml/tinyxml.h"
#include <windows.h>
#include <iostream>
#include "Excel\BasicExcel.hpp"
#include <stdlib.h>

#define VNAME(value) #value ;

using namespace std;
using namespace cv;
using namespace YExcel;

vector<int> xmlParse(int element, int step);
bool SaveImgLocal(Mat image, string fileName, string folder);

//image 演示   CV_8UC1 单通道 0-255 图
template<typename T>
Mat imageShow(const vector<vector<T>> &inputArr)
{
	int row = inputArr.size();
	int col = inputArr.front().size();
	Mat res(row, col, CV_8UC1, Scalar(0));

	double maxV = 0;
	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; ++j)
			maxV = max(maxV, (double)inputArr[i][j]);

	double shift = 255 / maxV;
	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; ++j)
			res.at<uchar>(i, j) = inputArr[i][j] * shift;
	return res;
}

template<typename T>
Mat imageShow(const vector<vector<T>> &inputArr, double shift)
{
	int row = inputArr.size();
	int col = inputArr.front().size();
	Mat res(row, col, CV_8UC1, Scalar(0));
	for (int i = 0; i < row; i++)
		for (int j = 0; j < col; ++j)
			res.at<uchar>(i, j) = (double)inputArr[i][j] * shift;
	return res;
}

template<typename T>																						
bool SaveTxtLocal(const vector<vector<T>> &inputArr, string fileName, string folder)  //save as .txt
{
	ofstream file;
	file.open(folder + "\\" + fileName, ios_base::out);
	if (!file.is_open()) {
		cout << ends << "error happended when save file !!!" << endl;
		return false;
	}

	int row = inputArr.size();
	int col = inputArr.front().size();
	for (int i = 0; i < row; ++i) {
		file << endl;
		for (int j = 0; j < col; ++j)
			//file << ends << ends << ends << inputArr[i][j] ;
			file <<ends << inputArr[i][j] << ",";
	}
	file.close();
	return true;
}

template<typename T>	 
bool SaveExcelLocal(const vector<vector<T>>& inputArr, string fileName, string folder)      //save as excel
{
	int row = inputArr.size();
	int col = inputArr.front().size();

	BasicExcel demo;
	demo.New(1);
	//demo.RenameWorksheet("Sheet1", "Test1");
	BasicExcelWorksheet* sheet = demo.GetWorksheet("Sheet1");
	
	BasicExcelCell* cell = sheet->Cell(0, 0);
	if (sheet)
	{
		for (int i = 0; i < row; ++i) {
			for (int j = 0; j < col; ++j) {
				cell = sheet->Cell(i, j);
				cell->SetDouble((double)inputArr[i][j]);
				// 4X4时报错
				// row 不能超过65536 ， col 不能超过 256
			}
		}
	}
	
	demo.SaveAs(string(folder + "\\" + fileName).c_str());

	return true;
}
