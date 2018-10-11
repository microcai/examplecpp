
#define SD_JOURNAL_SUPPRESS_LOCATION

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/uio.h>
#include <systemd/sd-journal.h>
#include <systemd/sd-daemon.h>
#else
#include <iostream>
#endif

//#include <boost/stacktrace.hpp>

#include <iostream>
#include <algorithm>
#include <boost/regex.hpp>
#include "beautylog/beautylog.hpp"

static bool conhost_is_vt_mode = false;

static void send_structured_message_lines_journald(const std::vector<std::string>& vector_field_string, const std::string message);
static void send_structured_message_lines_win_conhost_vtmode(const std::vector<std::string>& vector_field_string, const std::string message);
static void send_structured_message_lines_win_conhost(const std::vector<std::string>& vector_field_string, const std::string message);
static void send_structured_message_lines_stdout(const std::vector<std::string>& vector_field_string, const std::string message);
static beautylog::send_structured_message_lines_fn_t init_impl();

struct rgbmode
{
	int red, green, blue, bold;
};

static rgbmode color_cyc_buffer[] =
{
	{ 255, 0 , 0 },
	{ 255, 127, 0 },
	{ 255, 255, 0 },
	{ 0, 255, 0 },
	{ 0, 0, 255 },
	{ 75, 0, 130 },
	{ 148, 0, 211 },
};

namespace beautylog
{
	void print_backtrace(int priority)
	{
		if (priority < 3)
		{
			std::cerr << "---------begin backtrace ---------";
			std::cerr << "\x1b[1m\x1b[38;2;202;81;0m\n\n";
			//std::cerr << boost::stacktrace::stacktrace(4, static_cast<std::size_t>(-1)) << std::endl;
			std::cerr << "\x1b[1m\x1b[0m";
			std::cerr << "---------end backtrace ---------\n";
		}
	}

	sd_loger::sd_loger() = default;

	sd_loger::~sd_loger()
	{
		flush();
	}

	void sd_loger::flush()
	{
		std::unique_lock<std::mutex> l(m_mutex);
		auto log_entries_copyed = std::move(log_entries);

		l.unlock();

		for (auto e : log_entries_copyed)
		{
			send_structured_message(e);
		}
		log_entries.clear();
	}

	void sd_loger::disable_defer()
	{
		no_defer = true;
		flush();
	}

	void sd_loger::enable_defer()
	{
		no_defer = false;
	}


	void sd_loger::discardlog(int priority)
	{
		std::unique_lock<std::mutex> l(m_mutex);

		for (std::vector<log_entry>::iterator it = log_entries.begin(); it != log_entries.end();)
		{
			if (it->priority >= priority)
			{
				it = log_entries.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	void sd_loger::add_constant_field(std::string field_name, std::string field_value)
	{
		std::unique_lock<std::mutex> l(m_mutex);
		constant_fields.insert({field_name, field_value});
	}

	void sd_loger::set_constant_fields(const std::map<std::string, std::string>& cf)
	{
		std::unique_lock<std::mutex> l(m_mutex);
		constant_fields = cf;
	}

	void sd_loger::send_structured_message(const log_entry&e) const
	{
		std::unique_lock<std::mutex> l(m_mutex);

		std::vector<std::string> vector_string;

		vector_string.push_back("PRIORITY=" + std::to_string(e.priority));
		for (const auto& f : e.log_fields)
		{
			vector_string.push_back(f.first + "=" + f.second);
		}

		for (const auto& f : constant_fields)
		{
			vector_string.push_back(f.first + "=" + f.second);
		}

		send_structured_message_lines_impl()(vector_string, e.log_message);
	}

	std::string sd_loger::cpp_file_pretty(const char * file)
	{
#ifdef _WIN32
		auto p = strstr(file, u8R"_(flashpay\)_");
#else
		auto p = strstr(file, u8R"_(flashpay/)_");
#endif
		if (p)
		{
			return p + 9;
		}
		return file;
	}

	send_structured_message_lines_fn_t send_structured_message_lines_impl()
	{
		static send_structured_message_lines_fn_t _send_structured_message_lines_impl = init_impl();

		return _send_structured_message_lines_impl;
	}
} // namespace beautylog

#ifdef _WIN64

static beautylog::send_structured_message_lines_fn_t init_impl()
{
	// Set output mode to handle virtual terminal sequences
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE)
	{
		conhost_is_vt_mode = false;
		return send_structured_message_lines_stdout;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode))
	{
		conhost_is_vt_mode = false;
		return send_structured_message_lines_win_conhost;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

	if (!SetConsoleMode(hOut, dwMode))
	{
		conhost_is_vt_mode = false;
		return send_structured_message_lines_win_conhost;
	}
	else
	{
		conhost_is_vt_mode = true;
	}

	return send_structured_message_lines_win_conhost_vtmode;
}

#elif defined(__linux__)

static beautylog::send_structured_message_lines_fn_t init_impl()
{
	if (sd_booted() && getppid() == 1)
		return send_structured_message_lines_journald;
	else
		return send_structured_message_lines_stdout;
}

#else

static beautylog::send_structured_message_lines_fn_t init_impl()
{
	return send_structured_message_lines_stdout;
}

#endif


#ifdef _WIN32
static std::u16string ConvertUTF8ToUTF16(std::string u8str)
{
	std::size_t wchar_length = ::MultiByteToWideChar(
		CP_UTF8,                // convert from UTF-8
		0,   // error on invalid chars
		u8str.c_str(),      // source UTF-8 string
		(int)u8str.length() + 1, // total length of source UTF-8 string,
								 // in CHAR's (= bytes), including end-of-string \0
		nullptr,               // unused - no conversion done in this step
		0                   // request size of destination buffer, in WCHAR's
	);

	if (wchar_length == 0)
	{
		return u"";
	}

	std::u16string wstr;

	wstr.resize(wchar_length);

	//
	// Do the conversion from UTF-8 to UTF-16
	//
	int result = ::MultiByteToWideChar(
		CP_UTF8,                // convert from UTF-8
		0,   // error on invalid chars
		u8str.c_str(),      // source UTF-8 string
		(int)u8str.length() + 1, // total length of source UTF-8 string,

		(wchar_t*)&wstr[0],               // destination buffer
		(int)wchar_length            // size of destination buffer, in WCHAR's
	);


	// Return resulting UTF16 string
	return wstr;
}

void send_structured_message_lines_win_conhost(const std::vector<std::string>& vector_field_string, const std::string message)
{
	{
		std::string msgline;

		for (auto & s : vector_field_string)
		{
			msgline += "[" + s + "]";
		}
		std::u16string l = ConvertUTF8ToUTF16(msgline);
		OutputDebugStringW((const wchar_t*)l.c_str());
	}

	HANDLE handle_stdout = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(handle_stdout, &csbi);

	std::vector<WORD> color_cyc_buffer = {
		FOREGROUND_GREEN,
		FOREGROUND_INTENSITY | FOREGROUND_GREEN,
		FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
		FOREGROUND_RED | FOREGROUND_INTENSITY,
		FOREGROUND_RED | FOREGROUND_BLUE,
		FOREGROUND_BLUE | FOREGROUND_INTENSITY
	};

	for (int i = 0; i < vector_field_string.size(); i++)
	{
		auto s = "[" + vector_field_string[i] + "]";

		std::u16string l = ConvertUTF8ToUTF16(s);
		SetConsoleTextAttribute(handle_stdout, color_cyc_buffer[i % color_cyc_buffer.size()]);
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), l.data(), (DWORD) l.length(), nullptr, nullptr);
	}

	SetConsoleTextAttribute(handle_stdout, csbi.wAttributes);

	std::u16string l = ConvertUTF8ToUTF16(message) + u"\r\n";

	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), l.data(), (DWORD)l.length(), nullptr, nullptr);
}

void send_structured_message_lines_win_conhost_vtmode(const std::vector<std::string>& vector_field_string, const std::string message)
{
	{
		std::string msgline;

		for (auto & s : vector_field_string)
		{
			msgline += "[" + s + "]";
		}
		std::u16string l = ConvertUTF8ToUTF16(msgline);
		OutputDebugStringW((const wchar_t*)l.c_str());
	}

	auto vt_escape = [](int bold, int red, int green, int blue) -> std::string
	{
		char buf[100] = { 0 };
		if (!bold)
			sprintf_s(buf, sizeof buf, "\x1b[38;2;%d;%d;%dm", red, green, blue);
		else
			sprintf_s(buf, sizeof buf, "\x1b[1m\x1b[38;2;%d;%d;%dm", red, green, blue);
		return buf;
	};

	auto auto_format = [](std::string str)
	{
		boost::smatch what;
		if (boost::regex_match(str, what, boost::regex("(.+)=(.*)")))
		{
			std::string f = what[1];
			std::string v = what[2];

			if (f == "CODE_FUNC")
			{
				return v + "()";
			}
			else if (f == "CODE_LINE")
			{
				return "+" + v + ": ";
			}
		}
		return "[" + str +"]";
	};

	for (int i = 0; i < vector_field_string.size(); i++)
	{
		//printf("\x1b[48;2;%d;%d;%dm", red, green, blue); // produces RGB background
		rgbmode clr = color_cyc_buffer[i % boost::size(color_cyc_buffer)];

		auto s = vt_escape(clr.bold, clr.red, clr.green, clr.blue) +
			auto_format(vector_field_string[i]);

		std::u16string l = ConvertUTF8ToUTF16(s);
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), l.data(), (DWORD)l.length(), nullptr, nullptr);
	}

	auto coloredmessage = "\x1b[1m\x1b[0m" + message;

	std::u16string l = ConvertUTF8ToUTF16(coloredmessage) + u"\r\n";
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), l.data(), (DWORD)l.length(), nullptr, nullptr);
}

#endif

void send_structured_message_lines_stdout(const std::vector<std::string>& vector_field_string, const std::string message)
{
	std::cout << message << std::endl;
}

#ifdef __linux__
void send_structured_message_lines_journald(const std::vector<std::string>& vector_field_string, const std::string message)
{
	std::vector<iovec> iovecs(vector_field_string.size() + 1);

	for (int i = 0; i < vector_field_string.size(); i++)
	{
		iovecs[i].iov_base = const_cast<char*>(vector_field_string[i].data());
		iovecs[i].iov_len = vector_field_string[i].size();
	}

	std::string msg_field = "MESSAGE=" + message;

	iovecs[vector_field_string.size()].iov_base = const_cast<char*>(msg_field.c_str());
	iovecs[vector_field_string.size()].iov_len = msg_field.size();

	sd_journal_sendv(&iovecs[0], iovecs.size());
}
#endif
