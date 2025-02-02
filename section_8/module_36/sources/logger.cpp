#include "logger.h"
#include <ios>
#include <string>

namespace skillfactory
{

    Logger::Logger() : log_file("log.txt", std::ios::in | std::ios::out | std::ios::app) {}

    void Logger::write(const std::string& line)
    {
        this->mut.lock();
        this->log_file << line << '\n';
        this->mut.unlock();
    }
    bool Logger::read(std::string& line)
    {
        this->mut.lock_shared();
        std::getline(this->log_file, line);
        this->mut.unlock_shared();
        return this->log_file.eof();
    }

    Logger::~Logger() { this->log_file.close(); }
} // namespace skillfactory
