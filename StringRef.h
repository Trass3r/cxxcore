#pragma once
#include <string>

static constexpr size_t utf8strlen(const char* str)
{
	int i = 0;
	int j = 0;
	while (str[i++])
		if ((s[i] & 0xC0) != 0x80)
			++j;

	return j;
}

struct StringRef
{
	typedef const char* iterator;
	typedef const char* const_iterator;
	static const size_t npos = ~size_t(0);
	typedef size_t size_type;

private:
	const char* data;
	size_t length;

public:
	constexpr StringRef() : data(nullptr), length(0) {}
	constexpr StringRef(const char* str) : data(str)
	{
		length = utf8strlen(str);
	}
	constexpr StringRef(const char* str, size_t len) : data(str), length(len) {}
	constexpr StringRef(const std::string& str) : data(str.c_str()), length(str.length()) {}

	operator std::string() const
	{
		return std::string(data, length);
	}

	iterator begin() const { return data; }
	iterator end()   const { return data + length; }

	const char* ptr() const { return data; }
	bool empty()      const { return length == 0; }
	size_t size()     const { return length; }

	// get first character
	char32_t front() const
	{
		assert(!empty());
		return data[0];    // TODO
	}

	// get first character
	char32_t back() const
	{
		assert(!empty());
		return data[length - 1];    // TODO
	}

	char operator[](size_t idx) const
	{
		assert(idx < length && "Invalid index!");
		return Data[idx];
	}
};
