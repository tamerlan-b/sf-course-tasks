#pragma once
#include <algorithm> // for std::copy_n
#include "bad_length.h"
#include "bad_range.h"

template <typename T>
class CustomArray
{
private:
        size_t size{};
        T* data{};
    public:
        CustomArray() = default;
        
        CustomArray(size_t length): size{ length }
        {
            if(length == 0)
                throw BadLength();

            if (length > 0)
                this->data = new T[length]{};
        }

        ~CustomArray()
        {
            delete[] this->data;
            // we don't need to set data to null or size to 0 here, since the object will be destroyed immediately after this function anyway
        }

        CustomArray(const CustomArray& a)
        {
            // Set the size of the new array appropriately
            this->reallocate(a.getLength());
            std::copy_n(a.data, this->size, this->data); // copy the elements
        }

        CustomArray& operator=(const CustomArray& a) noexcept
        {
            // Self-assignment check
            if (&a == this)
                return *this;

            // Set the size of the new array appropriately
            this->reallocate(a.getLength());
            std::copy_n(a.data, this->size, this->data); // copy the elements

            return *this;
        }

        void erase() noexcept
        {
            delete[] this->data;
            // We need to make sure we set data to nullptr here, otherwise it will
            // be left pointing at deallocated memory!
            this->data = nullptr;
            this->size = 0;
        }

        T& operator[](size_t index)
        {
            if(index >= this->size)
                throw BadRange();
            return this->data[index];
        }

        void reallocate(size_t newLength) noexcept
        {
            // First we delete any existing elements
            this->erase();

            // If our array is going to be empty now, return here
            if (newLength == 0)
                return;

            // Then we have to allocate new elements
            this->data = new T[newLength];
            this->size = newLength;
        }

        void resize(size_t newLength) noexcept
        {
            // if the array is already the right length, we're done
            if (newLength == this->size)
                return;

            // If we are resizing to an empty array, do that and return
            if (newLength == 0)
            {
                this->erase();
                return;
            }

            // Now we can assume newLength is at least 1 element.  This algorithm
            // works as follows: First we are going to allocate a new array.  Then we
            // are going to copy elements from the existing array to the new array.
            // Once that is done, we can destroy the old array, and make data
            // point to the new array.

            // First we have to allocate a new array
            T* data{ new T[newLength] };

            // Then we have to figure out how many elements to copy from the existing
            // array to the new array.  We want to copy as many elements as there are
            // in the smaller of the two arrays.
            if (this->size > 0)
            {
                size_t elementsToCopy{ (newLength > this->size) ? this->size : newLength };
                std::copy_n(this->data, elementsToCopy, data); // copy the elements
            }

            // Now we can delete the old array because we don't need it any more
            delete[] this->data;

            // And use the new array instead!  Note that this simply makes data point
            // to the same address as the new array we dynamically allocated.  Because
            // data was dynamically allocated, it won't be destroyed when it goes out of scope.
            this->data = data;
            this->size = newLength;
        }

        void insertBefore(T value, size_t index)
        {
            // Sanity check our index value
            if(index > this->size)
                throw BadRange();

            // First create a new array one element larger than the old array
            T* data{ new T[this->size+1] };

            // Copy all of the elements up to the index
            std::copy_n(this->data, index, data);

            // Insert our new element into the new array
            data[index] = value;

            // Copy all of the values after the inserted element
            std::copy_n(this->data + index, this->size - index, data + index + 1);

            // Finally, delete the old array, and use the new array instead
            delete[] this->data;
            this->data = data;
            ++this->size;
        }

        void remove(size_t index)
        {
            // Sanity check our index value
            if(index >= this->size)
                throw BadRange();

            // If this is the last remaining element in the array, set the array to empty and bail out
            if (this->size == 1)
            {
                this->erase();
                return;
            }

            // First create a new array one element smaller than the old array
            T* data{ new T[this->size-1] };

            // Copy all of the elements up to the index
            std::copy_n(this->data, index, data);

            // Copy all of the values after the removed element
            std::copy_n(this->data + index + 1, this->size - index - 1, data + index);

            // Finally, delete the old array, and use the new array instead
            delete[] this->data;
            this->data = data;
            --this->size;
        }

        void insertAtBeginning(T value)
        {
            this->insertBefore(value, 0);
        }

        void insertAtEnd(T value)
        {
            this->insertBefore(value, this->size);
        }

        size_t getLength() const noexcept
        {
            return this->size;
        }
};