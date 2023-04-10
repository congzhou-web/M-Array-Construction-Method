#include "PublicFun.h"
#include "MArrayCheck.h"
#include "NoneZeroMaps.h"
#include "ImprovedMethod.h"
#include "PiecesGrowing.h"

int main()
{
	int symbol = 2;
	int winRow = 3;
	int winCol = 3;

	//PiecesGrowing growing(symbol, winRow, winCol);
	//if (!growing.algorithmRun()) {
	//	cout << " growing error !";
	//	system("pause");
	//}

	//growing.MArrayFileSave();
	//MArrayCheck growing_check(growing._MArray , growing._array_row, growing._array_col , growing._win_row, growing._win_col, growing._win_symbols);
	//growing_check.checkRun();

	//growing_check.printInfo();

	ImprovedMethod imprv(symbol, winRow, winCol);
	if (!imprv.algorithmRun()) {
		cout << " imprv error !";
		system("pause");
	}

	imprv.MArrayFileSave();
	MArrayCheck imprv_check(imprv._MArray , imprv._array_row, imprv._array_col , imprv._win_row, imprv._win_col, imprv._win_symbols);
	imprv_check.checkRun();



	//NoneZeroMaps maps(symbol, winRow, winCol);
	//maps.algorithmRun();
	//maps.MArrayFileSave();
	//MArrayCheck maps_check(maps._MArray, maps._array_row, maps._array_col, maps._win_row, maps._win_col, maps._win_symbols);
	//maps_check.checkRun();


	////¹éÒ»»¯
	//maps_check._symbolsPercentage_max = imprv_check._symbolsPercentage_max = max(imprv_check._symbolsPercentage_max, maps_check._symbolsPercentage_max);
	//maps_check._neighborsRectangle_max = imprv_check._neighborsRectangle_max = max(imprv_check._neighborsRectangle_max, maps_check._neighborsRectangle_max);
	//maps_check._neighborsConnected_max = imprv_check._neighborsConnected_max = max(imprv_check._neighborsConnected_max, maps_check._neighborsConnected_max);
	//maps_check._edgesPercentage_max = imprv_check._edgesPercentage_max = max(imprv_check._edgesPercentage_max, maps_check._edgesPercentage_max);
	//maps_check._neighborsHanming_max = imprv_check._neighborsHanming_max = max(imprv_check._neighborsHanming_max, maps_check._neighborsHanming_max);


	//imprv_check.printInfo();
	//maps_check.printInfo();


	system("pause");
	return 0;
}