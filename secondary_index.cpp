#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record
{
    string id;
    string title;
    string user;
    int timestamp;
    int karma;
};

// Реализуйте этот класс
class Database
{
public:
    bool Put(const Record &record)
    {
        auto [it, inserted] = _records.insert(make_pair(record.id, Item {record}));

        if (!inserted)
        {
            return false;
        }

        updateIndexes(it);
        return true;
    }

    const Record *GetById(const string &id) const
    {
        auto it = _records.find(id);

        if (it == _records.end())
        {
            return nullptr;
        }

        return &it->second.record;
    }

    bool Erase(const string &id)
    {
        auto it = _records.find(id);

        if (it == _records.end())
        {
            return false;
        }

        eraseFromIndexes(it->second);
        _records.erase(it);
        return true;
    }

    template <typename Callback>
    void RangeByTimestamp(int low, int high, Callback callback) const
    {
        rangeCallback(_byTs, low, high, callback);
    }

    template <typename Callback>
    void RangeByKarma(int low, int high, Callback callback) const
    {
        rangeCallback(_byKarma, low, high, callback);
    }

    template <typename Callback>
    void AllByUser(const string &user, Callback callback) const
    {
        applyCallback(_byUser.equal_range(user), callback);
    }

private:
    template <typename T>
    using Index = multimap<T, const Record *>;

    struct Item
    {
        Record record;
        Index<string>::iterator userIt;
        Index<int>::iterator tsIt;
        Index<int>::iterator karmaIt;
    };

    unordered_map<string, Item> _records;
    Index<string> _byUser;
    Index<int> _byTs;
    Index<int> _byKarma;

    template <typename Iterator>
    void updateIndexes(Iterator it)
    {
        Item &item = it->second;
        const Record *pRecord = &item.record;
        item.userIt = _byUser.insert({pRecord->user, pRecord});
        item.tsIt = _byTs.insert({pRecord->timestamp, pRecord});
        item.karmaIt = _byKarma.insert({pRecord->karma, pRecord});
    }

    void eraseFromIndexes(const Item &item)
    {
        _byUser.erase(item.userIt);
        _byTs.erase(item.tsIt);
        _byKarma.erase(item.karmaIt);
    }

    template <typename Pair, typename Callback>
    void applyCallback(const Pair &range, Callback callback) const
    {
        for (auto it = range.first; it != range.second; ++it)
        {
            if (!callback(*it->second))
            {
                return;
            }
        }
    }

    template <typename Container, typename Callback, typename Value>
    void rangeCallback(const Container &c, const Value &low, const Value &high, Callback callback) const
    {
        auto first = c.lower_bound(low);
        auto last = c.upper_bound(high);
        applyCallback(make_pair(first, last), callback);
    }
};

void TestRangeBoundaries()
{
    const int good_karma = 1000;
    const int bad_karma = -10;

    Database db;
    db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
    db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

    int count = 0;
    db.RangeByKarma(bad_karma, good_karma, [&count](const Record &)
    {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestSameUser()
{
    Database db;
    db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
    db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

    int count = 0;
    db.AllByUser("master", [&count](const Record &)
    {
        ++count;
        return true;
    });

    ASSERT_EQUAL(2, count);
}

void TestReplacement()
{
    const string final_body = "Feeling sad";

    Database db;
    db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
    db.Erase("id");
    db.Put({"id", final_body, "not-master", 1536107260, -10});

    auto record = db.GetById("id");
    ASSERT(record != nullptr);
    ASSERT_EQUAL(final_body, record->title);
}

int main()
{
    TestRunner tr;
    RUN_TEST(tr, TestRangeBoundaries);
    RUN_TEST(tr, TestSameUser);
    RUN_TEST(tr, TestReplacement);
    return 0;
}
