#include "ImprovedMethod.h"
#include <algorithm>

ImprovedMethod::ImprovedMethod(int symbols, int winrow, int wincol) : _win_symbols(symbols), _win_row(winrow), _win_col(wincol), _step(winrow * wincol), _win_sum(pow(_win_symbols, _step)), _errFlag(false)
{	
	//_array_col = _array_row = sqrt((double)_win_sum );
	//_array_col = _array_row = sqrt((double)_win_sum / 2) - 1  ;
	//_array_col = _array_row = sqrt((double)_win_sum / 3)  + 13;
	//_array_col = _array_row = sqrt((double)_win_sum / 4 ) ;


	//_thresholdArea = (_win_row  * _win_col )/ 2 ;
	//_thresholdEdge = (2 * _win_row  * _win_col - _win_row - _win_col) / 3  ;    //max edge count is 2mn-m-n

	_thresholdArea = 10;
	_thresholdEdge = 0;
	_array_col = _array_row = 21;

	convKernelInit();

	char path[256];
	GetCurrentDirectory(256, path);
	_url = string(path) + "\\";
	_folderName = "MArray_" + string(typeid(*this).name());
}
//��xml��ȡ��ԭ����ʽ��Ϣ
vector<int> ImprovedMethod::xmlParse(int step)
{
	vector<int> res(step, 0);
	string elementStr = "element_" + to_string(_win_symbols);
	string stepStr = "step_" + to_string(step);

	TiXmlDocument doc;
	if (!doc.LoadFile("config.xml")) {  //�����ĵ�
		std::cerr << doc.ErrorDesc() << std::endl;
		exit(1);
	}
	TiXmlElement* root = doc.FirstChildElement();   //��ȡ���ڵ㣬Ҳ��xml�ĵ�����������
	if (root == NULL) {
		std::cerr << "Failed to load file: No root element." << std::endl;
		doc.Clear();
		exit(1);
	}

	TiXmlNode *ELE_node = NULL;      //����element�ڵ�
	ELE_node = root->IterateChildren(elementStr.data(), ELE_node);
	TiXmlElement *ELE_elem = ELE_node->ToElement();

	TiXmlNode *step_node = NULL;     //����step�ڵ�
	step_node = ELE_elem->IterateChildren(stepStr.data(), step_node);

	string temp = step_node->ToElement()->GetText();
	if (temp.size() > step) {
		cout << "config.xml file format error !!! " << endl;
		return vector<int> {};
	}
	for (int i = 0; i < temp.size(); ++i)
		res[i] = temp[i] - '0';
	return res;
}
//M���й���
bool ImprovedMethod::m_sequenceGenerating()
{
	{
		int step = _win_row * _win_col;
		vector<int> paraInfo = xmlParse(step);				 //��ȡ��ԭ����ʽ��Ϣ
		if (paraInfo.empty()) {
			_errFlag = true;
			_errInfo = "The primitive polynomial ss not found ... ";
			cout << _errInfo << endl;
			return false;
		}
		map<int, int> para_hash;   //��ȡ�ݴκͲ���
		for (int i = 0; i < paraInfo.size(); ++i) if (paraInfo[i] != 0) para_hash[i] = paraInfo[i];
		_m_sequence.clear();
		for (int i = 0; i < _step; ++i) _m_sequence.push_back(i % _win_symbols);		 //��ʼֵ
		//for (int i = 0; i < step; ++i) _m_sequence.push_back(0);		 //��ʼֵ
		//_m_sequence.front() = 1;
		if (_win_symbols == 2) {															 //LFSR ����M����
			for (int i = 0; i < _win_sum - step - 1; ++i) {
				int temp = 1;
				for (map<int, int>::iterator iter = para_hash.begin(); iter != para_hash.end(); iter++) {
					temp ^= _m_sequence[i + iter->first];
				}
				temp ^= 1;
				_m_sequence.push_back(temp);
			}
		}
		else {
			for (int i = 0; i < _win_sum - step - 1; ++i) {
				int temp = 0;
				for (map<int, int>::iterator iter = para_hash.begin(); iter != para_hash.end(); iter++) {
					temp += _m_sequence[i + iter->first] * iter->second;
				}
				temp %= _win_symbols;
				if (temp) temp = _win_symbols - temp;
				_m_sequence.push_back(temp);
			}
		}
	}

	// sub m-sequence
	{
		int step = _win_col;
		vector<int> paraInfo = xmlParse(step);				 //��ȡ��ԭ����ʽ��Ϣ
		if (paraInfo.empty()) {
			_errFlag = true;
			_errInfo = "The primitive polynomial ss not found ... ";
			cout << _errInfo << endl;
			return false;
		}
		map<int, int> para_hash;   //��ȡ�ݴκͲ���
		for (int i = 0; i < paraInfo.size(); ++i) if (paraInfo[i] != 0) para_hash[i] = paraInfo[i];
		_sub_m_sequence.clear();
		//for (int i = 0; i < step; ++i) _sub_m_sequence.push_back(i % _win_symbols);		 //��ʼֵ
		for (int i = 0; i < step; ++i) _sub_m_sequence.push_back(0);		 //��ʼֵ
		_sub_m_sequence.front() = 1;
		if (_win_symbols == 2) {															 //LFSR ����M����
			for (int i = 0; i < pow(_win_symbols, step) - step; ++i) {
				int temp = 1;
				for (map<int, int>::iterator iter = para_hash.begin(); iter != para_hash.end(); iter++) {
					temp ^= _sub_m_sequence[i + iter->first];
				}
				temp ^= 1;
				_sub_m_sequence.push_back(temp);
			}
		}
		else {
			for (int i = 0; i < _win_sum - step; ++i) {
				int temp = 0;
				for (map<int, int>::iterator iter = para_hash.begin(); iter != para_hash.end(); iter++) {
					temp += _sub_m_sequence[i + iter->first] * iter->second;
				}
				temp %= _win_symbols;
				if (temp) temp = _win_symbols - temp;
				_sub_m_sequence.push_back(temp);
			}
		}
	}

	return true;
}
//ȡһ����������ƴƬ
inline int ImprovedMethod::getOneValue()
{
	static int index = 0;
	return _m_sequence[(++index) %= (_win_sum - 1)];
	//Ϊʲô���治��
	//static int temp_win_sum = _sub_m_sequence.size();
	//return _sub_m_sequence[(++index) %= temp_win_sum];
}
//��ʼ������
void ImprovedMethod::dataInit()
{
	_MArray.resize(_array_row, vector<int>(_array_col, 0));
	_codewordMap.resize(_win_sum + 1, false);
	_dp_height.resize(_array_row, vector<int>(_array_col, 0));
	_dp_codeword.resize(_array_row, vector<int>(_array_col, _win_sum));
	_backCnt.resize(_array_row, vector<int>(_array_col, 0));
	_deep_backCnt.resize(_array_row, vector<int>(_array_col, 0));
	_debugImg = Mat(_array_row, _array_col, CV_8UC1, Scalar(127));   //������ 
}
//���������
bool ImprovedMethod::algorithmRun()
{
	dataInit();
	if(!m_sequenceGenerating()) return false; 
	int row = 0, col = 0;
	while(row < _array_row){
		col = 0;
		cout << " row : " << row << endl;
		while(col  < _array_col){
			if (_backCnt[row][col] == 0) _MArray[row][col] = getOneValue();									//��ȡ�����һ��Ԫ��
			else if (_backCnt[row][col] <= _win_symbols) _MArray[row][col] = (_MArray[row][col] + 1) % _win_symbols;								//һ���ع� ���� ��ǰ��
			else if (_deep_backCnt[row][col] <= _win_col ) {
				if (!rollbackOnRow(row, col, _win_col)) return false;												//�����ع� ���� _win_col ��
				_backCnt[row][col] ++;
				_deep_backCnt[row][col] ++;
				continue;
			}
			else {
				if (!rollbackOnRow(row, col, _array_col)) return false;												//�����ع� ����  һ��	
				_backCnt[row][col] ++;
				_deep_backCnt[row][col] ++;
				continue;
			}

			//_debugImg.at<uchar>(row, col) = _MArray[row][col] * 255;

			if (edgeCheck(row, col) && areaCheck(row, col) && winPropertyCheck(row, col)) col++;									//��֤�������  ��������     
			else _backCnt[row][col] ++;
						
			//if (!areaCheck(row, col)){
			//	_backCnt[row][col] ++;								//��֤�������
			//	continue;
			//}
			//if (winPropertyCheck(row, col)) col++;													    //��������      
			//else _backCnt[row][col] ++;
		}
		row++;
	}
	return true;
}
void ImprovedMethod::MArrayFileSave()
{
	if (_errFlag) return;
	string folder = _url + _folderName;
	if (GetFileAttributes(folder.c_str()) != FILE_ATTRIBUTE_DIRECTORY)    //��鲢������Ӧ�ļ���
		if (!CreateDirectory(folder.c_str(), NULL)) {
			cout << ends << "Create Folder Failed !!!" << endl;
			return ;
		}
	time_t cur_time = time(0);
	string sizeInfo = to_string(_win_symbols) + "_" + to_string(_win_row) + "x" + to_string(_win_col) + "_" + to_string(_array_row) + "x" + to_string(_array_col);
	// file save 
	string fileName = "MArray_" + sizeInfo + "_" + to_string(cur_time) + ".txt";
	if(!SaveTxtLocal(_MArray, fileName, folder))cout << ends << "save m-array file failed !!!" << endl;
	//image save 
	string imgName = "MArray_" + sizeInfo + "_" + to_string(cur_time) + ".bmp";
	Mat img = imageShow(_MArray);
	if (!SaveImgLocal(img, imgName, folder)) cout << ends << "save image failed !!!" << endl;
}
//��ʼ�������
void ImprovedMethod::convKernelInit()
{
	_conv_kernel.resize(_win_row, vector<int>(_win_col, 0));
	int shift = 1;
	for (int i = 0; i < _win_row; ++i){
		for (int j = 0; j < _win_col; ++j){
			_conv_kernel[i][j] = shift;
			shift *= _win_symbols;
		}
	}
}
//����������֣����봰�����½ǵ�����
int ImprovedMethod::convWindow(int rowInd, int colInd)
{
	if (rowInd < _win_row - 1 || colInd < _win_col - 1) return _win_sum ;  //��������Ч����
	int temp = 0;
	for (int i = 0; i < _win_row; ++i)
		for (int j = 0; j < _win_col; ++j)
			temp += (_MArray[rowInd - i][colInd - j] * _conv_kernel[i][j]);
	return temp;
}
//����ֱ��ͼ�������
int ImprovedMethod::calArea(vector<int>& heights)
{
	//if (heights.empty()) return 0;
	//if (heights.size() == 1) return heights.front();
	//stack<int> sta;
	//sta.push(heights.front());
	//int resArea = heights.front();
	//for (int i = 1; i < heights.size(); ++i){
	//	sta.push(min(heights[i], sta.top()));
	//	int curArea = sta.top() * sta.size();
	//	resArea = max(resArea, curArea);
	//}
	//return resArea;

	//�����г���������
	//if (heights.empty()) return 0;
	//if (heights.size() == 1) return min(heights.front(), this->_win_row);
	//if(heights.front() <= 1) return min(heights.size(), this->_win_col);

	if (heights.size() <= 1 || heights.front() <= 1) return 1;

	stack<int> sta;
	sta.push(heights.front());
	int resArea = heights.front();
	for (int i = 1; i < heights.size() && heights[i] > 1; ++i) {
		sta.push(min(heights[i], sta.top()));
		int curArea = sta.top() * sta.size();
		resArea = max(resArea, curArea);
	}
	return resArea;
}
//��֤��ǰ����������Ե�ǰindexΪ���µ׽ǵ�������
bool ImprovedMethod::areaCheck(int & rowInd, int & colInd)
{
	if (rowInd == 0 || _MArray[rowInd][colInd] != _MArray[rowInd - 1][colInd]) _dp_height[rowInd][colInd] = 1;
	else _dp_height[rowInd][colInd] = _dp_height[rowInd - 1][colInd] + 1;
	vector<int> heights;
	heights.push_back(_dp_height[rowInd][colInd]);
	for (int k = colInd - 1; k >= 0; --k) {
		if (_MArray[rowInd][k] != _MArray[rowInd][colInd]) break;
		heights.push_back(_dp_height[rowInd][k]);
	}
	int localArea = calArea(heights);
	if (localArea > _thresholdArea) return false;	  // area should <= _thresholdArea	
	return true;
}
//��֤�����ڱ�Ե����
bool ImprovedMethod::edgeCheck(int & rowInd, int & colInd)
{
	if (rowInd < _win_row - 1 || colInd < _win_col - 1) return true;

	int edgeCnt = 0;
	for (int i = 0; i < _win_row; ++i) {
		for (int j = 0; j < _win_col - 1; ++j) {
			if (_MArray[rowInd - i][colInd - j] != _MArray[rowInd - i][colInd - j - 1]) edgeCnt++;
		}
	}
	for (int j = 0; j < _win_col; ++j) {
		for (int i = 0; i < _win_row - 1; ++i) {
			if (_MArray[rowInd - i][colInd - j] != _MArray[rowInd - i - 1][colInd - j]) edgeCnt++;
		}
	}
	if (edgeCnt < _thresholdEdge)
	{
		return false;
	}
	return true;
}
//��֤��������
bool ImprovedMethod::winPropertyCheck(int & rowInd, int & colInd)
{
	if (rowInd < _win_row - 1 || colInd < _win_col - 1) return true;
	int codeword = convWindow(rowInd, colInd);
	if (_codewordMap[codeword] == true) {
		return false;
	}
	else {
		_dp_codeword[rowInd][colInd] = codeword;
		_codewordMap[codeword] = true;
		//_tempMap[codeword] = true;
	}
	return true;
}
//���лع� cnt ������
bool ImprovedMethod::rollbackOnRow(int & rowInd, int & colInd, int cnt)
{
	if (cnt <= 0) return false;

	//�ȳ���һ��    ��Ϊ��һ�����ó��� _codewordMap
	//_MArray[rowInd][colInd] = 0;
	//_debugImg.at<uchar>(rowInd, colInd) = 127;
	_backCnt[rowInd][colInd] = 0;
	_deep_backCnt[rowInd][colInd] /= 2;

	cnt--;
	colInd--;
	if (colInd < 0)
	{
		colInd = _array_col - 1;
		rowInd--;
		if (rowInd < 0)
		{
			dataInit();
			rowInd = 0;
			colInd = 0;
			return true;    //���¿�ʼ
		}
	}

	//��������cnt-1��
	while (cnt--) {
		//_MArray[rowInd][colInd] = 0;
		//_debugImg.at<uchar>(rowInd, colInd) = 127;
		_backCnt[rowInd][colInd] = 0;
		_deep_backCnt[rowInd][colInd] /= 2 ;
		_codewordMap[_dp_codeword[rowInd][colInd]] = false;
		//_tempMap.erase(_dp_codeword[rowInd][colInd]);
		colInd--;
		if (colInd < 0)
		{
			colInd = _array_col - 1;
			rowInd--;
			if (rowInd < 0)
			{
				dataInit();
				rowInd = 0;
				colInd = 0;
				return true;    //���¿�ʼ
			}
		}
	}

	//  �� [rowInd][colInd] ������  ���� _codewordMap[_dp_codeword[rowInd][colInd]] ��Ҫ����
	_codewordMap[_dp_codeword[rowInd][colInd]] = false;

	return true;
}

