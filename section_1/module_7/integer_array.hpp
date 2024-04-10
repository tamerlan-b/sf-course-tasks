#pragma once

class IntegerArray
{
private:
        int m_length{};
        int* m_data{};
    public:
        IntegerArray() = default;
        IntegerArray(int length);
        ~IntegerArray();
        IntegerArray(const IntegerArray& a);
        IntegerArray& operator=(const IntegerArray& a) noexcept;
        void erase() noexcept;
        int& operator[](int index);
        // reallocate resizes the array.  Any existing elements will be destroyed.  This function operates quickly.
        void reallocate(int newLength) noexcept;
        // resize resizes the array.  Any existing elements will be kept.  This function operates slowly.
        void resize(int newLength) noexcept;
        void insertBefore(int value, int index);
        void remove(int index);
        // A couple of additional functions just for convenience
        void insertAtBeginning(int value);
        void insertAtEnd(int value);
        int getLength() const noexcept;
};