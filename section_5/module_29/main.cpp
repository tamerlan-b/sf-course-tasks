#include <array>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <ostream>
#include <stdexcept>
#include <thread>

struct Node
{
    int value;
    Node* next;
    std::mutex* node_mutex;

    Node() : next(nullptr), node_mutex(new std::mutex()) {}
    Node(int value) : Node() { this->value = value; }
    ~Node() { delete node_mutex; }
};

class FineGrainedQueue
{
  private:
    Node* head{nullptr};
    std::mutex* queue_mutex;

  public:
    FineGrainedQueue() : queue_mutex(new std::mutex()) {}
    void push(int value)
    {
        this->queue_mutex->lock();
        if (this->head == nullptr)
        {
            this->head = new Node(value);
            this->queue_mutex->unlock();
            return;
        }

        Node* cur = this->head;
        cur->node_mutex->lock(); // блокируем только head

        while (cur->next != nullptr)
        {
            // блокируем текущий элемент (т.к. знаем что он не null)
            cur->next->node_mutex->lock();
            // снимаем блокировку с прошлого элемента
            cur->node_mutex->unlock();
            // если текущий элемент - начало списка
            if (cur == this->head)
            {
                this->queue_mutex->unlock(); // снимем блокировку с списка
            }
            cur = cur->next;
        }

        Node* node = new Node(value);
        cur->next = node;
        cur->node_mutex->unlock(); // снимаем блокировку с текущего элемента
        // если текущий элемент - начало списка
        if (cur == this->head)
        {
            this->queue_mutex->unlock(); // снимем блокировку со списка
        }
    }

    void insertIntoMiddle(int value, int pos)
    {
        // Выбрасываем исключение, если pos < 0
        if (pos < 0)
        {
            throw std::out_of_range("pos should be >= 0!");
        }

        // Вставка в пустой список
        this->queue_mutex->lock();
        if (this->head == nullptr)
        {
            this->head = new Node(value);
            this->queue_mutex->unlock();
            return;
        }

        Node* before_inserted = this->head;
        before_inserted->node_mutex->lock();

        // Вставка в начало списка
        if (pos == 0)
        {
            Node* inserted = new Node(value);
            inserted->next = before_inserted;
            before_inserted->node_mutex->unlock();
            this->head = inserted;
            this->queue_mutex->unlock();
            return;
        }

        // Находим элемент, за которым будем вставлять
        int index = 0;
        Node* old_before_inserted;
        while (before_inserted->next != nullptr && index < pos - 1)
        {
            // Блокируем следующий элемент
            before_inserted->next->node_mutex->lock();
            old_before_inserted = before_inserted;
            // Меняем указатель
            before_inserted = before_inserted->next;
            // Снимаем блокировку с предыдущего элемента
            old_before_inserted->node_mutex->unlock();

            // Снимаем блокировку со списка
            if (old_before_inserted == this->head)
            {
                this->queue_mutex->unlock();
            }
            ++index;
        }

        // Создаем узел со вставляемым элементов
        Node* inserted = new Node(value);
        // Вставляем элемент
        if (before_inserted->next != nullptr)
        {
            inserted->next = before_inserted->next;
        }
        before_inserted->next = inserted;
        before_inserted->node_mutex->unlock();
        // Снимаем блокировку со списка
        if (before_inserted == this->head)
        {
            this->queue_mutex->unlock();
        }
    }

    virtual ~FineGrainedQueue()
    {
        // TODO: нужно ли здесь добавлять потокобезопасность?
        Node* cur = this->head;
        while (cur != nullptr)
        {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        delete queue_mutex;
    }

    void print() const
    {
        this->queue_mutex->lock();
        if (this->head == nullptr)
        {
            this->queue_mutex->unlock();
            return;
        }
        // TODO: использовать thread-safe std::cout
        std::cout << "Queue: ";
        Node* cur = this->head;
        do
        {
            cur->node_mutex->lock();
            std::cout << cur->value << " ";
            if (cur == this->head)
            {
                this->queue_mutex->unlock();
            }
            cur->node_mutex->unlock();
            cur = cur->next;
        } while (cur != nullptr);
        std::cout << '\n';
    }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv)
{
    FineGrainedQueue queue;

    // Наполняем очередь элементами
    // const int max_elems = 10;
    // for (size_t i = 0; i < max_elems; ++i)
    // {
    //     queue.push(i);
    // }

    // Выводим очередь
    // queue.print();

    // Многопоточно вставляем элементы в середину очереди
    const int num_threads = 5;
    std::array<std::thread, num_threads> threads;

    for (size_t i = 0; i < threads.size(); ++i)
    {
        threads[i] = std::thread([&queue, i]() { queue.insertIntoMiddle((i + 1) * 100, 5); });
    }

    for (auto& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    // Выводим очередь
    queue.print();
    return 0;
}