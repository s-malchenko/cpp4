#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <optional>

using namespace std;

struct Email
{
    string from;
    string to;
    string body;
};

class Worker
{
public:
    virtual ~Worker() = default;
    virtual void Process(unique_ptr<Email> email) = 0;
    virtual void Run()
    {
        // только первому worker-у в пайплайне нужно это имплементировать
        throw logic_error("Unimplemented");
    }

protected:
    // реализации должны вызывать PassOn, чтобы передать объект дальше
    // по цепочке обработчиков
    void PassOn(unique_ptr<Email> email) const
    {
        if (_next)
        {
            _next.value()->Process(move(email));
        }
    }

    static unique_ptr<Email> makeUniqueEmail(const string &from, const string &to, const string &body)
    {
        auto p = make_unique<Email>();
        *p = {from, to, body};
        return p;
    }

public:
    void SetNext(unique_ptr<Worker> next)
    {
        _next = move(next);
    }

private:
    optional<unique_ptr<Worker>> _next;
};


class Reader : public Worker
{
public:
    Reader(istream &is) : _stream(is) {}

    void Run() override
    {
        string from, to, body;

        while (getline(_stream, from) &&
                getline(_stream, to) &&
                getline(_stream, body))
        {
            Process(makeUniqueEmail(from, to, body));
        }
    }

    void Process(unique_ptr<Email> email)
    {
        PassOn(move(email));
    }

private:
    istream &_stream;
};


class Filter : public Worker
{
public:
    using Function = function<bool(const Email &)>;

    Filter(Function pred) : _pred(pred) {}

    void Process(unique_ptr<Email> email) override
    {
        if (_pred(*email))
        {
            PassOn(move(email));
        }
    }

private:
    const Function _pred;
};


class Copier : public Worker
{
public:
    Copier(const string &copyTo) : _copyTo(copyTo) {}

    void Process(unique_ptr<Email> email) override
    {
        unique_ptr<Email> copy;

        if (_copyTo != email->to)
        {
            copy.reset(new Email{email->from, _copyTo, email->body});
        }

        PassOn(move(email));

        if (copy)
        {
            PassOn(move(copy));
        }
    }

private:
    const string _copyTo;
};


class Sender : public Worker
{
public:
    Sender(ostream &os) : _stream(os) {}

    void Process(unique_ptr<Email> email) override
    {
        _stream << email->from << endl;
        _stream << email->to << endl;
        _stream << email->body << endl;
        PassOn(move(email));
    }

private:
    ostream &_stream;
};


// реализуйте класс
class PipelineBuilder
{
public:
    // добавляет в качестве первого обработчика Reader
    explicit PipelineBuilder(istream &in) : _chain(make_unique<Reader>(in))
    {
        _last = _chain.get();
    }

    // добавляет новый обработчик Filter
    PipelineBuilder &FilterBy(Filter::Function filter)
    {
        append(make_unique<Filter>(filter));
        return *this;
    }

    // добавляет новый обработчик Copier
    PipelineBuilder &CopyTo(string recipient)
    {
        append(make_unique<Copier>(recipient));
        return *this;
    }

    // добавляет новый обработчик Sender
    PipelineBuilder &Send(ostream &out)
    {
        append(make_unique<Sender>(out));
        return *this;
    }

    // возвращает готовую цепочку обработчиков
    unique_ptr<Worker> Build()
    {
        return move(_chain);
    }

private:
    unique_ptr<Worker> _chain;
    Worker *_last;

    void append(unique_ptr<Worker> ptr)
    {
        auto tmp = ptr.get();
        _last->SetNext(move(ptr));
        _last = tmp;
    }
};


void TestSanity()
{
    string input = (
                       "erich@example.com\n"
                       "richard@example.com\n"
                       "Hello there\n"

                       "erich@example.com\n"
                       "ralph@example.com\n"
                       "Are you sure you pressed the right button?\n"

                       "ralph@example.com\n"
                       "erich@example.com\n"
                       "I do not make mistakes of that kind\n"
                   );
    istringstream inStream(input);
    ostringstream outStream;

    PipelineBuilder builder(inStream);
    builder.FilterBy([](const Email & email)
    {
        return email.from == "erich@example.com";
    });
    builder.CopyTo("richard@example.com");
    builder.Send(outStream);
    auto pipeline = builder.Build();

    pipeline->Run();

    string expectedOutput = (
                                "erich@example.com\n"
                                "richard@example.com\n"
                                "Hello there\n"

                                "erich@example.com\n"
                                "ralph@example.com\n"
                                "Are you sure you pressed the right button?\n"

                                "erich@example.com\n"
                                "richard@example.com\n"
                                "Are you sure you pressed the right button?\n"
                            );

    ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main()
{
    TestRunner tr;
    RUN_TEST(tr, TestSanity);
    return 0;
}
