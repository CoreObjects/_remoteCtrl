#pragma once
#include <vector>
#include <cstdarg>
#include <iostream>
class MyString {
private:
	char* m_data; // �洢�ַ�����ָ��
	size_t m_len; // �ַ����ĳ���

public:
	// Ĭ�Ϲ��캯��
	MyString();
	// ���ι��캯��
	MyString(const char* s);
	// �������캯��
	MyString(const MyString& other);
	// ��������
	~MyString();
	// ���ظ�ֵ�����
	MyString& operator=(const MyString& other);
	MyString& operator=(const char* str);
	// ���ؼӷ������
	MyString operator+(const MyString& other) const;
	MyString operator+(const char* str) const;
	MyString& operator+=(const char* str);
	MyString& operator+=(const MyString& other);
	// �Ƚ������
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
	// �����±������
	char& operator[](size_t index) const;
	// �����ַ���
	size_t find(const char* str, size_t pos) const;
	size_t find_KMP(const char* str, size_t pos) const;
	//�и��ַ���
	MyString substring(size_t pos, size_t len) const;
	std::vector<MyString> split(const char* delimiter) const;
	// ת��Ϊ����
	int toInt() const;
	long long toLongLong()const;
	double toDouble() const;
	float toFloat()const;
	// �����ַ�������
	size_t length() const;
	// �����ַ����Ƿ�Ϊ��
	bool empty() const;
	// ����C����ַ���ָ��
	const char* c_str() const;
	// ����ַ���
	void clear();
	// ��Сдת��
	void toUpper();
	void toLower();
	// �ַ�����ת
	void reverse();
	// �ַ������
	void padLeft(size_t length, char ch = ' ');
	void padRight(size_t length, char ch = ' ');
	//�ַ����滻
	MyString& replace(const MyString& old_str, const MyString& new_str);
	//���ܽ���
	void xorEncrypt(char key);
	void xorDecrypt(char key);
	//����ַ�
	void pushback(char c);
	//��ʽ���ַ���
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
		str = MyString(); // �Ƚ� MyString �����ʼ��Ϊ���ַ���
		while (is.get(c) && !std::isspace(c)) {
			str.pushback(c);
		}
		return is;
	}
	friend std::istream& getline(std::istream& is, MyString& str, const char delim = '\n') {
		char c;
		str = MyString(); // �Ƚ� MyString �����ʼ��Ϊ���ַ���
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

