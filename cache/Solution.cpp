#include "Common.h"
#include <unordered_map>
#include <mutex>
#include <list>
#include <functional>

using namespace std;

class LruCache : public ICache
{
public:
    LruCache(shared_ptr<IBooksUnpacker> booksUnpacker, const Settings &settings) :
    _unpacker(booksUnpacker),
    _settings(settings),
    _freeMemory(settings.max_memory)
    {
    }

    BookPtr GetBook(const string &bookName) override
    {
        lock_guard<mutex> guard(_locker);

        auto it = _cache.find(bookName);

        if (it != _cache.end())
        {
            it->second.itLru = raiseInLru(it->second.itLru);
            return it->second.book;
        }

        return getNewBook(bookName);
    }
private:
    struct Entry
    {
        BookPtr book;
        list<Entry*>::iterator itLru;
    };

    const shared_ptr<IBooksUnpacker> _unpacker;
    const Settings _settings;
    unordered_map<string, Entry> _cache;
    list<Entry*> _indexLru;
    size_t _freeMemory;
    mutex _locker;

    list<Entry*>::iterator raiseInLru(list<Entry*>::iterator it)
    {
        auto entry = *it;
        _indexLru.erase(it);
        _indexLru.push_front(entry);
        return _indexLru.begin();
    }

    BookPtr getNewBook(const string &bookName)
    {
        BookPtr book = _unpacker->UnpackBook(bookName);
        size_t size = book->GetContent().size();

        if (size > _settings.max_memory)
        {
            clear();
            return move(book);
        }

        if (_freeMemory < size)
        {
            if (size == _settings.max_memory)
            {
                clear();
            }
            else
            {
                freeCacheMemory(size);
            }
        }

        return addBook(move(book));
    }

    BookPtr addBook(BookPtr book)
    {
        auto [it, inserted] = _cache.insert({book->GetName(), {move(book)}});
        _indexLru.push_front(&it->second);
        it->second.itLru = _indexLru.begin();
        _freeMemory -= it->second.book->GetContent().size();
        return it->second.book;
    }

    void clear()
    {
        _cache.clear();
        _indexLru.clear();
        _freeMemory = _settings.max_memory;
    }

    void freeCacheMemory(size_t neededMemory)
    {
        do
        {
            auto entry = _indexLru.back();
            _indexLru.pop_back();
            string nameToDelete = entry->book->GetName();
            _freeMemory += entry->book->GetContent().size();
            _cache.erase(nameToDelete);
        }
        while (_freeMemory < neededMemory);
    }
};


unique_ptr<ICache> MakeCache(
    shared_ptr<IBooksUnpacker> books_unpacker,
    const ICache::Settings &settings
)
{
    return make_unique<LruCache>(books_unpacker, settings);
}
