#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <numeric>

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

        for (Person &p : _data)
        {
            char gender;
            input >> p.name >> p.age >> p.income >> gender;
            p.is_male = (gender == 'M');
        }

        sort(begin(_data), end(_data), [](const Person & lhs, const Person & rhs)
        {
            return lhs.age < rhs.age;
        });
    }

    void ProceedCommands(istream &is, ostream &os)
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

                os << "There are " << std::distance(adult_begin, end(_data))
                     << " adult people for maturity age " << adult_age << '\n';
            }
            else if (command == "WEALTHY")
            {
                int count;
                is >> count;

                auto head = Head(_data, count);

                partial_sort(
                    head.begin(), head.end(), end(_data), [](const Person & lhs, const Person & rhs)
                {
                    return lhs.income > rhs.income;
                });

                int total_income = accumulate(
                                       head.begin(), head.end(), 0, [](int cur, Person & p)
                {
                    return p.income += cur;
                });
                os << "Top-" << count << " people have total income " << total_income << '\n';
            }
            else if (command == "POPULAR_NAME")
            {
                char gender;
                is >> gender;

                IteratorRange range
                {
                    begin(_data),
                    partition(begin(_data), end(_data), [gender](Person & p)
                    {
                        return p.is_male = (gender == 'M');
                    })
                };
                if (range.begin() == range.end())
                {
                    os << "No people of gender " << gender << '\n';
                }
                else
                {
                    sort(range.begin(), range.end(), [](const Person & lhs, const Person & rhs)
                    {
                        return lhs.name < rhs.name;
                    });
                    const string *most_popular_name = &range.begin()->name;
                    int count = 1;
                    for (auto i = range.begin(); i != range.end(); )
                    {
                        auto same_name_end = find_if_not(i, range.end(), [i](const Person & p)
                        {
                            return p.name == i->name;
                        });
                        auto cur_name_count = std::distance(i, same_name_end);
                        if (cur_name_count > count)
                        {
                            count = cur_name_count;
                            most_popular_name = &i->name;
                        }
                        i = same_name_end;
                    }
                    os << "Most popular name among people of gender " << gender << " is "
                         << *most_popular_name << '\n';
                }
            }
        }
    }

private:
    vector<Person> _data;
};

void TestSmoke()
{
    stringstream input("11\nIvan 25 1000 M\nOlga 30 623 W\nSergey 24 825 M\nMaria 42 1254 W\nMikhail 15 215 M\nOleg 18 230 M\nDenis 53 8965 M\nMaxim 37 9050 M\nIvan 47 19050 M\nIvan 17 50 M\nOlga 23 550 W\nAGE 18\nAGE 25\nWEALTHY 5\nPOPULAR_NAME M");
    stringstream output;

    Stats stats(input);
    stats.ProceedCommands(input, output);
    string expected("There are 9 adult people for maturity age 18\nThere are 6 adult people for maturity age 25\nTop-5 people have total income 39319\nMost popular name among people of gender M is Ivan\n");
    ASSERT_EQUAL(expected, output.str());
}

int main()
{
    TestRunner tr;
    RUN_TEST(tr, TestSmoke);
}
