#pragma once

#include <string>
#include <ostream>
#include <optional>
#include <functional>

namespace skillfactory
{
    // Хэш-таблица
    template <typename KeyT, typename ValueT>
    class HashTable
    {
        public:    
            HashTable(std::function<int(KeyT)> hash_function):
                count(0), max_size(8), hash_function(std::move(hash_function))
            {
                array = new PairItem[max_size];
            }

            virtual ~HashTable()
            {
                delete[] array;
            }

            // Конструктор копирования
            HashTable(const HashTable& other)
            {
                this->count = other.count;
                this->max_size = other.max_size;
                this->hash_function = other.hash_function;
                
                this->array = new PairItem[other.max_size];
                for (size_t i = 0; i < other.max_size; ++i)
                {
                    this->array[i] = other.array[i];
                }
                
            }

            // Оператор присваивания копированием
            HashTable& operator=(const HashTable& other)
            {
                this->count = other.count;
                this->max_size = other.max_size;
                this->hash_function = other.hash_function;
                
                delete[] this->array;
                this->array = new PairItem[other.max_size];
                for (size_t i = 0; i < other.max_size; ++i)
                {
                    this->array[i] = other.array[i];
                }
                return *this;
            }

            // Конструктор перемещения
            HashTable(HashTable&& other) noexcept
            {
                this->count = other.count;
                this->max_size = other.max_size;
                this->hash_function = std::move(other.hash_function);
                this->array = other.array;

                other.array = nullptr;
                other.count = 0;
                other.max_size = 0;
            }

            // Оператор присваивания перемещением
            HashTable& operator=(HashTable&& other) noexcept
            {
                this->count = other.count;
                this->max_size = other.max_size;
                this->hash_function = std::move(other.hash_function);
                delete[] this->array;
                this->array = other.array;

                other.array = nullptr;
                other.count = 0;
                other.max_size = 0;
                return *this;
            }

            void remove(KeyT key)
            {
                size_t index;
                // берем пробы по всем i от 0 до размера массива
                for(size_t i = 0; i < max_size; ++i)
                {
                    index = this->hash_method(key, i);
                    if(array[index].status == PairItemStatus::engaged && 
                        array[index].key == key)
                    { 
                        array[index].status = PairItemStatus::deleted;
                        --count;
                        return;
                    }
                    else if(array[index].status == PairItemStatus::free)
                    {
                        return;
                    }
                }
            }

            std::optional<ValueT> find(KeyT key)
            {
                for(size_t i = 0; i < max_size; ++i)
                {
                    size_t index = this->hash_method(key, i);
                    if(array[index].status == PairItemStatus::engaged && 
                        array[index].key == key)
                    {
                        return array[index].value;
                    }
                    else if(array[index].status == PairItemStatus::free)
                    {
                        return {};
                    }
                }   
                return {};
            }

            void add(KeyT key, ValueT value)
            {
                size_t index = 0, i = 0;
                
                // берем пробы по всем i от 0 до размера массива
                for(;i < max_size; ++i)
                {
                    index = this->hash_method(key, i);
                    if(array[index].status == PairItemStatus::free)
                    {
                        break;  // найдена пустая ячейка, занимаем ее
                    }
                }

                if(i >= max_size)
                {
                    this->resize();
                    this->add(std::move(key), std::move(value));
                }
                else
                {
                    this->array[index] = std::move(PairItem(key, value));
                    count++;        
                }
            }

            friend std::ostream& operator<<(std::ostream& os, const HashTable& ht)
            {
                for (size_t i = 0; i < ht.max_size; ++i)
                {
                    const auto& item = ht.array[i];
                    if(item.status != PairItemStatus::free)
                    {
                        os << "[" << item.key << ": " << item.value;
                        if(item.status == PairItemStatus::engaged)
                        {
                            os << " (E)";
                        }
                        else if(item.status == PairItemStatus::deleted)
                        {
                            os << " (D)";
                        }
                        os << "], ";
                    }
                }
                return os;
            }

        private:
            void resize()
            {
                PairItem* save = this->array;
                size_t save_ms = this->max_size;
                
                this->max_size *= 2;
                this->array = new PairItem[max_size];
                this->count = 0;
                
                for(size_t i = 0; i < save_ms; ++i)
                {
                    PairItem& old_pair = save[i];
                    if(old_pair.status == PairItemStatus::engaged)
                    {
                        this->add(old_pair.key, old_pair.value);
                    }
                }
                
                delete[] save;
            }
        
            enum PairItemStatus
            {
                free, // свободен
                engaged, //занят
                deleted //удален
            };
            
            // Пара ключ-значение и состояние
            struct PairItem
            {
                // Конструктор по умолчанию
                PairItem(): 
                    key(), 
                    value(),
                    status(PairItemStatus::free) {
                    
                }
                
                PairItem(KeyT key, ValueT value):
                    value(value), key(key),
                    status(PairItemStatus::engaged) {
                }

                // Конструктор копирования
                PairItem(const PairItem& other)
                {
                    this->key = other.key;
                    this->value = other.value;
                    this->status = other.status;
                }

                // Оператор присваивания копированием
                PairItem& operator=(const PairItem& other) {
                    value =  other.value;
                    key = other.key;
                    status = other.status;
                    return *this;
                }

                // Конструктор перемещения
                PairItem(PairItem&& other)
                {
                    this->key = std::move(other.key);
                    this->value = std::move(other.value);
                    this->status = std::move(other.status);
                }

                // Оператор присваивания перемещением
                PairItem& operator=(PairItem&& other) noexcept
                {
                    this->value = std::move(other.value);
                    this->key = std::move(other.key);
                    this->status = std::move(other.status);
                    return *this;
                }
                
                KeyT key; // имя фрукта (ключ)
                ValueT value; // количество фрукта (значение)
                PairItemStatus status; // состояние ячейки
            };

            size_t linear_probing(KeyT key, size_t offset)
            {
                // вычисляем индекс
                size_t sum = this->hash_function(key);
                // линейные пробы
                return (sum % max_size + offset) % max_size; 
            }

            size_t quadradic_probing(KeyT key, size_t offset)
            {
                // вычисляем индекс
                size_t sum = this->hash_function(key);
                // квадратичные пробы
                return (sum % max_size + offset*offset) % max_size; 
            }

            size_t double_hashing(KeyT key, size_t offset)
            {
                // вычисляем индекс
                size_t sum = this->hash_function(key);
                // второе хеширование
                size_t f2 = sum % (max_size * 2);
                // проба
                return (sum % max_size + offset*f2) % max_size; 
            }

            // TODO: заменить на std::function<int(KeyT, size_t)> hash_method;
            size_t hash_method(KeyT key, size_t offset)
            {
                // TODO: реализовать выбор защиты от коллизий в конструкторе
                return this->quadradic_probing(key, offset);
            }
            
            PairItem* array;
            size_t max_size;
            size_t count;
            std::function<size_t(KeyT)> hash_function;
    };

} // namespace skillfactory