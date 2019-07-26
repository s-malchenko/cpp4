#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <functional>

using namespace std;

template <typename Iterator>
class IteratorRange
{
public:
    IteratorRange(Iterator begin, Iterator end) : first(begin), last(end)
    {
    }

    Iterator begin() const
    {
        return first;
    }

    Iterator end() const
    {
        return last;
    }

private:
    Iterator first, last;
};

template <typename Collection>
auto Head(Collection &v, size_t top)
{
    return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person
{
    string name;
    int age, income;
    bool is_male;
};

class Stats
{
public:
    explicit Stats(istream &input)
    {
        int count;
        input >> count;
        _data.resize(count);
        map<string, size_t> namesM, namesW;
        auto nameM = namesM.end();
        auto nameW = namesW.end();

        for (Person &p : _data)
        {
            char gender;
            input >> p.name >> p.age >> p.income >> gender;
            p.is_male = (gender == 'M');

            if (p.is_male)
            {
                proceedName(p.name, namesM, nameM);
            }
            else
            {
                proceedName(p.name, namesW, nameW);
            }
        }

        if (nameM != namesM.end())
        {
            _nameM = nameM->first;
        }

        if (nameW != namesW.end())
        {
            _nameW = nameW->first;
        }

        sort(begin(_data), end(_data), [](const Person & lhs, const Person & rhs)
        {
            return lhs.age < rhs.age;
        });

        fillByIncome();
    }

    void ProceedCommands(istream &is, ostream &os) const
    {
        for (string command; is >> command; )
        {
            if (command == "AGE")
            {
                int adult_age;
                is >> adult_age;

                auto adult_begin = lower_bound(
                                       begin(_data), end(_data), adult_age, [](const Person & lhs, int age)
                {
                    return lhs.age < age;
                });

                os << "There are " << distance(adult_begin, end(_data))
                   << " adult people for maturity age " << adult_age << '\n';
            }
            else if (command == "WEALTHY")
            {
                int count;
                is >> count;
                auto head = Head(_byIncome, count);

                int total_income = accumulate(
                                       head.begin(), head.end(), 0, [](int cur, vector<Person>::const_iterator it)
                {
                    return cur + it->income;
                });
                os << "Top-" << count << " people have total income " << total_income << '\n';
            }
            else if (command == "POPULAR_NAME")
            {
                char gender;
                is >> gender;
                auto name = &_nameM;

                if (gender == 'W')
                {
                    name = &_nameW;
                }

                if (name->empty())
                {
                    os << "No people of gender " << gender << '\n';
                }
                else
                {
                    os << "Most popular name among people of gender " << gender << " is "
                       << *name << '\n';
                }
            }
        }
    }

private:
    vector<Person> _data;
    vector<vector<Person>::const_iterator> _byIncome;
    string _nameM, _nameW;

    void proceedName(const string &name, map<string, size_t> &data, map<string, size_t>::iterator &it)
    {
        auto result = data.find(name);

        if (result == data.end())
        {
            result = data.insert({name, 1}).first;
        }
        else
        {
            ++result->second;
        }

        if (it == data.end() ||
            it->second < result->second ||
            (it->second == result->second && it->first > result->first))
        {
            it = result;
        }
    }

    void fillByIncome()
    {
        for (auto it = _data.begin(); it != _data.end(); ++it)
        {
            _byIncome.push_back(it);
        }

        sort(_byIncome.begin(), _byIncome.end(), [](vector<Person>::const_iterator lhs, vector<Person>::const_iterator rhs)
                {
                    return lhs->income > rhs->income;
                });
    }
};

static void fillStream(ostream &os, const vector<string> &v)
{
    for (const auto &i : v)
    {
        os << i << endl;
    }
}

void TestSmoke()
{
    stringstream input;
    stringstream output;

    fillStream(input, {"11",
                       "Ivan 25 1000 M",
                       "Olga 30 623 W",
                       "Sergey 24 825 M",
                       "Maria 42 1254 W",
                       "Mikhail 15 215 M",
                       "Oleg 18 230 M",
                       "Denis 53 8965 M",
                       "Maxim 37 9050 M",
                       "Ivan 47 19050 M",
                       "Ivan 17 50 M",
                       "Olga 23 550 W",
                       "AGE 18",
                       "AGE 25",
                       "WEALTHY 5",
                       "POPULAR_NAME M"
                      });

    Stats stats(input);
    stats.ProceedCommands(input, output);
    stringstream expected;
    fillStream(expected, {"There are 9 adult people for maturity age 18",
                          "There are 6 adult people for maturity age 25",
                          "Top-5 people have total income 39319",
                          "Most popular name among people of gender M is Ivan"
                         });
    ASSERT_EQUAL(expected.str(), output.str());
}

void TestDoubleSmoke()
{
    stringstream input;
    stringstream output;

    fillStream(input, {"11",
                       "Ivan 25 1000 M",
                       "Olga 30 623 W",
                       "Sergey 24 825 M",
                       "Maria 42 1254 W",
                       "Mikhail 15 215 M",
                       "Oleg 18 230 M",
                       "Denis 53 8965 M",
                       "Maxim 37 9050 M",
                       "Ivan 47 19050 M",
                       "Ivan 17 50 M",
                       "Olga 23 550 W",
                       "AGE 18",
                       "AGE 25",
                       "WEALTHY 5",
                       "POPULAR_NAME M",
                       "AGE 18",
                       "AGE 25",
                       "WEALTHY 5",
                       "POPULAR_NAME M"
                      });

    Stats stats(input);
    stats.ProceedCommands(input, output);
    stringstream expected;
    fillStream(expected, {"There are 9 adult people for maturity age 18",
                          "There are 6 adult people for maturity age 25",
                          "Top-5 people have total income 39319",
                          "Most popular name among people of gender M is Ivan",
                          "There are 9 adult people for maturity age 18",
                          "There are 6 adult people for maturity age 25",
                          "Top-5 people have total income 39319",
                          "Most popular name among people of gender M is Ivan"
                         });
    ASSERT_EQUAL(expected.str(), output.str());
}

int main()
{
    TestRunner tr;
    RUN_TEST(tr, TestSmoke);
    RUN_TEST(tr, TestDoubleSmoke);
    Stats stats(cin);
    stats.ProceedCommands(cin, cout);
}
