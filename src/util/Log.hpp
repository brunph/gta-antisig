#pragma once

namespace util
{
	template <typename... Args>
	void log(const char* fmt, Args... args)
	{
		char buf[500]{};

		auto fmtLen = sprintf(buf, fmt, args...);

		if (fmtLen <= 0) throw std::exception("failed to format log");

		OutputDebugStringA(buf);
	}
}