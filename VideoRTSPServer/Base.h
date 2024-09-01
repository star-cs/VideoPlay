#pragma once
#include <string>

#ifndef BYTE
	typedef unsigned char BYTE;
#endif

class MBuffer : public std::string
{
public:
	MBuffer(size_t size = 0) :std::string() {
		if (size > 0) {
			resize(size);
			memset(*this, 0, this->size());
		}
	}

	MBuffer(const char* str) {
		resize(strlen(str));
		memcpy((void*)c_str(), str, size());
	}

	MBuffer(void* buffer, size_t size) :std::string() {
		resize(size);
		memcpy((void*)c_str(), buffer, size);
	}

	~MBuffer() {
		std::string::~basic_string();
	}

	operator char* () const { return (char*)c_str(); }
	operator const char* () const { return c_str(); }
	operator BYTE* () const { return (BYTE*)c_str(); }
	operator void* () const { return (void*)c_str(); }

	MBuffer operator=(const char* str) {
		std::string::operator=(str);
		return *this;
	}

	void Updata(void* buffer, size_t size) {
		resize(size);
		memcpy((void*)c_str(), buffer, size);
	}

	void Zero() {
		memset((void*)c_str(), 0, size());
	}

	MBuffer& operator<<(const MBuffer& str) {
		if (this != str) {
			*this += str;
		}
		else {
			MBuffer tmp = str;
			*this += tmp;
		}
		return *this;
	}

	MBuffer& operator<<(const std::string& str) {
		*this += str;
		return *this;
	}

	MBuffer& operator<<(const char* str) {
		*this += str;
		return *this;
	}

	MBuffer& operator<<(int data) {
		char s[16] = "";
		snprintf(s, sizeof(s), "%d", data);
		*this += s;
		return *this;
	}

	const MBuffer& operator>>(int& data) const {
		data = atoi(c_str());
		return *this;
	}

	const MBuffer& operator>>(short& data) const {
		data = (short)atoi(c_str());
		return *this;
	}
};