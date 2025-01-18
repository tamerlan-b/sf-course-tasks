#pragma once
#include "logger_interface.hpp"
#include <fstream>
#include <shared_mutex>
#include <string>

namespace skillfactory
{

    class Logger : public ILogger
    {
      private:
        std::shared_mutex mut;
        std::fstream log_file;

      public:
        Logger();
        void write(const std::string& line) override;
        bool read(std::string& line) override;
        virtual ~Logger();
    };
} // namespace skillfactory