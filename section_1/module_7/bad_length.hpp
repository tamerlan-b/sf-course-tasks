#pragma once
#include <iostream>
#include <exception>

class BadLength: public std::exception
{
    public:
        virtual const char* what() const noexcept override
        {
            return "ERROR: Length value should be poisitive!";
        }
};