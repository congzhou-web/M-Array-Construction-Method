#include "PublicFun.h"

using namespace std;
using namespace cv;

//从xml读取本原多项式信息
vector<int> xmlParse(int element, int step)
{
	vector<int> res(step, 0);
	string elementStr = "element_" + to_string(element);
	string stepStr = "step_" + to_string(step);

	TiXmlDocument doc;
	if (!doc.LoadFile("config.xml")){  //加载文档
		std::cerr << doc.ErrorDesc() << std::endl;
		exit(1);
	}	
	TiXmlElement* root = doc.FirstChildElement();   //获取根节点，也是xml文档的声明部分
	if (root == NULL){
		std::cerr << " Failed to load file: No root element." << std::endl;
		doc.Clear();
		exit(1);
	}

	TiXmlNode *ELE_node = NULL;      //查找element节点
	ELE_node = root->IterateChildren(elementStr.data(), ELE_node);
	TiXmlElement *ELE_elem = ELE_node->ToElement();

	TiXmlNode *step_node = NULL;     //查找step节点
	step_node = ELE_elem->IterateChildren(stepStr.data(), step_node);   

	string temp = step_node->ToElement()->GetText();
	if (temp.size() > step){
		cout <<ends << "config.xml file format error !!! " << endl;
		return vector<int> {};
	}
	for (int i = 0; i < temp.size(); ++i)
		res[i] = temp[i] - '0';
	return res;
}


bool SaveImgLocal(Mat image, string fileName, string folder)
{
	imwrite(folder + "\\" + fileName, image);
	return true;
}
