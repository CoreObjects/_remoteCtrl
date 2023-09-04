#include "MyString.h"
#include <stdexcept>
MyString::MyString() {
	m_data = new char[1];
	m_data[0] = '\0';
	m_len = 0;
}

MyString::MyString(const char* s) {
	m_len = my_strlen(s);
	m_data = new char[m_len + 1];
	my_strcpy(m_data, s);
}

MyString::MyString(const MyString& other) {
	m_len = other.m_len;
	m_data = new char[m_len + 1];
	my_strcpy(m_data, other.m_data);
}

MyString::~MyString() {
	delete[] m_data;
}

MyString& MyString::operator=(const char* str) {
	if (m_data != str) {
		// 释放原有的内存
		delete[] m_data;
		m_data = nullptr;
		m_len = 0;
		// 分配新的内存
		if (str) {
			m_len = my_strlen(str);
			m_data = new char[m_len + 1];
			my_strcpy(m_data, str);
		}
	}
	return *this;
}
MyString MyString::operator+(const char* str) const {
	MyString newString;
	newString.m_len = m_len + my_strlen(str);
	newString.m_data = new char[newString.m_len + 1];
	my_strcpy(newString.m_data, m_data);
	my_strcat(newString.m_data, str);
	return newString;
}


MyString MyString::operator+(const MyString& other) const {
	MyString newString;
	newString.m_len = m_len + other.m_len;
	newString.m_data = new char[newString.m_len + 1];
	my_strcpy(newString.m_data, m_data);
	my_strcat(newString.m_data, other.m_data);
	return newString;
}

MyString& MyString::operator+=(const char* str) {
	// 计算新的长度
	size_t newLen = m_len + strlen(str);
	// 分配新的内存
	char* newData = new char[newLen + 1];
	// 复制原有的字符串
	my_strcpy(newData, m_data);
	// 添加新的字符串
	my_strcat(newData, str);
	// 释放原有的内存并更新成员变量
	delete[] m_data;
	m_data = newData;
	m_len = newLen;
	// 返回当前对象的引用，以支持链式赋值
	return *this;
}

MyString& MyString::operator+=(const MyString& other) {
	// 调用之前实现的重载版本，将 other 对象的字符串追加到当前对象的末尾
	*this += other.m_data;

	// 返回当前对象的引用，以支持链式追加操作
	return *this;
}
bool MyString::operator==(const char* str) const {
	return my_strcmp(m_data, str) == 0;
}
bool MyString::operator==(const MyString& other) const {
	return my_strcmp(m_data, other.m_data) == 0;
}
bool MyString::operator!=(const char* str) const {
	return my_strcmp(m_data, str) != 0;
}
bool MyString::operator!=(const MyString& other) const {
	return my_strcmp(m_data, other.m_data) != 0;
}
bool MyString::operator<(const char* s) const {
	return my_strcmp(m_data, s) < 0;
}
bool MyString::operator<(const MyString& other) const {
	return my_strcmp(m_data, other.m_data) < 0;
}
bool MyString::operator>(const char* s) const {
	return my_strcmp(m_data, s) > 0;
}
bool MyString::operator>(const MyString& other) const {
	return my_strcmp(m_data, other.m_data) > 0;
}
bool MyString::operator<=(const char* s) const {
	return my_strcmp(m_data, s) <= 0;
}
bool MyString::operator<=(const MyString& other) const {
	return my_strcmp(m_data, other.m_data) <= 0;
}
bool MyString::operator>=(const char* s) const {
	return my_strcmp(m_data, s) >= 0;
}
bool MyString::operator>=(const MyString& other) const {
	return my_strcmp(m_data, other.m_data) >= 0;
}
char& MyString::operator[](size_t index) const {
	if (index < 0 || index >= m_len) {
		throw std::out_of_range("Index out of range");
	}
	return m_data[index];
}

size_t MyString::find(const char* str, size_t pos) const {
	size_t len = my_strlen(str);
	if (pos + len > m_len) {
		return -1;
	}
	for (size_t i = pos; i <= m_len - len; i++) {
		if (my_strncmp(m_data + i, str, len) == 0) {
			return i;
		}
	}
	return -1;
}

size_t MyString::find_KMP(const char* str, size_t pos) const {
	size_t n = m_len;
	size_t m = strlen(str);
	if (m == 0) {
		return pos;
	}
	if (m > n) {
		return -1;
	}
	size_t* next = new size_t[m];
	get_next(str, next, m);
	size_t j = 0;
	for (size_t i = pos; i < n; ++i) {
		while (j > 0 && str[j] != m_data[i]) {
			j = next[j - 1];
		}
		if (str[j] == m_data[i]) {
			++j;
		}
		if (j == m) {
			delete[] next;
			return i - m + 1;
		}
	}
	delete[] next;
	return -1;
}

MyString MyString::substring(size_t pos, size_t len) const {
	if (pos >= m_len) {
		throw std::out_of_range("substring position out of range");
	}
	MyString newString;
	newString.m_len = (len == -1) ? (m_len - pos) : len;
	newString.m_data = new char[newString.m_len + 1];
	std::memcpy(newString.m_data, m_data + pos, newString.m_len);
	newString.m_data[newString.m_len] = '\0';
	return newString;
}


std::vector<MyString> MyString::split(const char* delimiter) const {
	std::vector<MyString> result;
	size_t len = my_strlen(delimiter);
	size_t pos = 0;
	while (pos < m_len) {
		size_t next_pos = find(delimiter, pos);
		if (next_pos == -1) {
			next_pos = m_len;
		}
		result.push_back(substring(pos, next_pos - pos));
		pos = next_pos + len;
	}
	return result;
}

int MyString::toInt() const {
	int result = 0;
	int sign = 1;
	const char* p = m_data;
	// 判断正负号
	if (*p == '-') {
		sign = -1;
		p++;
	}
	else if (*p == '+') {
		p++;
	}
	// 转换数字
	while (*p >= '0' && *p <= '9') {
		result = result * 10 + (*p - '0');
		p++;
	}
	// 判断是否包含非数字字符
	if (*p != '\0') {
		// TODO: 处理非数字字符
	}
	return result * sign;
}

long long MyString::toLongLong() const {
	const char* ptr = this->c_str();
	long long result = 0;
	bool isNegative = false;
	if (*ptr == '-') {
		isNegative = true;
		++ptr;
	}
	while (my_isdigit(*ptr)) {
		result = result * 10 + (*ptr - '0');
		++ptr;
	}
	if (*ptr != '\0') {
		// conversion failed
		throw std::invalid_argument("invalid argument");
	}
	if (isNegative) {
		result = -result;
	}
	return result;
}

double MyString::toDouble() const {
	double result = 0.0;
	bool isNegative = false;
	bool hasDecimalPoint = false;
	int decimalPlaces = 0;
	size_t i = 0;

	// Check for negative sign
	if (m_data[i] == '-') {
		isNegative = true;
		i++;
	}

	// Parse integer part of string
	for (; i < m_len; i++) {
		if (my_isdigit(m_data[i])) {
			result = result * 10.0 + (m_data[i] - '0');
		}
		else if (m_data[i] == '.') {
			hasDecimalPoint = true;
			break;
		}
		else {
			// Invalid character
			return 0.0;
		}
	}

	// Parse fractional part of string
	if (hasDecimalPoint) {
		i++;
		for (; i < m_len; i++) {
			if (my_isdigit(m_data[i])) {
				result = result * 10.0 + (m_data[i] - '0');
				decimalPlaces++;
			}
			else {
				// Invalid character
				return 0.0;
			}
		}
	}

	// Apply sign and decimal point position
	if (isNegative) {
		result = -result;
	}
	if (hasDecimalPoint) {
		result /= pow(10.0, decimalPlaces);
	}

	return result;
}
float MyString::toFloat() const {
	int integerPart = 0;
	float decimalPart = 0.0f;
	bool isNegative = false;
	bool isDecimal = false;
	int decimalPlaces = 0;
	size_t index = 0;
	// 处理负号
	if (m_data[index] == '-') {
		isNegative = true;
		index++;
	}
	// 处理整数部分
	while (index < m_len && my_isdigit(m_data[index])) {
		integerPart = integerPart * 10 + (m_data[index] - '0');
		index++;
	}
	// 处理小数点及其后的数字
	if (index < m_len && m_data[index] == '.') {
		isDecimal = true;
		index++;

		while (index < m_len && my_isdigit(m_data[index])) {
			decimalPart = decimalPart * 10 + (m_data[index] - '0');
			decimalPlaces++;
			index++;
		}
	}
	// 计算最终的浮点数
	float result = (float)(static_cast<float>(integerPart) + decimalPart / pow(10, decimalPlaces));
	// 处理负数
	if (isNegative) {
		result = -result;
	}
	return result;
}
void MyString::padLeft(size_t length, char ch) {
	if (length <= m_len) return; // 如果指定的长度小于等于当前字符串长度，则不进行填充

	int pad_len = length - m_len;
	char* pad_str = new char[pad_len + 1]; // 创建一个新的填充字符串
	for (int i = 0; i < pad_len; i++) {
		pad_str[i] = ch;
	}
	pad_str[pad_len] = '\0';

	char* new_str = new char[length + 1];
	my_strcpy(new_str, pad_str); // 先拼接填充字符串
	my_strcat(new_str, m_data); // 再拼接原字符串

	delete[] pad_str;
	delete[] m_data;
	m_data = new_str;
	m_len = length;
}
void MyString::padRight(size_t length, char ch) {
	if (length <= m_len) return; // 如果指定的长度小于等于当前字符串长度，则不进行填充

	int pad_len = length - m_len;
	char* pad_str = new char[pad_len + 1]; // 创建一个新的填充字符串
	for (int i = 0; i < pad_len; i++) {
		pad_str[i] = ch;
	}
	pad_str[pad_len] = '\0';

	char* new_str = new char[length + 1];
	my_strcpy(new_str, m_data); // 先拼接原字符串
	my_strcat(new_str, pad_str); // 再拼接填充字符串

	delete[] pad_str;
	delete[] m_data;
	m_data = new_str;
	m_len = length;
}

MyString& MyString::replace(const MyString& old_str, const MyString& new_str) {
	if (old_str.empty()) {
		// 如果要替换的字符串为空，则直接返回
		return *this;
	}
	size_t pos = 0;
	while ((pos = find(old_str.c_str(), pos)) != -1) {
		// 找到 old_str 的位置，替换为 new_str
		m_data[pos] = '\0'; // 把原字符串的匹配位置截断
		size_t new_size = m_len - old_str.m_len + new_str.m_len; // 新字符串的长度
		char* new_data = new char[new_size + 1]; // 新字符串的缓冲区
		my_memcpy(new_data, m_data, pos); // 拷贝匹配位置前面的部分
		my_memcpy(new_data + pos, new_str.c_str(), new_str.m_len); // 拷贝新字符串
		my_memcpy(new_data + pos + new_str.m_len, m_data + pos + old_str.m_len, m_len - pos - old_str.m_len); // 拷贝匹配位置后面的部分
		new_data[new_size] = '\0'; // 新字符串的末尾添加空字符
		delete[] m_data; // 释放原字符串的缓冲区
		m_data = new_data; // 使用新字符串的缓冲区
		m_len = new_size; // 更新字符串的长度
		pos += new_str.m_len; // 从新字符串的位置开始继续查找
	}
	return *this;
}

size_t MyString::length() const {
	return m_len;
}

bool MyString::empty() const {
	return m_len == 0;
}

const char* MyString::c_str() const {
	return m_data;
}
void MyString::clear() {
	delete[] m_data;
	m_data = new char[1];
	m_data[0] = '\0';
	m_len = 0;
}

void MyString::toUpper() {
	for (size_t i = 0; i < m_len; i++) {
		if (m_data[i] >= 'a' && m_data[i] <= 'z') {
			m_data[i] -= 32; // 转换为大写字母
		}
	}
}

void MyString::toLower() {
	for (size_t i = 0; i < m_len; i++) {
		if (m_data[i] >= 'A' && m_data[i] <= 'Z') {
			m_data[i] += 32; // 转换为小写字母
		}
	}
}

void MyString::reverse() {
	int len = m_len;
	for (int i = 0; i < len / 2; i++) {
		char temp = m_data[i];
		m_data[i] = m_data[len - i - 1];
		m_data[len - i - 1] = temp;
	}
}

char* MyString::my_strcpy(char* dest, const char* src)const {
	char* ret = dest;
	while (*src) {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0'; // 添加字符串结束符
	return ret;
}

size_t MyString::my_strlen(const char* str)const {
	size_t length = 0;
	while (*str != '\0') {
		length++;
		str++;
	}
	return length;
}

char* MyString::my_strcat(char* dest, const char* src)const {
	char* ret = dest;
	while (*dest) dest++; // 找到目标字符串的末尾
	while (*src) {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0'; // 添加字符串结束符
	return ret;
}

int MyString::my_strcmp(const char* str1, const char* str2) const {
	while (*str1 && *str2 && *str1 == *str2) {
		++str1;
		++str2;
	}
	return *str1 - *str2;
}

int MyString::my_strncmp(const char* s1, const char* s2, size_t n) const {
	for (size_t i = 0; i < n; i++) {
		if (s1[i] != s2[i]) {
			return s1[i] - s2[i];
		}
		if (s1[i] == '\0') { // s1已经结束
			return 0;
		}
	}
	return 0;
}

void MyString::get_next(const char* str, size_t* next, size_t m) const {
	next[0] = 0;
	size_t j = 0;
	for (size_t i = 1; i < m; ++i) {
		while (j > 0 && str[i] != str[j]) {
			j = next[j - 1];
		}
		if (str[i] == str[j]) {
			++j;
		}
		next[i] = j;
	}
}

void* MyString::my_memcpy(void* dest, const void* src, size_t n) const {
	char* d = (char*)dest;
	const char* s = (const char*)src;
	while (n--) {
		*d++ = *s++;
	}
	return dest;
}

bool MyString::my_isdigit(char c) const {
	return (c >= '0' && c <= '9');
}


void MyString::formatHelper(MyString& ss, const char* formatString, va_list args) {
	for (size_t i = 0; formatString[i] != '\0'; ++i) {
		if (formatString[i] == '%' && formatString[i + 1] != '\0') {
			if (formatString[i + 1] == 'd') {
				int value = va_arg(args, int);
				ss += fromInt(value);
			}
			else if (formatString[i + 1] == 'f') {
				double value = va_arg(args, double);
				ss +=fromDouble(value);
			}
			else if (formatString[i + 1] == 's') {
				const char* value = va_arg(args, const char*);
				ss += value;
			}
			else {
				// handle other formats as needed
			}
			i += 1;
		}
		else {
			ss.pushback(formatString[i]);
		}
	}
}

MyString& MyString::operator=(const MyString& other) {
	if (this == &other) return *this;
	delete[] m_data;
	m_len = other.m_len;
	m_data = new char[m_len + 1];
	my_strcpy(m_data, other.m_data);
	return *this;
}

void MyString::xorEncrypt(char key) {
	for (size_t i = 0; i < m_len; ++i) {
		m_data[i] ^= key;
	}
}

void MyString::xorDecrypt(char key) {
	for (size_t i = 0; i < m_len; ++i) {
		m_data[i] ^= key;
	}
}

void MyString::pushback(char c) {
	char* new_str = new char[m_len + 2];
	my_strcpy(new_str, m_data);
	delete[] m_data;
	m_data = new_str;
	m_data[m_len] = c;
	m_data[++m_len] = '\0';
}

void MyString::format(const char* formatString, ...) {
	va_list args;
	va_start(args, formatString);
	MyString ss;
	formatHelper(ss, formatString, args);
	*this = ss;
	va_end(args);
}

MyString MyString::fromInt(int value) {
	char buffer[20]; // 可以容纳最长的int长度和负号
	int pos = 0;
	if (value < 0) {
		buffer[pos++] = '-';
		value = -value;
	}
	do {
		buffer[pos++] = static_cast<char>('0' + value % 10);
		value /= 10;
	} while (value > 0);

	buffer[pos] = '\0';

	// 反转字符串
	for (int i = 0; i < pos / 2; ++i) {
		char temp = buffer[i];
		buffer[i] = buffer[pos - i - 1];
		buffer[pos - i - 1] = temp;
	}
	return MyString(buffer);
}

MyString MyString::fromDouble(double value, int precision /*= 2*/) {
	char buffer[50]; // 可以容纳最长的double长度和小数点

	if (value < 0) {
		value = -value;
		buffer[0] = '-';
		buffer[1] = '\0';
	}
	else {
		buffer[0] = '\0';
	}

	// 转换整数部分
	int intPart = static_cast<int>(value);
	int pos = 0;
	do {
		buffer[pos++] = static_cast<char>('0' + intPart % 10);
		intPart /= 10;
	} while (intPart > 0);

	// 反转整数部分字符串
	for (int i = 0; i < pos / 2; ++i) {
		char temp = buffer[i];
		buffer[i] = buffer[pos - i - 1];
		buffer[pos - i - 1] = temp;
	}

	// 添加小数点和小数部分
	if (precision > 0) {
		buffer[pos++] = '.';
		value -= static_cast<int>(value); // 将整数部分去掉，只留下小数部分

		for (int i = 0; i < precision; ++i) {
			value *= 10;
			int digit = static_cast<int>(value);
			buffer[pos++] = static_cast<char>('0' + digit);
			value -= digit;
		}
	}

	buffer[pos] = '\0';

	return MyString(buffer);
}
