#pragma once
#include <iostream>
#include <exception>

class BadRange: public std::exception
{
    public:
        virtual const char* what() const noexcept override
        {
            return "ERROR: Index is out of range!";
        }
};