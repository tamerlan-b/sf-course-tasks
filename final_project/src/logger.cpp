#include "logger.hpp"
#include <ios>
#include <mutex>
#include <string>

namespace skillfactory
{

    Logger::Logger() : log_file("log.txt", std::ios::in | std::ios::out | std::ios::app) {}

    void Logger::write(const std::string& line)
    {
        std::unique_lock lock(this->mut);
        this->log_file << line << '\n';
    }
    bool Logger::read(std::string& line)
    {
        std::shared_lock lock(this->mut);
        std::getline(this->log_file, line);
        return this->log_file.eof();
    }

    Logger::~Logger() { this->log_file.close(); }
} // namespace skillfactory