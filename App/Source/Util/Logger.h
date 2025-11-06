#pragma once

#include <format>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string_view>

constexpr auto LOG_AQUA   = "\033[36m";
constexpr auto LOG_YELLOW = "\033[33m";
constexpr auto LOG_RED    = "\033[91m";
constexpr auto LOG_GREEN  = "\033[92m";
constexpr auto LOG_RESET  = "\033[0m";

namespace Logger
{
	inline std::string GetTimestamp()
	{
		using namespace std::chrono;
		auto now = system_clock::now();
		std::time_t now_c = system_clock::to_time_t(now);
		std::tm tm;
		localtime_s(&tm, &now_c);

		return std::format("[{:02d}:{:02d}:{:02d}]", tm.tm_hour, tm.tm_min, tm.tm_sec);
	}

	inline void Print(std::string_view color, std::string_view level, std::string_view message)
	{
		std::cout << color
			<< GetTimestamp() << " [" << level << "] "
			<< message
			<< LOG_RESET << '\n';
	}

	template<typename... Args>
	inline void Log(std::string_view fmt, Args&&... args)
	{
		Print(LOG_AQUA, "LOG", std::vformat(fmt, std::make_format_args(args...)));
	}

	template<typename... Args>
	inline void Warning(std::string_view fmt, Args&&... args)
	{
		Print(LOG_YELLOW, "WRN", std::vformat(fmt, std::make_format_args(args...)));
	}

	template<typename... Args>
	inline void Error(std::string_view fmt, Args&&... args)
	{
		Print(LOG_RED, "ERR", std::vformat(fmt, std::make_format_args(args...)));
	}

	template<typename... Args>
	inline void Success(std::string_view fmt, Args&&... args)
	{
		Print(LOG_GREEN, "SCS", std::vformat(fmt, std::make_format_args(args...)));
	}
}