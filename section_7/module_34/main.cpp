#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <type_traits>
#include <typeinfo>

template <typename T>
concept ComplexConcept = requires(T t) {
    { t.hash() } -> std::convertible_to<long>;
    { t.toString() } -> std::same_as<std::string>;
    !std::has_virtual_destructor_v<T>;
};

class EmptyClass
{
};

class HashClass
{
  public:
    long hash() { return 0L; }
};

class ToStringClass
{
  public:
    std::string toString() { return ""; }
};

class VirtualDestructorClass
{
  public:
    virtual ~VirtualDestructorClass() {}
};

class FullClass
{
  public:
    long hash() { return 0L; }
    std::string toString() { return ""; }
    // virtual ~Full() = delete;
};

template <ComplexConcept T> void concept_func(T t) { std::cout << typeid(t).name() << '\n'; }

int main()
{
    // concept_func(EmptyClass());  // ошибка компиляции
    // concept_func(HashClass());   // ошибка компиляции
    // concept_func(ToStringClass());   // ошибка компиляции
    // concept_func(VirtualDestructorClass());  // ошибка компиляции
    concept_func(FullClass()); // ок
    return 0;
}