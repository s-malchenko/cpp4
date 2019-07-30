#pragma once
#include <stdexcept>
#include <utility>

namespace RAII
{

template <typename T>
class Booking
{
public:
    Booking(T *provider, int id) : _provider(provider)
    {
    }

    Booking(const Booking &) = delete;

    Booking(Booking &&other)
    {
        stealProvider(std::move(other));
    }

    ~Booking()
    {
        complete();
    }

    Booking &operator=(const Booking &) = delete;

    Booking &operator=(Booking &&other)
    {
        complete();
        stealProvider(std::move(other));
        return *this;
    }

private:
    T *_provider;

    void complete()
    {
        if (_provider)
        {
            _provider->CancelOrComplete(*this);
        }
    }

    void stealProvider(Booking &&other)
    {
        _provider = other._provider;
        other._provider = nullptr;
    }
};

}