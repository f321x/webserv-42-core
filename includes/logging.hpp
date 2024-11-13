#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

enum LogLevel
{
	TRACE,
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

// set the log level for the webserver
extern LogLevel LOG_LEVEL;

LogLevel parseLogLevel(const std::string &level);
void setLogLevel(LogLevel level);

void log(const std::string &message, LogLevel level = INFO);

// define logging macros
#ifndef LOGGING_MACROS_H
#define LOGGING_MACROS_H

#define TRACE(message) log(message, TRACE)

#ifndef DEBUG
#define DEBUG(message) log(message, DEBUG)
#endif

#ifndef INFO
#define INFO(message) log(message, INFO)
#endif

#ifndef WARN
#define WARN(message) log(message, WARNING)
#endif

#ifndef ERROR
#define ERROR(message) log(message, ERROR)
#endif

#endif
