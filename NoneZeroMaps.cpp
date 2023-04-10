// pch.cpp: 与预编译标头对应的源文件；编译成功所必需的

#include "NoneZeroMaps.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include <opencv2\opencv.hpp>

using namespace std;

NoneZeroMaps::NoneZeroMaps(int symbols,int winrow, int wincol): _win_symbols(symbols), _win_row(winrow), _win_col(wincol) , _step(winrow * wincol)
{
	this->_win_sum = pow(_win_symbols, _step) - 1;
	_errFlag = false;

	char path[256];
	GetCurrentDirectory(256, path);
	_url = string(path) + "\\";
	_folderName = "MArray_" + string(typeid(*this).name());
}

//M序列构造
bool NoneZeroMaps::m_sequenceGenerating()
{
	vector<int> paraInfo = xmlParse();				 //获取本原多项式信息
	if (paraInfo.empty()) {
		_errFlag = true;
		_errInfo = "The primitive polynomial ss not found ... ";
		cout << _errInfo << endl;
		return false;
	}
	map<int, int> para_hash;   //获取幂次和参数
	for (int i = 0; i < paraInfo.size(); ++i) if (paraInfo[i] != 0) para_hash[i] = paraInfo[i];
	_m_sequence.clear();
	for (int i = 0; i < _step; ++i) _m_sequence.push_back(i % _win_symbols);		 //初始值
	if (_win_symbols == 2) {															 //LFSR 构造M序列
		for (int i = 0; i < _win_sum - _step; ++i) {
			int temp = 1;
			for (map<int, int>::iterator iter = para_hash.begin(); iter != para_hash.end(); iter++) {
				temp ^= _m_sequence[i + iter->first];
			}
			temp ^= 1;
			_m_sequence.push_back(temp);
		}
	}
	else {
		for (int i = 0; i < _win_sum - _step; ++i) {
			int temp = 0;
			for (map<int, int>::iterator iter = para_hash.begin(); iter != para_hash.end(); iter++) {
				temp += _m_sequence[i + iter->first] * iter->second;
			}
			temp %= _win_symbols;
			if (temp) temp = _win_symbols - temp;
			_m_sequence.push_back(temp);
		}
	}
	return true;
}

void NoneZeroMaps::algorithmRun()
{	
	m_sequenceGenerating();
	if (_m_sequence.empty()){
		_errFlag = true;
		_errInfo = "M-sequence is empty !!!";
		cout << ends << _errInfo << endl;
		return;
	}
	dataInit();	
	int colSize = getShiftSize();   //获取最佳移位 位数  作为M阵列的列数
	if (colSize < 0){
		_errFlag = true;
		_errInfo = "property shift num is not found!!!";
		cout<< ends << _errInfo << endl;
		return ;
	}
	int rowSize = _win_sum / colSize;
	//resImage = Mat(rowSize + win_row - 1, colSize + win_col - 1, CV_8UC1, Scalar(127));
	//构造二维阵列
	//for (int i = 0; i < rowSize ; ++i)
	for (int i = 0; i < rowSize + _win_row - 1; ++i){
		_MArray.push_back(vector<int>{});
		//for (int j = 0; j < colSize ; ++j)
		for (int j = 0; j < colSize + _win_col - 1; ++j){
			int index = i * colSize + j;
			if (index >= _win_sum) index -= _win_sum;
			_MArray.back().push_back(_m_sequence[index]);
			//resImage.at<uchar>(i, j) = M_Array[index]? 255:0;
		}
	}
	_array_row = _MArray.size();
	_array_col = _MArray.front().size();
	return ;
}

void NoneZeroMaps::MArrayFileSave()
{
	if (_errFlag) return;
	string folder = _url + _folderName;
	if (GetFileAttributes(folder.c_str()) != FILE_ATTRIBUTE_DIRECTORY)
		if (!CreateDirectory(folder.c_str(), NULL)) {
			cout << ends << "Create Folder Failed !!!" << endl;
			return;
		}
	time_t cur_time = time(0);
	string sizeInfo = to_string(_win_symbols) + "_" + to_string(_win_row) + "x" + to_string(_win_col) + "_" + to_string(_array_row) + "x" + to_string(_array_col);
	// file save 
	string fileName = "MArray_" + sizeInfo + "_" + to_string(cur_time) + ".txt";
	if (!SaveTxtLocal(_MArray, fileName, folder))cout << ends << "save m-array file failed !!!" << endl;
	//image save 
	string imgName = "MArray_" + sizeInfo + "_" + to_string(cur_time) + ".bmp";
	Mat img = imageShow(_MArray);
	if (!SaveImgLocal(img, imgName, folder)) cout << ends << "save image failed !!!" << endl;
}

int NoneZeroMaps::getShiftSize()
{
	//double temp = sqrt((double)win_sum + 1);
	//temp *= 2;
	//temp /= sqrt(3);
	//int sqrtRoot = temp;   //构造3：4的阵列 

	/*double temp = sqrt((double)win_sum + 1);
	temp *= 3;
	temp /= 4;
	int sqrtRoot = temp;   //构造16：9的阵列 */

	//double temp = sqrt((double)win_sum + 1);
	//temp *= sqrt((double)2);
	//temp /= sqrt((double)3);
	//int sqrtRoot = temp;   //构造3：2的阵列 

	int sqrtRoot = sqrt((double)_win_sum +1);
	int left = sqrtRoot-1;   //从sqrtRoot向两侧找位移数
	int right = sqrtRoot;

	// 验证移位后的有效性  //最多执行 win_row * win_col 次
	for (int i = 0; i < _win_row * _win_col; ++i){
		map<int, bool>::iterator iter;
		for (iter = hash_zeroIndex.begin(); iter != hash_zeroIndex.end(); iter++){
			bool flag = true;  //当前位移数能否产生全零窗口
			for (int k = 1; k < _win_row ; ++k)	{
				int valueTemp = iter->first + k * right;
				valueTemp %= _win_sum;
				if (hash_zeroIndex.find(valueTemp) == hash_zeroIndex.end()) { //找不到
					flag = false;
					break;
				}
			}
			if (flag) break;  //出现了全零窗口    true表示出现了全零窗口，当前检验的移位数无效
		}
		if (iter == hash_zeroIndex.end()) return right;
		for (iter = hash_zeroIndex.begin(); iter != hash_zeroIndex.end(); iter++){
			bool flag = true;                           //当前位移数能否产生全零窗口
			for (int k = 1; k < _win_row ; ++k){
				int valueTemp = iter->first + k * left;
				valueTemp %= _win_sum;
				if (hash_zeroIndex.find(valueTemp) == hash_zeroIndex.end()){
					flag = false;
					break;
				}
			}
			if (flag) break;
		}
		if (iter == hash_zeroIndex.end()) return left;
		left--;
		right++;
	}

	return -1 ;
}

void NoneZeroMaps::dataInit()
{
	int right = this->_win_sum - 1;    //记录连续 win_col个0 的位置索引
	while (right >= 0){
		if (_m_sequence[right] == 0){
			int left = right - 1;
			while (left >= 0 && this->_m_sequence[left] == 0){
				if (right - left + 1 >= this->_win_col) hash_zeroIndex[left] = true;  //true表示当前索引为0
				left--;
			}
			right = left;
		}
		else right--;
	}
}
//从xml读取本原多项式信息
vector<int> NoneZeroMaps::xmlParse()
{
	vector<int> res(_step, 0);
	string elementStr = "element_" + to_string(_win_symbols);
	string stepStr = "step_" + to_string(_step);

	TiXmlDocument doc;
	if (!doc.LoadFile("config.xml")) {  //加载文档
		std::cerr << doc.ErrorDesc() << std::endl;
		exit(1);
	}
	TiXmlElement* root = doc.FirstChildElement();   //获取根节点，也是xml文档的声明部分
	if (root == NULL) {
		std::cerr << "Failed to load file: No root element." << std::endl;
		doc.Clear();
		exit(1);
	}

	TiXmlNode *ELE_node = NULL;      //查找element节点
	ELE_node = root->IterateChildren(elementStr.data(), ELE_node);
	TiXmlElement *ELE_elem = ELE_node->ToElement();

	TiXmlNode *step_node = NULL;     //查找step节点
	step_node = ELE_elem->IterateChildren(stepStr.data(), step_node);

	string temp = step_node->ToElement()->GetText();
	if (temp.size() > _step) {
		cout << "config.xml file format error !!! " << endl;
		return vector<int> {};
	}
	for (int i = 0; i < temp.size(); ++i)
		res[i] = temp[i] - '0';
	return res;
}

// 一般情况下，忽略此文件，但如果你使用的是预编译标头，请保留它。
