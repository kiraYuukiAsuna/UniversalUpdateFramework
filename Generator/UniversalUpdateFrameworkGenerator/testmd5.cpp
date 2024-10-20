#include <iostream>

#include "md5.h"

void PrintMD5(const string& str, MD5& md5) {
	std::cout << "MD5(\"" << str << "\") = " << md5.toString() << std::endl;
}

int testmain() {
	MD5 md5;
	md5.update("");
	PrintMD5("", md5);

	md5.update("a");
	PrintMD5("a", md5);

	md5.update("bc");
	PrintMD5("abc", md5);

	md5.update("defghijklmnopqrstuvwxyz");
	PrintMD5("abcdefghijklmnopqrstuvwxyz", md5);

	md5.reset();
	md5.update("message digest");
	PrintMD5("message digest", md5);

	md5.reset();
	auto a = ifstream("D:\\test.txt");
	md5.update(a);
	PrintMD5("D:\\test.txt", md5);

	// 打开二进制文件（例如常见的二进制文件  .so,.dll,.exe,.bin ）
	ifstream in(R"(C:\Users\KiraY\Desktop\cxxopts-3.1.1.zip)",
				std::ios::binary);
	if (!in) {
		return 0;
	}

	md5.reset();
	md5.update(in);
	std::string str = md5.toString();
	PrintMD5(R"(C:\Users\KiraY\Desktop\cxxopts-3.1.1.zip)", md5);

	return 0;
}
