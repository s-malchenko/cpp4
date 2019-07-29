#include "test_runner.h"

#include <cstddef>  // нужно для nullptr_t

using namespace std;

// Реализуйте шаблон класса UniquePtr
template <typename T>
class UniquePtr
{
private:
    T *_ptr = nullptr;

    void deletePtr()
    {
        if (_ptr)
        {
            delete _ptr;
            _ptr = nullptr;
        }

    }

public:
    UniquePtr()
    {}

    UniquePtr(T *ptr) : _ptr(ptr)
    {}

    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator = (const UniquePtr &) = delete;

    UniquePtr(UniquePtr &&other)
    {
        deletePtr();
        _ptr = other._ptr;
        other._ptr = nullptr;
    }


    UniquePtr &operator = (nullptr_t)
    {
        deletePtr();
        return *this;
    }

    UniquePtr &operator = (UniquePtr &&other)
    {
        deletePtr();
        _ptr = other._ptr;
        other._ptr = nullptr;
        return *this;
    }

    ~UniquePtr()
    {
        deletePtr();
    }

    T &operator * () const
    {
        return *_ptr;
    }

    T *operator -> () const
    {
        return _ptr;
    }

    T *Release()
    {
        auto tmp = _ptr;
        _ptr = nullptr;
        return tmp;
    }

    void Reset(T *ptr)
    {
        deletePtr();
        _ptr = ptr;
    }

    void Swap(UniquePtr &other)
    {
        swap(_ptr, other._ptr);
    }

    T *Get() const
    {
        return _ptr;
    }
};


struct Item
{
    static int counter;
    int value;
    Item(int v = 0): value(v)
    {
        ++counter;
    }
    Item(const Item &other): value(other.value)
    {
        ++counter;
    }
    ~Item()
    {
        --counter;
    }
};

int Item::counter = 0;


void TestLifetime()
{
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        ASSERT_EQUAL(Item::counter, 1);

        ptr.Reset(new Item);
        ASSERT_EQUAL(Item::counter, 1);
    }
    ASSERT_EQUAL(Item::counter, 0);

    {
        UniquePtr<Item> ptr(new Item);
        ASSERT_EQUAL(Item::counter, 1);

        auto rawPtr = ptr.Release();
        ASSERT_EQUAL(Item::counter, 1);

        delete rawPtr;
        ASSERT_EQUAL(Item::counter, 0);
    }
    ASSERT_EQUAL(Item::counter, 0);
}

void TestGetters()
{
    UniquePtr<Item> ptr(new Item(42));
    ASSERT_EQUAL(ptr.Get()->value, 42);
    ASSERT_EQUAL((*ptr).value, 42);
    ASSERT_EQUAL(ptr->value, 42);
}

void TestMove()
{
    UniquePtr<Item> ptr(new Item(42));
    ASSERT_EQUAL(ptr.Get()->value, 42);
    ASSERT_EQUAL((*ptr).value, 42);
    ASSERT_EQUAL(ptr->value, 42);

    UniquePtr<Item> ptr2(move(ptr));
    ASSERT_EQUAL(ptr2.Get()->value, 42);
    ASSERT_EQUAL((*ptr2).value, 42);
    ASSERT_EQUAL(ptr2->value, 42);

    UniquePtr<Item> ptr3;
    ptr3 = move(ptr2);
    ASSERT_EQUAL(ptr3.Get()->value, 42);
    ASSERT_EQUAL((*ptr3).value, 42);
    ASSERT_EQUAL(ptr3->value, 42);
}

int main()
{
    TestRunner tr;
    RUN_TEST(tr, TestLifetime);
    RUN_TEST(tr, TestGetters);
    RUN_TEST(tr, TestMove);
}
