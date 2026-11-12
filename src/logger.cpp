//Copyright 2K Games, Engineering Graduate Program. All rights reserved. DO NOT redistribute.

#include "logger.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

Logger& Logger::GetInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : logFile("log.txt", std::ios_base::trunc) {
    if (!logFile) throw std::runtime_error("Unable to open log.txt");
    logThread = std::thread(&Logger::ProcessLogs, this);
}

Logger::~Logger() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        exitFlag = true;
    }
    queueChanged.notify_one();
    if (logThread.joinable()) {
        logThread.join();
    }
    logFile.flush();
}

void Logger::Log(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        logQueue.push_back(GetCurrentTimestamp() + " " + message);
        ++pendingLogs;
    }
    queueChanged.notify_one();
}

void Logger::Flush() {
    std::unique_lock<std::mutex> lock(mutex);
    queueDrained.wait(lock, [this] { return pendingLogs == 0; });
    logFile.flush();
}

void Logger::ProcessLogs() {
    std::unique_lock<std::mutex> lock(mutex);
    while (true) {
        queueChanged.wait(lock, [this] { return exitFlag || !logQueue.empty(); });
        if (logQueue.empty() && exitFlag) break;

        std::string message = std::move(logQueue.front());
        logQueue.pop_front();
        lock.unlock();
        logFile << message << '\n';
        lock.lock();
        --pendingLogs;
        if (pendingLogs == 0) queueDrained.notify_all();
    }
}

std::string Logger::GetCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &in_time_t);
#else
    localtime_r(&in_time_t, &localTime);
#endif
    std::stringstream ss;
    ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
