#pragma once
#include <vector>
#include <cstdarg>
#include <iostream>
class MyString {
private:
	char* m_data; // 存储字符串的指针
	size_t m_len; // 字符串的长度

public:
	// 默认构造函数
	MyString();
	// 带参构造函数
	MyString(const char* s);
	// 拷贝构造函数
	MyString(const MyString& other);
	// 析构函数
	~MyString();
	// 重载赋值运算符
	MyString& operator=(const MyString& other);
	MyString& operator=(const char* str);
	// 重载加法运算符
	MyString operator+(const MyString& other) const;
	MyString operator+(const char* str) const;
	MyString& operator+=(const char* str);
	MyString& operator+=(const MyString& other);
	// 比较运算符
	bool operator==(const char* str) const;
	bool operator==(const MyString& other) const;
	bool operator!=(const char* str) const;
	bool operator!=(const MyString& other) const;
	bool operator<(const char* s) const;
	bool operator<(const MyString& other) const;
	bool operator>(const char* s) const;
	bool operator>(const MyString& other) const;
	bool operator<=(const MyString& other) const;
	bool operator<=(const char* s) const;
	bool operator>=(const char* s) const;
	bool operator>=(const MyString& other) const;
	// 重载下标运算符
	char& operator[](size_t index) const;
	// 查找字符串
	size_t find(const char* str, size_t pos) const;
	size_t find_KMP(const char* str, size_t pos) const;
	//切割字符串
	MyString substring(size_t pos, size_t len) const;
	std::vector<MyString> split(const char* delimiter) const;
	// 转换为数字
	int toInt() const;
	long long toLongLong()const;
	double toDouble() const;
	float toFloat()const;
	// 返回字符串长度
	size_t length() const;
	// 返回字符串是否为空
	bool empty() const;
	// 返回C风格字符串指针
	const char* c_str() const;
	// 清空字符串
	void clear();
	// 大小写转换
	void toUpper();
	void toLower();
	// 字符串反转
	void reverse();
	// 字符串填充
	void padLeft(size_t length, char ch = ' ');
	void padRight(size_t length, char ch = ' ');
	//字符串替换
	MyString& replace(const MyString& old_str, const MyString& new_str);
	//加密解密
	void xorEncrypt(char key);
	void xorDecrypt(char key);
	//添加字符
	void pushback(char c);
	//格式化字符串
	void format(const char* formatString, ...);
public:
	static MyString fromInt(int value);
	static MyString fromDouble(double value, int precision = 4);
	friend std::ostream& operator<<(std::ostream& os, const MyString& str) {
		os << str.m_data;
		return os;
	}
	friend std::istream& operator>>(std::istream& is, MyString& str) {
		char c;
		str = MyString(); // 先将 MyString 对象初始化为空字符串
		while (is.get(c) && !std::isspace(c)) {
			str.pushback(c);
		}
		return is;
	}
	friend std::istream& getline(std::istream& is, MyString& str, const char delim = '\n') {
		char c;
		str = MyString(); // 先将 MyString 对象初始化为空字符串
		while (is.get(c) && c != delim) {
			str.pushback(c);
		}
		return is;
	}

private:
	char* my_strcpy(char* dest, const char* src)const;
	size_t my_strlen(const char* str)const;
	char* my_strcat(char* dest, const char* src)const;
	int my_strcmp(const char* str1, const char* str2)const;
	int my_strncmp(const char* s1, const char* s2, size_t n)const;
	void get_next(const char* str, size_t* next, size_t m) const;
	void* my_memcpy(void* dest, const void* src, size_t n)const;
	bool my_isdigit(char c)const;
	void formatHelper(MyString& ss, const char* formatString, va_list args);
};

