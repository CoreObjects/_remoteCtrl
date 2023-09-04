// 客户端.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "Model.h"

int main() {
	system("pause");
	CModel m;
	m.downLoadFile("C:\\Users\\89598\\source\\repos\\网络编程库\\客户端\\1.jpg", \
		"C:\\Users\\89598\\source\\repos\\网络编程库\\客户端\\10.jpg");
	system("pause");
	std::cout << "Hello World!\n";

}
