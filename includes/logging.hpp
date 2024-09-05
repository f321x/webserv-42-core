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
const LogLevel LOG_LEVEL = TRACE;

void log(const std::string &message, LogLevel level = INFO);

// define logging macros
#ifndef LOGGING_MACROS_H
#define LOGGING_MACROS_H

#define TRACE(message) log(message, TRACE)

#define DEBUG(message) log(message, DEBUG)

#define INFO(message) log(message, INFO)

#define WARN(message) log(message, WARNING)

#define ERROR(message) log(message, ERROR)

#endif
