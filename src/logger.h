//Copyright 2K Games, Engineering Graduate Program. All rights reserved. DO NOT redistribute.

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

// Implement the following methods:
// Log
// Flush
// Message format is "2024-08-30 14:48:16 Starting game..."

class Logger {
   public:
    static Logger& GetInstance();
    void Log(const std::string& message);
    void Flush();

    ~Logger();

   private:
    Logger();

    std::ofstream logFile;
    std::thread logThread;
    std::deque<std::string> logQueue;
    std::mutex mutex;
    std::condition_variable queueChanged;
    std::condition_variable queueDrained;
    size_t pendingLogs = 0;
    bool exitFlag = false;

    void ProcessLogs();
    std::string GetCurrentTimestamp();
};

#endif  // LOGGER_H
