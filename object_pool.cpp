#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <queue>
#include <stdexcept>
#include <unordered_set>
using namespace std;

template <class T>
class ObjectPool
{
public:
    T *Allocate()
    {
        T *result;

        if (_deallocated.empty())
        {
            result = new T;
        }
        else
        {
            result = _deallocated.front();
            _deallocated.pop();
        }

        _allocated.insert(result);
        return result;
    }

    T *TryAllocate()
    {
        if (_deallocated.empty())
        {
            return nullptr;
        }

        T* result = _deallocated.front();
        _deallocated.pop();
        _allocated.insert(result);
        return result;
    }

    void Deallocate(T *object)
    {
        auto it = _allocated.find(object);

        if (it == _allocated.end())
        {
            throw invalid_argument("wrong deallocation");
        }

        _allocated.erase(it);
        _deallocated.push(object);
    }

    ~ObjectPool()
    {
        for (const auto &p : _allocated)
        {
            delete p;
        }

        while (!_deallocated.empty())
        {
            auto p = _deallocated.front();
            delete p;
            _deallocated.pop();
        }
    }

private:
    unordered_set<T*> _allocated;
    queue<T*> _deallocated;
};

void TestObjectPool()
{
    ObjectPool<string> pool;

    auto p1 = pool.Allocate();
    auto p2 = pool.Allocate();
    auto p3 = pool.Allocate();

    *p1 = "first";
    *p2 = "second";
    *p3 = "third";

    pool.Deallocate(p2);
    ASSERT_EQUAL(*pool.Allocate(), "second");

    pool.Deallocate(p3);
    pool.Deallocate(p1);
    ASSERT_EQUAL(*pool.Allocate(), "third");
    ASSERT_EQUAL(*pool.Allocate(), "first");

    pool.Deallocate(p1);
}

int main()
{
    TestRunner tr;
    RUN_TEST(tr, TestObjectPool);
    return 0;
}
