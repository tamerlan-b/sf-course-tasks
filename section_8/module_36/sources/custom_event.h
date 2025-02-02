#pragma once

#include <functional>
template<typename ... Args>
class Event
{

  public:
    Event() = default;
    virtual ~Event() = default;
    void subscribe(std::function<void(Args...)> handler)
    {
        handlers.emplace_back(std::move(handler));
    }

    // void unsubscribe(std::function<void(Args...)> handler)
    // {
    //     // TODO: implement
    // }

    void happen(Args... args) const
    {
        for(const auto& h: this->handlers)
        {
            h(std::forward<Args...>(args...));
        }
    }
  private:
    std::vector<std::function<void(Args...)>> handlers;

};

