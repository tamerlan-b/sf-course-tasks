#pragma once

#include <ostream>
#include <optional>
#include <functional>

namespace skillfactory
{
    enum PairItemStatus
    {
        free, // свободен
        engaged, //занят
        deleted //удален
    };

    // Пара ключ-значение и состояние
    template <typename KeyT, typename ValueT>
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
        PairItem(PairItem&& other) noexcept
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
        
        // Преобразование к bool
        operator bool() const noexcept
        {
            return this->status == PairItemStatus::engaged;
        }
        
        KeyT key; // имя фрукта (ключ)
        ValueT value; // количество фрукта (значение)
        PairItemStatus status; // состояние ячейки
    };

    // Хэш-таблица
    template <typename KeyT, typename ValueT>
    class HashTable
    {
        public:
            HashTable(std::function<int(KeyT)> hash_function):
                hash_function(std::move(hash_function))
            {
                array = new PairItem<KeyT, ValueT>[max_size];
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
                
                this->array = new PairItem<KeyT, ValueT>[other.max_size];
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
                this->array = new PairItem<KeyT, ValueT>[other.max_size];
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

            size_t get_count(){ return this->count;}

            void remove(KeyT key)
            {
                size_t index;
                // берем пробы по всем i от 0 до размера массива
                for(size_t i = 0; i < max_size; ++i)
                {
                    index = this->hash_method(key, i);
                    if(array[index].status == PairItemStatus::free)
                    {
                        break;
                    }
                    if(array[index].status == PairItemStatus::engaged && 
                        array[index].key == key)
                    {
                        array[index].status = PairItemStatus::deleted;
                        --count;
                        break;
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
                    if(array[index].status == PairItemStatus::free)
                    {
                        return {};
                    }
                }   
                return {};
            }

            void add(KeyT key, ValueT value)
            {
                size_t index = 0;
                size_t i = 0;
                
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
                    this->array[index] = std::move(PairItem<KeyT, ValueT>(key, value));
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

            const PairItem<KeyT, ValueT>& operator[](size_t index) const
            {
                if(index >= this->max_size)
                {
                    throw std::out_of_range("Index is out of range!");
                }
                return this->array[index];
            }

            [[nodiscard]] inline size_t size() const
            {
                return this->max_size;
            }

        private:
            void resize()
            {
                PairItem<KeyT, ValueT>* save = this->array;
                size_t save_ms = this->max_size;
                
                this->max_size *= 2;
                this->array = new PairItem<KeyT, ValueT>[max_size];
                this->count = 0;
                
                for(size_t i = 0; i < save_ms; ++i)
                {
                    PairItem<KeyT, ValueT>& old_pair = save[i];
                    if(old_pair.status == PairItemStatus::engaged)
                    {
                        this->add(old_pair.key, old_pair.value);
                    }
                }
                
                delete[] save;
            }
            
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
                size_t func2 = sum % (max_size * 2);
                // проба
                return (sum % max_size + offset * func2) % max_size; 
            }

            // TODO: заменить на std::function<int(KeyT, size_t)> hash_method;
            size_t hash_method(KeyT key, size_t offset)
            {
                // TODO: реализовать выбор защиты от коллизий в конструкторе
                return this->quadradic_probing(key, offset);
            }
            
            PairItem<KeyT, ValueT>* array;
            size_t max_size{8};
            size_t count{0};
            std::function<size_t(KeyT)> hash_function;
    };
} // namespace skillfactory