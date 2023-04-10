#include "MArrayCheck.h"
#include <map>

MArrayCheck::MArrayCheck(const vector<vector<int>>& arrayCode, int arrayRow, int arrayCol, int winRow, int winCol, int symbols) : _arrayCode(arrayCode), _arrayRow(arrayRow), _arrayCol(arrayCol), _winRow(winRow), _winCol(winCol) , _symbols(symbols), _convKernel(winRow , vector<int> (winCol , 0)), _errFlag(false)
{
	_symGlbBalance = -1;
	_hanmingAverage = -1;
	_hanmingMinum = _winRow * _winCol;
	_winProperty = false;

	this->_RC_ratio = (double)_arrayRow / (double)_arrayCol;
	this->_CR_ratio = (double)_arrayCol / (double)_arrayRow;
	this->_winSum = (_arrayRow - _winRow + 1) * (_arrayCol - _winCol + 1);

	convKernelInit();

	char path[256];
	GetCurrentDirectory(256, path);
	_url = string(path) + "\\";
	_folderName = "MArray_" + string(typeid(*this).name());
}
//元素均衡   全局 局部      // 多个symbol ，（最大值-最小值）/（最大值+最小值）  衡量元素均衡性  两个symbol情况下 = （最大值-最小值）/ 窗口大小
void MArrayCheck::symbolsPercentageCal()
{
	vector<vector<vector<int>>> dpAry(_arrayRow , vector<vector<int>> (_arrayCol, vector<int>(_symbols , 0)));   //记录各元素的数量 三维数组
	for (int i = 0; i < _arrayRow; ++i){
		for (int j = 0; j < _arrayCol; ++j){
			if (i == 0 && j == 0) dpAry[i][j][_arrayCode[i][j]] = 1;
			else if (i == 0){
				for (int k = 0; k < _symbols; ++k) dpAry[i][j][k] = dpAry[i][j - 1][k];
				dpAry[i][j][_arrayCode[i][j]] ++;
			}
			else if (j == 0){
				for (int k = 0; k < _symbols; ++k) dpAry[i][j][k] = dpAry[i-1][j][k];
				dpAry[i][j][_arrayCode[i][j]] ++;
			}
			else{
				for (int k = 0; k < _symbols; ++k) dpAry[i][j][k] = dpAry[i][j-1][k] + dpAry[i - 1][j][k] - dpAry[i - 1][j - 1][k];
				dpAry[i][j][_arrayCode[i][j]] ++;
			}
		}
	}
	int maxV = dpAry.back().back().back();
	int minV = dpAry.back().back().back();
	for (int k = 0; k < _symbols; ++k)
	{
		minV = min(minV, dpAry.back().back().at(k));
		maxV = max(maxV, dpAry.back().back().at(k));
	}
	this->_symGlbBalance = (double)(maxV - minV) / (double)(maxV + minV);

	_symbolsPercentage_max = 0;
	_symbolsPercentage.resize(_arrayRow - _winRow + 1, vector<double>(_arrayCol - _winCol + 1, 0)); 
	for (int i = 0; i < _arrayRow - _winRow + 1; ++i){
		for (int j = 0; j < _arrayCol - _winCol + 1; ++j){
			maxV = 0;
			minV = _winRow * _winCol;
			for (int k = 0; k < _symbols; ++k)      //计算局部窗口各元素占比
			{
				int temp = dpAry[i + _winRow - 1][j + _winCol - 1][k];
				if (i > 0) temp -= dpAry[i - 1][j + _winCol - 1][k];
				if (j > 0) temp -= dpAry[i + _winRow - 1][j - 1][k];
				if (i > 0 && j > 0) temp += dpAry[i - 1][j - 1][k];
				maxV = max(maxV, temp);
				minV = min(minV, temp);
			}
			_symbolsPercentage[i][j] = (double)(maxV - minV) / (double)(maxV + minV);
			_symbolsPercentage_max = max(_symbolsPercentage_max, _symbolsPercentage[i][j]);
		}
	}
}
//窗口特性验证
void MArrayCheck::winPropertyCal()
{
	for (int i = 0; i < _arrayRow - _winRow + 1; ++i){
		for (int j = 0; j < _arrayCol - _winCol + 1; ++j){
			int temp = convolution(i , j);
			//if (_winMap.find(temp) != _winMap.end())
			//{
			//	cout << "error happend i  j  is " << i << " " << j << endl;
			//}
			_winMap[temp] = 1;
		}
	}
	this->_winProperty = (_winMap.size() == this->_winSum ? true : false);
}

//void MArrayCheck::hanmingCal()
//{
//	double sumVal = 0;
//	vector<vector<int>> hanmingDisRow(_arrayRow - _winRow + 1, vector<int>(_arrayCol - _winCol, 0));
//	for (int i = 0; i < _arrayRow - _winRow + 1; ++i)
//	{
//		for (int j = 0; j < _arrayCol - _winCol ; ++j)
//		{
//			int temp = 0;
//			for (int m = 0; m < _winRow; ++m)
//			{
//				for (int n = 0; n < _winCol; ++n)
//				{
//					temp += _arrayCode[i + m][j + n] ^ _arrayCode[i + m][j + n + 1];
//				}
//			}
//			hanmingDisRow[i][j] = temp;
//			sumVal += temp;
//		}
//	}
//	this->_hanmingAverage_row = sumVal / (double)((_arrayRow - _winRow + 1)*(_arrayCol - _winCol));
//	this->_hanmingDistance_row = hanmingDisRow;
//
//	sumVal = 0;
//	vector<vector<int>> hanmingDisCol(_arrayRow - _winRow , vector<int>(_arrayCol - _winCol + 1, 0));
//	for (int i = 0; i < _arrayRow - _winRow ; ++i)
//	{
//		for (int j = 0; j < _arrayCol - _winCol + 1; ++j)
//		{
//			int temp = 0;
//			for (int m = 0; m < _winRow; ++m)
//			{
//				for (int n = 0; n < _winCol; ++n)
//				{
//					temp += _arrayCode[i + m][j + n] ^ _arrayCode[i + m + 1][j + n ];
//				}
//			}
//			hanmingDisCol[i][j] = temp;
//			sumVal += temp;
//		}
//	}
//	this->_hanmingAverage_col = sumVal / (double)((_arrayRow - _winRow)*(_arrayCol - _winCol + 1));
//	this->_hanmingDistance_col = hanmingDisCol;
//}

void MArrayCheck::hanmingCal()  // 计算平均汉明距离，最小汉明距离
{
	if (!this->_winProperty) return;
	map<int, int>::iterator iter = this->_winMap.begin();
	int tempValue = 0;
	double hanmingValue = 0;
	int layerCnt = 0;
	while (iter != _winMap.end()){
		map<int, int>::iterator iterSub = iter;
		while ((++iterSub) != _winMap.end()){
			tempValue = hanmingCount(iter->first, iterSub->first);
			this->_hanmingMinum = min(_hanmingMinum, tempValue);
			layerCnt++;
			hanmingValue += tempValue;
		}
		iter++;		
	}
	this->_hanmingAverage = hanmingValue / (double)layerCnt;
}

int MArrayCheck::hanmingCount(int left, int right)
{
	int cnt = 0;
	while (left || right){
		if (left % _symbols != right % _symbols) cnt++;
		left /= _symbols;
		right /= _symbols;
	}
	return cnt;
}

//领域相同元素包围而成的矩形
//每个点所在的最大矩形面积
void MArrayCheck::neighborsRectangleCal()    // 参考leetcode  剑指 Offer II 040. 矩阵中最大的矩形
{
	// up to down
	vector<int> dp_up_down(_arrayCol, 1);        //基于当前行的直方图  列方向上相同元素个数
	vector<vector<int>> dpRectangle_up_down(_arrayRow, vector<int>(_arrayCol, 1));   //基于当前行的最大矩形
	for (int i = 0; i < _arrayRow; ++ i){
		for (int j = 0; j < _arrayCol; ++j) {
			if (i > 0 && _arrayCode[i][j] == _arrayCode[i-1][j]){
				dp_up_down[j]++;
			}
			else{
				dp_up_down[j] = 1;
			}
		}
		for (int left = 0; left < _arrayCol; ){
			int right = left + 1;
			for (; right < _arrayCol; ++right)	if (_arrayCode[i][right] != _arrayCode[i][left]) break;
			rectangleArea(dp_up_down, left, right - 1, dpRectangle_up_down[i]);
			left = right;
		}
	}

	// down to up
	vector<int> dp_down_up(_arrayCol, 1);        //基于当前行的直方图  列方向上相同元素个数
	vector<vector<int>> dpRectangle_down_up(_arrayRow, vector<int>(_arrayCol, 1));   //基于当前行的最大矩形
	for (int i = _arrayRow - 1; i >= 0; --i){
		for (int j = 0; j < _arrayCol; ++j){
			if (i < _arrayRow - 1 && _arrayCode[i][j] == _arrayCode[i + 1][j]) {
				dp_down_up[j]++;
			}
			else{
				dp_down_up[j] = 1;
			}
		}
		for (int left = 0; left < _arrayCol; ){
			int right = left + 1;
			for (; right < _arrayCol; ++right)	if (_arrayCode[i][right] != _arrayCode[i][left]) break;
			rectangleArea(dp_down_up, left, right - 1, dpRectangle_down_up[i]);
			left = right;
		}
	}

	// left to right
	vector<int> dp_left_right(_arrayRow, 1);        //基于当前行的直方图  列方向上相同元素个数
	vector<vector<int>> dpRectangle_left_right(_arrayRow, vector<int>(_arrayCol, 1));   //基于当前行的最大矩形
	for (int j = 0; j < _arrayCol; ++j){
		for (int i = 0; i < _arrayRow; ++i){
			if (j > 0 && _arrayCode[i][j] == _arrayCode[i][j-1]) {
				dp_left_right[i]++;
			}
			else{
				dp_left_right[i] = 1;
			}
		}
		vector<int> temp(_arrayRow, 1);
		for (int left = 0; left < _arrayRow; ){
			int right = left + 1;
			for (; right < _arrayRow; ++right)	if (_arrayCode[right][j] != _arrayCode[left][j]) break;

			rectangleArea(dp_left_right, left, right - 1, temp);
			left = right;
		}
		for (int i = 0; i < _arrayRow; ++i) dpRectangle_left_right[i][j] = temp[i];
	}

	// right to left
	vector<int> dp_right_left(_arrayRow, 1);        //基于当前行的直方图  列方向上相同元素个数
	vector<vector<int>> dpRectangle_right_left(_arrayRow, vector<int>(_arrayCol, 1));   //基于当前行的最大矩形
	for (int j = _arrayCol - 1; j >= 0; --j){
		for (int i = 0; i < _arrayRow; ++i){
			if (j < _arrayCol - 1 && _arrayCode[i][j] == _arrayCode[i][j + 1]) {
				dp_right_left[i]++;
			}
			else{
				dp_right_left[i] = 1;
			}
		}
		vector<int> temp(_arrayRow, 1);
		for (int left = 0; left < _arrayRow; ){
			int right = left + 1;
			for (; right < _arrayRow; ++right)	if (_arrayCode[right][j] != _arrayCode[left][j]) break;

			rectangleArea(dp_right_left, left, right - 1, temp);
			left = right;
		}
		for (int i = 0; i < _arrayRow; ++i) dpRectangle_right_left[i][j] = temp[i];
	}

	//根据 dpRectangle 求局部的矩形面积
	//根据上述四个 dpRectangle ，每个点取最大值即可
	_neighborsRectangle_max = 0;
	_neighborsRectangle.resize(_arrayRow, vector<int>(_arrayCol, 0));
	for (int i = 0; i < _arrayRow; ++i){
		for (int j = 0; j < _arrayCol; ++j)	{
			this->_neighborsRectangle[i][j] = max(_neighborsRectangle[i][j], dpRectangle_up_down[i][j]);
			this->_neighborsRectangle[i][j] = max(_neighborsRectangle[i][j], dpRectangle_down_up[i][j]);
			this->_neighborsRectangle[i][j] = max(_neighborsRectangle[i][j], dpRectangle_left_right[i][j]);
			this->_neighborsRectangle[i][j] = max(_neighborsRectangle[i][j], dpRectangle_right_left[i][j]);

			//_neighborsRectangle_max = max(_neighborsRectangle_max, this->_neighborsRectangle[i][j]);
		}
	}

	_neighborsRectangle_double.resize(_arrayRow, vector<double>(_arrayCol, 0));
	for (int i = 0; i < _arrayRow; ++i) {
		for (int j = 0; j < _arrayCol; ++j) {
			this->_neighborsRectangle_double[i][j] = (double)_neighborsRectangle[i][j] / ((double)_winRow * _winCol);
			_neighborsRectangle_max = max(_neighborsRectangle_max, this->_neighborsRectangle_double[i][j]);
		}
	}

	//// 允许单列连续元素
	//_neighborsRectangle_max = 0;
	//bool singleFlag = false;
	//for (int i = 0; i < _arrayRow; ++i) {
	//	for (int j = 0; j < _arrayCol; ++j) {
	//		singleFlag = false;
	//		if (i == 0 && _arrayCode[i][j] != _arrayCode[i + 1][j]) singleFlag = true;
	//		else if(i == _arrayRow - 1 && _arrayCode[i][j] != _arrayCode[i - 1][j]) singleFlag = true;
	//		else if(i > 0 && i < _arrayRow - 1 && _arrayCode[i][j] != _arrayCode[i + 1][j] && _arrayCode[i][j] != _arrayCode[i - 1][j])singleFlag = true;

	//		if (singleFlag)
	//		{
	//			this->_neighborsRectangle[i][j] = min(this->_neighborsRectangle[i][j], _winCol);
	//			_neighborsRectangle_max = max(_neighborsRectangle_max, this->_neighborsRectangle[i][j]);
	//			continue;
	//		}

	//		if (j == 0 && _arrayCode[i][j] != _arrayCode[i][j+1]) singleFlag = true;
	//		else if (j == _arrayCol - 1 && _arrayCode[i][j] != _arrayCode[i][j-1]) singleFlag = true;
	//		else if (j > 0 && j < _arrayCol - 1 && _arrayCode[i][j] != _arrayCode[i][j+1] && _arrayCode[i][j] != _arrayCode[i][j-1])singleFlag = true;

	//		if (singleFlag)
	//		{
	//			this->_neighborsRectangle[i][j] = min(this->_neighborsRectangle[i][j], _winRow);
	//			_neighborsRectangle_max = max(_neighborsRectangle_max, this->_neighborsRectangle[i][j]);
	//			continue;
	//		}
	//	}
	//}


}
//邻域连通
void MArrayCheck::neighborsConnectedCal()
{
	_neighborsConnected.clear();
	_neighborsConnected.resize(_arrayRow, vector<int>(_arrayCol , 0));
	for (int i = 0; i < _arrayRow; ++i){
		for (int j = 0; j < _arrayCol; ++j){
			if (i > 0 && this->_arrayCode[i][j] == this->_arrayCode[i - 1][j]) _neighborsConnected[i][j] = _neighborsConnected[i - 1][j];
			else if ((j > 0 && this->_arrayCode[i][j] == this->_arrayCode[i][j - 1])) _neighborsConnected[i][j] = _neighborsConnected[i][j-1];
			else{
				vector<vector<bool>> visited(_arrayRow, vector<bool>(_arrayCol, false));
				_neighborsConnected[i][j] = neighborsExpandOut(i, j, visited);
				//_neighborsConnected_max = max(_neighborsConnected_max, _neighborsConnected[i][j]);
			}
		}
	}
	_neighborsConnected_max = 0;
	_neighborsConnected_double.resize(_arrayRow, vector<double>(_arrayCol, 0));
	for (int i = 0; i < _arrayRow; ++i) {
		for (int j = 0; j < _arrayCol; ++j) {
			this->_neighborsConnected_double[i][j] = (double)_neighborsConnected[i][j] / ((double)_winRow * _winCol);
			_neighborsConnected_max = max(_neighborsConnected_max, _neighborsConnected_double[i][j]);
		}
	}
}

void MArrayCheck::edgesPercentageCal()
{
	_edgesPercentage_max = 0;
	_edgesPercentage.resize(_arrayRow - _winRow + 1, vector<double>(_arrayCol - _winCol + 1, 0));
	for (int i = 0; i < _arrayRow - _winRow + 1; ++i) {
		for (int j = 0; j < _arrayCol - _winCol + 1; ++j) {
			int edgeCnt = 0;
			for (int m = 0; m < _winRow; ++m) {
				for (int n = 0; n < _winCol - 1; ++n) {
					if (_arrayCode[i + m][j+n] != _arrayCode[i + m][j + n + 1]) edgeCnt++;
				}
			}
			for (int n = 0; n < _winCol; ++n) {
				for (int m = 0; m < _winRow - 1; ++m) {
					if (_arrayCode[i + m][j + n] != _arrayCode[i + m + 1][j + n]) edgeCnt++;
				}
			}
			_edgesPercentage[i][j] = (double)edgeCnt / ((double)(2 * _winRow * _winCol - _winRow - _winCol));
			_edgesPercentage[i][j] = 1 / _edgesPercentage[i][j];     //取倒数，与其他指标归一化
			_edgesPercentage_max = max(_edgesPercentage_max, _edgesPercentage[i][j]);
		}
	}
	return;

}

void MArrayCheck::neighborsHanmingCal()
{
	_neighborsHanming_max = 0;
	_neighborsHanming_double.resize(_arrayRow - _winRow - 1 , vector<double>(_arrayCol - _winCol - 1 , 0));
	for (int i = 1; i <= _arrayRow - _winRow - 1; ++i) {
		for (int j = 1; j <= _arrayCol - _winCol - 1; ++j) {
			int hanming_up = 0;
			int hanming_dn = 0;
			int hanming_lf = 0;
			int hanming_rt = 0;
			for (int m = 0; m < _winRow; ++m) {
				for (int n = 0; n < _winCol; ++n) {
					if (_arrayCode[i + m][j + n] != _arrayCode[i + m - 1][j + n]) hanming_up++;
					if (_arrayCode[i + m][j + n] != _arrayCode[i + m + 1][j + n]) hanming_dn++;
					if (_arrayCode[i + m][j + n] != _arrayCode[i + m][j + n - 1]) hanming_lf++;
					if (_arrayCode[i + m][j + n] != _arrayCode[i + m][j + n + 1]) hanming_rt++;
				}
			}
			double hanming = (double)(hanming_up + hanming_dn + hanming_lf + hanming_rt) / 4;
			_neighborsHanming_double[i - 1][j - 1] = hanming;
			_neighborsHanming_double[i - 1][j - 1] /= (double)(_winRow * _winCol);
			_neighborsHanming_double[i - 1][j - 1] = 1 / _neighborsHanming_double[i - 1][j - 1];         //取倒数，与其他指标归一化
			_neighborsHanming_max = max(_neighborsHanming_double[i - 1][j - 1], _neighborsHanming_max);
		}
	}
	return;
}

void MArrayCheck::paraFileSave()
{
	if (_errFlag) return;
	string folder;
	string fileName;
	string imgName;	
	time_t cur_time = time(0);
	string sizeInfo = to_string(_symbols) + "_" + to_string(_winRow) + "x" + to_string(_winCol) + "_" + to_string(_arrayRow) + "x" + to_string(_arrayCol) ;

	folder = _url + _folderName ;
	if (GetFileAttributes(folder.c_str()) != FILE_ATTRIBUTE_DIRECTORY)    //检查并创建母文件夹
		if (!CreateDirectory(folder.c_str(), NULL)) {
			cout << ends << "Create Folder Failed !!!" << endl;
			return;
		}

	// _symbolsPercentage
	folder = _url + _folderName + "\\" + VNAME(_symbolsPercentage);
	if (GetFileAttributes(folder.c_str()) != FILE_ATTRIBUTE_DIRECTORY)    //检查并创建子文件夹
		if (!CreateDirectory(folder.c_str(), NULL)) {
			cout << ends << "Create Folder Failed !!!" << endl;
			return;
		}
	fileName = VNAME(_symbolsPercentage);   // file save 
	fileName += "_" + sizeInfo + "_" + to_string(cur_time) + ".txt";
	if (!SaveTxtLocal(_symbolsPercentage, fileName, folder))cout << ends << "save _symbolsPercentage file failed !!!" << endl;
	imgName = VNAME(_symbolsPercentage);    //image save 
	imgName += "_" + sizeInfo + "_" + to_string(cur_time) + ".bmp";
	if (!SaveImgLocal(_symbolsPerImg_hot, imgName, folder)) cout << ends << "save image failed !!!" << endl;

	// _neighborsRectangle
	folder = _url + _folderName + "\\" + VNAME(_neighborsRectangle_double);
	if (GetFileAttributes(folder.c_str()) != FILE_ATTRIBUTE_DIRECTORY)    //检查并创建子文件夹
		if (!CreateDirectory(folder.c_str(), NULL)) {
			cout << ends << "Create Folder Failed !!!" << endl;
			return;
		}
	fileName = VNAME(_neighborsRectangle_double);   // file save 
	fileName += "_" + sizeInfo + "_" + to_string(cur_time) + ".txt";
	if (!SaveTxtLocal(_neighborsRectangle_double, fileName, folder))cout << ends << "save _symbolsPercentage file failed !!!" << endl;
	imgName = VNAME(_neighborsRectangle_double);
	imgName += "_" + sizeInfo + "_" + to_string(cur_time) + ".bmp";
	if (!SaveImgLocal(_neighborsRectangleImg_hot, imgName, folder)) cout << ends << "save image failed !!!" << endl;

	// _neighborsConnected
	folder = _url + _folderName + "\\" + VNAME(_neighborsConnected_double);
	if (GetFileAttributes(folder.c_str()) != FILE_ATTRIBUTE_DIRECTORY)    //检查并创建子文件夹
		if (!CreateDirectory(folder.c_str(), NULL)) {
			cout << ends << "Create Folder Failed !!!" << endl;
			return;
		}
	fileName = VNAME(_neighborsConnected_double);   // file save 
	fileName += "_" + sizeInfo + "_" + to_string(cur_time) + ".txt";
	if (!SaveTxtLocal(_neighborsConnected_double, fileName, folder))cout << ends << "save _symbolsPercentage file failed !!!" << endl;
	imgName = VNAME(_neighborsConnected_double);   //image save 
	imgName += "_" + sizeInfo + "_" + to_string(cur_time) + ".bmp";
	if (!SaveImgLocal(_neighborsConnectedImg_hot, imgName, folder)) cout << ends << "save image failed !!!" << endl;

	// _edgesPercentage
	folder = _url + _folderName + "\\" + VNAME(_edgesPercentage);
	if (GetFileAttributes(folder.c_str()) != FILE_ATTRIBUTE_DIRECTORY)    //检查并创建子文件夹
		if (!CreateDirectory(folder.c_str(), NULL)) {
			cout << ends << "Create Folder Failed !!!" << endl;
			return;
		}
	fileName = VNAME(_edgesPercentage);   // file save 
	fileName += "_" + sizeInfo + "_" + to_string(cur_time) + ".txt";
	if (!SaveTxtLocal(_edgesPercentage, fileName, folder))cout << ends << "save _symbolsPercentage file failed !!!" << endl;
	imgName = VNAME(_edgesPercentage);   //image save 
	imgName += "_" + sizeInfo + "_" + to_string(cur_time) + ".bmp";
	if (!SaveImgLocal(_edgesPercentageImg_hot, imgName, folder)) cout << ends << "save image failed !!!" << endl;

	// _neighborsHanming_double
	folder = _url + _folderName + "\\" + VNAME(_neighborsHanming_double);
	if (GetFileAttributes(folder.c_str()) != FILE_ATTRIBUTE_DIRECTORY)    //检查并创建子文件夹
		if (!CreateDirectory(folder.c_str(), NULL)) {
			cout << ends << "Create Folder Failed !!!" << endl;
			return;
		}
	fileName = VNAME(_neighborsHanming_double);   // file save 
	fileName += "_" + sizeInfo + "_" + to_string(cur_time) + ".txt";
	if (!SaveTxtLocal(_neighborsHanming_double, fileName, folder))cout << ends << "save _symbolsPercentage file failed !!!" << endl;
	imgName = VNAME(_neighborsHanming_double);   //image save 
	imgName += "_" + sizeInfo + "_" + to_string(cur_time) + ".bmp";
	if (!SaveImgLocal(_neighborsHanmingImg_hot, imgName, folder)) cout << ends << "save image failed !!!" << endl;

}

int MArrayCheck::neighborsExpandOut(int row, int col, vector<vector<bool>>& visited)
{
	if (row < 0 || col < 0) return 0;
	if (visited[row][col] == true) return 0;
	visited[row][col] = true;
	int temp = 1;
	if (row > 0 ) temp += (_arrayCode[row][col] == _arrayCode[row - 1][col] ? neighborsExpandOut(row - 1, col, visited) : 0);
	if (col > 0) temp += (_arrayCode[row][col] == _arrayCode[row][col - 1] ? neighborsExpandOut(row, col - 1, visited) : 0);
	if (row < _arrayRow - 1) temp += (_arrayCode[row][col] == _arrayCode[row + 1][col] ? neighborsExpandOut(row + 1, col, visited) : 0);
	if (col < _arrayCol - 1) temp += (_arrayCode[row][col] == _arrayCode[row][col + 1] ? neighborsExpandOut(row, col + 1, visited) : 0);
	return temp;
}


void MArrayCheck::rectangleArea(const vector<int>& heights, int begin , int end, vector<int>& area)  //单调栈
{
	//stack<int> sta;
	//sta.push(begin - 1);
	//for (int i = begin; i <= end; ++i) {
	//	while (sta.top() >= begin && heights[sta.top()] >= heights[i]) {
	//		int height = heights[sta.top()];
	//		int cur = sta.top();
	//		sta.pop();
	//		int width = i - sta.top() - 1;
	//		while (cur > sta.top()) {
	//			area[cur] = max(area[cur], height * width);
	//			cur--;
	//		}
	//	}
	//	sta.push(i);
	//}

	//while (sta.top() >= begin) {
	//	int height = heights[sta.top()];
	//	int cur = sta.top();
	//	sta.pop();
	//	int width = end - sta.top();
	//	while (cur > sta.top()) {
	//		area[cur] = max(area[cur], height * width);
	//		cur--;
	//	}
	//}
	//return;

	//允许单列长连续符号
	stack<int> sta;
	sta.push(begin -1);
	for (int i = begin; i <= end; ++i){
		while (sta.top() >= begin && heights[sta.top()] >= heights[i]) {
			int height = heights[sta.top()];
			int cur = sta.top();
			sta.pop();
			int width = i - sta.top() - 1;
			int temp = height * width;
			if (height == 1 || width == 1) temp = min(temp, _winRow);
			while (cur > sta.top())	{
				area[cur] = max(area[cur], temp);
				cur--;
			}
		}
		sta.push(i);
	}

	while (sta.top() >= begin){
		int height = heights[sta.top()];
		int cur = sta.top();
		sta.pop();
		int width = end - sta.top() ;
		int temp = height * width;
		if (height == 1 || width == 1) temp = min(temp, _winRow);
		while (cur > sta.top()){
			area[cur] = max(area[cur], temp);
			cur--;
		}
	}
	return;
}
//热力图
template<typename T>
Mat MArrayCheck::imageColorMap(const vector<vector<T>>& inputArr, checkItem item)
{
	Mat temp;
	switch (item)
	{
	case MArrayCheck::symbolsPercentage:
	{
		temp = imageShow(inputArr, (double)255 / (double)_symbolsPercentage_max);
		break;
	}
	case MArrayCheck::neighborsRectangle:
	{
		temp = imageShow(inputArr, (double)255 / (double)_neighborsRectangle_max);
		break;
	}
	case MArrayCheck::neighborsConnected:
	{
		temp = imageShow(inputArr, (double)255 / (double)_neighborsConnected_max);
		break;
	}
	case MArrayCheck::edgesPercentage:
	{
		temp = imageShow(inputArr, (double)255 / (double)_edgesPercentage_max);
		break;
	}
	case MArrayCheck::neighborsHanming:
	{
		temp = imageShow(inputArr, (double)255 / (double)_neighborsHanming_max);
		break;
	}
	default:
		break;
	}
	applyColorMap(temp, temp, cv::COLORMAP_JET);   //0-12
	return temp;
}

void MArrayCheck::checkRun()
{
	winPropertyCal();

	symbolsPercentageCal();
	neighborsRectangleCal();
	neighborsConnectedCal();
	edgesPercentageCal();
	neighborsHanmingCal();
}

//输出显示
void MArrayCheck::printInfo()
{
	cout << endl;
	cout << ends << "M-array size : " << _arrayRow << " x " << _arrayCol << endl;
	cout << ends << "sub-window size : " << _winRow << " x " << _winCol << endl;
	cout << ends << "symbols : " << _symbols << endl;
	cout << ends << "sub-window totals : " << _winSum << endl;

	cout << endl;
	cout << ends << "the window-property is : " << _winProperty << endl;
	cout << ends << "ratio :  Row / Col : " << _RC_ratio << endl;
	cout << ends << "ratio :  Col / Row : " << _CR_ratio << endl;
	cout << ends << "elements balance (max - min)/(max + min) : " << _symGlbBalance << endl;
	cout << ends << "Average hanming distance : " << _hanmingAverage << endl;
	cout << ends << "Min hanming distance : " << _hanmingMinum << endl;

	_imageDemo = imageShow(this->_arrayCode);

	_symbolsPerImg = imageShow(this->_symbolsPercentage);
	_neighborsConnectedImg = imageShow(this->_neighborsConnected_double);
	_neighborsRectangleImg = imageShow(this->_neighborsRectangle_double);
	_edgesPercentageImg = imageShow(this->_edgesPercentage);
	_neighborsHanmingImg = imageShow(this->_neighborsHanming_double);

	_symbolsPerImg_hot = imageColorMap(this->_symbolsPercentage, symbolsPercentage);
	_neighborsConnectedImg_hot = imageColorMap(this->_neighborsConnected_double, neighborsConnected);
	_neighborsRectangleImg_hot = imageColorMap(this->_neighborsRectangle_double, neighborsRectangle);
	_edgesPercentageImg_hot = imageColorMap(this->_edgesPercentage, edgesPercentage);
	_neighborsHanmingImg_hot = imageColorMap(this->_neighborsHanming_double, neighborsHanming);

	paraFileSave();

	return;
}


void MArrayCheck::convKernelInit()
{
	int shift = 1;
	for (int i = 0; i < _winRow; ++i)
	{
		for (int j = 0; j < _winCol; ++j)
		{
			_convKernel[i][j] = shift;
			shift *= _symbols;
		}
	}
}

int MArrayCheck::convolution(int rowInd, int colInd)
{
	int temp = 0;
	for (int i = 0; i < _winRow; ++i){
		for (int j = 0; j < _winCol; ++j){
			temp += (_arrayCode[rowInd + i][colInd + j] * _convKernel[i][j]);
		}
	}
	return temp;
}


