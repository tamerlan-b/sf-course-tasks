#include "integer_array.hpp"
#include <algorithm> // for std::copy_n
#include "bad_length.hpp"
#include "bad_range.hpp"

IntegerArray::IntegerArray(int length): m_length{ length }
{
    if(length <= 0)
        throw BadLength();

    if (length > 0)
        this->m_data = new int[length]{};
}

IntegerArray::~IntegerArray()
{
    delete[] this->m_data;
    // we don't need to set m_data to null or m_length to 0 here, since the object will be destroyed immediately after this function anyway
}

IntegerArray::IntegerArray(const IntegerArray& a)
{
    // Set the size of the new array appropriately
    this->reallocate(a.getLength());
    std::copy_n(a.m_data, this->m_length, this->m_data); // copy the elements
}

IntegerArray& IntegerArray::operator=(const IntegerArray& a) noexcept
{
    // Self-assignment check
    if (&a == this)
        return *this;

    // Set the size of the new array appropriately
    this->reallocate(a.getLength());
    std::copy_n(a.m_data, this->m_length, this->m_data); // copy the elements

    return *this;
}

void IntegerArray::erase() noexcept
{
    delete[] this->m_data;
    // We need to make sure we set m_data to nullptr here, otherwise it will
    // be left pointing at deallocated memory!
    this->m_data = nullptr;
    this->m_length = 0;
}

int& IntegerArray::operator[](int index)
{
    if(index < 0 || index >= this->m_length)
        throw BadRange();
    return this->m_data[index];
}

void IntegerArray::reallocate(int newLength) noexcept
{
    // First we delete any existing elements
    this->erase();

    // If our array is going to be empty now, return here
    if (newLength <= 0)
        return;

    // Then we have to allocate new elements
    this->m_data = new int[newLength];
    this->m_length = newLength;
}

void IntegerArray::resize(int newLength) noexcept
{
    // if the array is already the right length, we're done
    if (newLength == this->m_length)
        return;

    // If we are resizing to an empty array, do that and return
    if (newLength <= 0)
    {
        this->erase();
        return;
    }

    // Now we can assume newLength is at least 1 element.  This algorithm
    // works as follows: First we are going to allocate a new array.  Then we
    // are going to copy elements from the existing array to the new array.
    // Once that is done, we can destroy the old array, and make m_data
    // point to the new array.

    // First we have to allocate a new array
    int* data{ new int[newLength] };

    // Then we have to figure out how many elements to copy from the existing
    // array to the new array.  We want to copy as many elements as there are
    // in the smaller of the two arrays.
    if (this->m_length > 0)
    {
        int elementsToCopy{ (newLength > this->m_length) ? this->m_length : newLength };
        std::copy_n(this->m_data, elementsToCopy, data); // copy the elements
    }

    // Now we can delete the old array because we don't need it any more
    delete[] this->m_data;

    // And use the new array instead!  Note that this simply makes m_data point
    // to the same address as the new array we dynamically allocated.  Because
    // data was dynamically allocated, it won't be destroyed when it goes out of scope.
    this->m_data = data;
    this->m_length = newLength;
}

void IntegerArray::insertBefore(int value, int index)
{
    // Sanity check our index value
    if(index < 0 || index > this->m_length)
        throw BadRange();

    // First create a new array one element larger than the old array
    int* data{ new int[this->m_length+1] };

    // Copy all of the elements up to the index
    std::copy_n(this->m_data, index, data);

    // Insert our new element into the new array
    data[index] = value;

    // Copy all of the values after the inserted element
    std::copy_n(this->m_data + index, this->m_length - index, data + index + 1);

    // Finally, delete the old array, and use the new array instead
    delete[] this->m_data;
    this->m_data = data;
    ++this->m_length;
}

void IntegerArray::remove(int index)
{
    // Sanity check our index value
    if(index < 0 || index >= this->m_length)
        throw BadRange();

    // If this is the last remaining element in the array, set the array to empty and bail out
    if (this->m_length == 1)
    {
        this->erase();
        return;
    }

    // First create a new array one element smaller than the old array
    int* data{ new int[this->m_length-1] };

    // Copy all of the elements up to the index
    std::copy_n(this->m_data, index, data);

    // Copy all of the values after the removed element
    std::copy_n(this->m_data + index + 1, this->m_length - index - 1, data + index);

    // Finally, delete the old array, and use the new array instead
    delete[] this->m_data;
    this->m_data = data;
    --this->m_length;
}

void IntegerArray::insertAtBeginning(int value)
{
    this->insertBefore(value, 0);
}

void IntegerArray::insertAtEnd(int value)
{
    this->insertBefore(value, this->m_length);
}

int IntegerArray::getLength() const noexcept
{
    return this->m_length;
}
