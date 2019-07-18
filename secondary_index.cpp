#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <set>

using namespace std;

struct Record
{
    string id;
    string title;
    string user;
    int timestamp;
    int karma;

    bool operator<(const Record &other) const
    {
        return id < other.id;
    }
};

struct compareTs
{
    bool operator() (const set<Record>::const_iterator &i, const set<Record>::const_iterator &j) const
    {
        return i->timestamp < j->timestamp;
    }
};

struct compareKarma
{
    bool operator() (const set<Record>::const_iterator &i, const set<Record>::const_iterator &j) const
    {
        return i->karma < j->karma;
    }
};

// Реализуйте этот класс
class Database
{
public:
    bool Put(const Record &record)
    {
        auto result = _records.insert(record);
        bool inserted = result.second;

        if (inserted)
        {
            updateIndexes(result.first);
        }

        return inserted;
    }

    const Record *GetById(const string &id) const
    {
        Record tmp = {id};
        auto it = _records.find(tmp);

        if (it == _records.end())
        {
            return nullptr;
        }

        return &(*it);
    }

    bool Erase(const string &id)
    {
        Record tmp = {id};
        auto it = _records.find(tmp);

        if (it == _records.end())
        {
            return false;
        }

        eraseFromIndexes(it);
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
    set<Record> _records;
    unordered_multimap<string, set<Record>::const_iterator> _byUser;
    set<set<Record>::const_iterator, compareTs> _byTs;
    set<set<Record>::const_iterator, compareKarma> _byKarma;

    void updateIndexes(set<Record>::const_iterator it)
    {
        _byUser.insert({it->user, it});
        _byTs.insert({it->timestamp, it});
        _byKarma.insert({it->karma, it});
    }

    void eraseFromIndexes(set<Record>::const_iterator it)
    {
        eraseByValue(_byUser, it->user, it);
        eraseByValue(_byTs, it->timestamp, it);
        eraseByValue(_byKarma, it->karma, it);
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

    template <typename Container, typename Key, typename Value>
    void eraseByValue(Container &c, const Key &k, const Value &v)
    {
        auto range = c.equal_range(k);
        auto first = range.first;
        auto last = range.second;

        for (auto it = first; it != last; ++it)
        {
            if (it->second == v)
            {
                c.erase(it);
                return;
            }
        }
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
