#pragma once

// credit: https://www.unknowncheats.me/forum/3135334-post10.html

#pragma once

constexpr int charToInt(const char& ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';

	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;

	return ch - 'a' + 10;
};

template <typename T, T F = 16>
constexpr auto concatenateHex(const T& a, const T& b)
{
	return T(F) * a + b;
}

template <size_t N>
struct StrToBa
{
	std::array<char, N> str{};

	constexpr StrToBa(const char* a) noexcept
	{
		for (size_t i = 0u; i < N; ++i)
		{
			str[i] = a[i];
		}
	}

	constexpr size_t getSkipCount() const
	{
		size_t skips = 0;
		bool skipNextChar = false;

		for (size_t i = 0u; i < N; ++i)
		{
			// not delimiter
			if (str[i] != ' ')
			{
				// no need to do next checks if it's a skip wildcard
				if (str[i] == '?')
					continue;

				if (!skipNextChar)
				{
					// hit char pair
					if (i + 1 < N && str[i + 1] != ' ')
						skipNextChar = true;
				}
				// do only 1 skip for char pair
				else
				{
					++skips;
					skipNextChar = false;
				}

				continue;
			}

			++skips;
		}

		return skips;
	}

	static constexpr auto size()
	{
		return N;
	}
};

// only doing this part to decrease size of N by 1 cos of null char, otherwise would put this directly in constructor :(
template <size_t N>
StrToBa(const char(&)[N])->StrToBa<N - 1>;

template <StrToBa Str>
constexpr auto sToBa()
{
	std::array<int, Str.size() - Str.getSkipCount()> result{};

	size_t at = 0;
	bool skipNextChar = false;

	for (size_t i = 0u; i < Str.size(); ++i)
	{
		// delimiting wildcard
		if (Str.str[i] == ' ')
			continue;

		// skip wildcard
		if (Str.str[i] == '?')
		{
			result[at] = -1;
			++at;
			continue;
		}

		// skip next char
		if (skipNextChar)
		{
			skipNextChar = false;
			continue;
		}

		// need next char to check
		if (i + 1 < Str.size())
		{
			// set and increase
			result[at] = concatenateHex<int>(charToInt(Str.str[i]), charToInt(Str.str[i + 1]));
			++at;

			skipNextChar = true;
		}
	}

	return result;
}

template <auto V>
static constexpr auto forceCompileTime = V;

#define BYTE_ARRAY(str) forceCompileTime<sToBa<str>()>