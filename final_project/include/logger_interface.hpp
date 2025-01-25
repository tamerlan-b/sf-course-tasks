#pragma once

#include <string>

namespace skillfactory
{
    class ILogger
    {
      public:
        virtual void write(const std::string& line) = 0;
        virtual bool read(std::string& line) = 0;
        virtual ~ILogger() = default;
    };
} // namespace skillfactory