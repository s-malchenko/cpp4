#include "test_runner.h"
#include "profile.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <numeric>
#include <ostream>
#include <sstream>
#include <string>

using namespace std;

/**
 * @brief      Class and functions for date.
 */
class Date
{
public:
    Date() { setDate(1, 1, 1); }
    Date(int year, int month, int day) { setDate(year, month, day); }
    Date(const std::string &str) { setDate(str); }

    Date(std::istream &is)
    {
        string str;
        is >> str;
        setDate(str);
    }

    Date(time_t ts)
    {
        tm *t = localtime(&ts);
        _day = t->tm_mday;
        _month = t->tm_mon + 1;
        _year = t->tm_year + 1900;
    }

    int GetYear() const { return _year; }
    int GetMonth() const { return _month; }
    int GetDay() const { return _day; }

    Date NextDay()
    {
        return {AsTimestamp() + SECONDS_IN_DAY};
    }

    time_t AsTimestamp() const
    {
        std::tm t;
        t.tm_sec   = 0;
        t.tm_min   = 0;
        t.tm_hour  = 0;
        t.tm_mday  = _day;
        t.tm_mon   = _month - 1;
        t.tm_year  = _year - 1900;
        t.tm_isdst = 0;
        return mktime(&t);
    }

    static const int SECONDS_IN_DAY = 60 * 60 * 24;
private:
    int _year;
    int _month;
    int _day;

    void checkFormat(bool arg, const std::string &str)
    {
        if (!arg)
        {
            throw invalid_argument("Wrong date format: " + str);
        }
    }

    void setDate(int year, int month, int day)
    {
        if (month < 1 || month > 12)
        {
            throw invalid_argument("Month value is invalid: " + to_string(month));
        }

        if (day < 1 || day > 31)
        {
            throw invalid_argument("Day value is invalid: " + to_string(day));
        }

        _year = year;
        _month = month;
        _day = day;
    }

    void setDate(const std::string &str)
    {
        istringstream ss(str.c_str());
        int year, month, day;
        char c;
        checkFormat(bool(ss >> year), str);
        checkFormat(bool(ss >> c), str);
        checkFormat(c == '-', str);
        checkFormat(bool(ss >> month), str);
        checkFormat(bool(ss >> c), str);
        checkFormat(c == '-', str);
        checkFormat(bool(ss >> day), str);
        checkFormat(!(ss >> c), str);
        setDate(year, month, day);
    }
};

bool operator<(const Date &lhs, const Date &rhs)
{
    if (lhs.GetYear() < rhs.GetYear())
    {
        return true;
    }
    else if (lhs.GetYear() > rhs.GetYear())
    {
        return false;
    }

    if (lhs.GetMonth() < rhs.GetMonth())
    {
        return true;
    }
    else if (lhs.GetMonth() > rhs.GetMonth())
    {
        return false;
    }

    if (lhs.GetDay() < rhs.GetDay())
    {
        return true;
    }

    return false;
}


bool operator==(const Date &lhs, const Date &rhs)
{
    return (lhs.GetYear() == rhs.GetYear() &&
            lhs.GetMonth() == rhs.GetMonth() &&
            lhs.GetDay() == rhs.GetDay());
}

bool operator>=(const Date &lhs, const Date &rhs)
{
    return !(lhs < rhs);
}

bool operator>(const Date &lhs, const Date &rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

bool operator<=(const Date &lhs, const Date &rhs)
{
    return !(lhs > rhs);
}

bool operator!=(const Date &lhs, const Date &rhs)
{
    return !(lhs == rhs);
}

ostream &operator<<(ostream &stream, const Date &date)
{
    stream << setfill('0') << setw(4) << date.GetYear() << "-"
           << setw(2) << date.GetMonth() << "-" << setw(2) << date.GetDay();
    return stream;
}

Date ParseDate(istream &is)
{
    return {is};
}

int ComputeDaysDiff(const Date &date_to, const Date &date_from)
{
    const time_t timestamp_to = date_to.AsTimestamp();
    const time_t timestamp_from = date_from.AsTimestamp();
    return (timestamp_to - timestamp_from) / Date::SECONDS_IN_DAY;
}

struct DateHasher
{
    size_t operator()(const Date &d) const
    {
        size_t k = 2004007;

        return d.GetYear() * k * k + d.GetMonth() * k + d.GetDay();
    };
};

/**
 * @brief      Class for budget.
 */
class Budget
{
public:
    Budget() : _earnings(vector<double>(getIndex({2101, 1, 1}) + 1)) {}

    void Earn(Date from, Date to, unsigned int value)
    {
        double dailyEarning = value / (ComputeDaysDiff(to, from) + 1.0);
        for_each(getValue(from), getValue(to) + 1, [dailyEarning](auto & b) { b += dailyEarning; });
    }

    double Income(Date from, Date to) const
    {
        return accumulate(getValue(from), next(getValue(to)), 0.0);
    }

    void PayTax(Date from, Date to)
    {
        for_each(getValue(from), next(getValue(to)), [](auto & b) { b *= 0.87; });
    }

protected:
    vector<double> _earnings;

    static size_t getIndex(const Date &d)
    {
        return ComputeDaysDiff(d, {2000, 1, 1});
    }

    vector<double>::iterator getValue(const Date &d)
    {
        return _earnings.begin() + getIndex(d);
    }

    vector<double>::const_iterator getValue(const Date &d) const
    {
        return _earnings.begin() + getIndex(d);
    }

};

void BudgetCycle(std::istream &in, std::ostream &out)
{
    int n;
    in >> n;
    Budget budget;
    out.precision(25);

    for (int i = 0; i < n; ++i)
    {
        string cmd;
        in >> cmd;
        Date from(in);
        Date to(in);

        if (cmd == "Earn")
        {
            int value;
            in >> value;
            budget.Earn(from, to, value);
        }
        else if (cmd == "ComputeIncome")
        {
            out << budget.Income(from, to);

            if (i != n - 1)
            {
                out << endl;
            }
        }
        else
        {
            budget.PayTax(from, to);
        }
    }
}

/**
 * @brief      Functions for testing Budget class.
 */

void TestEarnDay()
{
    Budget b;
    Date date(2000, 1, 1);
    b.Earn(date, date, 50);
    ASSERT_EQUAL(50, b.Income(date, date));
}

void TestEarnYear()
{
    Budget b;
    Date from(2014, 6, 13);
    Date to(2015, 6, 12);
    b.Earn(from, to, 365);
    ASSERT_EQUAL(365, b.Income(from, to));

    for (Date d = from; d < to.NextDay(); d = d.NextDay())
    {
        ASSERT_EQUAL(1, b.Income(d, d));
    }
}

void TestPayTax()
{
    Budget b;
    Date from(2014, 6, 13);
    Date to(2015, 6, 12);
    b.Earn(from, to, 365);
    b.PayTax(from, to);
    ASSERT_EQUAL(to_string(365 * 0.87), to_string(b.Income(from, to)));

    for (Date d = from; d < to.NextDay(); d = d.NextDay())
    {
        ASSERT_EQUAL(0.87, b.Income(d, d));
    }
}

void TestCycle()
{
    {
        stringstream in(R"(8
Earn 2000-01-02 2000-01-06 20
ComputeIncome 2000-01-01 2001-01-01
PayTax 2000-01-02 2000-01-03
ComputeIncome 2000-01-01 2001-01-01
Earn 2000-01-03 2000-01-03 10
ComputeIncome 2000-01-01 2001-01-01
PayTax 2000-01-03 2000-01-03
ComputeIncome 2000-01-01 2001-01-01)");
        string expected(R"(20
18.96000000000000085265128
28.96000000000000085265128
27.20759999999999934061634)");
        stringstream out;
        BudgetCycle(in, out);
        ASSERT_EQUAL(expected, out.str());
    }
    {
        stringstream in(R"(2
Earn 2000-01-02 2000-01-02 200000000
ComputeIncome 2000-01-02 2001-01-02)");
        string expected(R"(200000000)");
        stringstream out;
        BudgetCycle(in, out);
        ASSERT_EQUAL(expected, out.str());
    }
}

void TestSpeed()
{
    static const int ITERATIONS = 500;
    static const int DAYS = 20;
    Budget budget;
    Date d(2001, 1, 1), d1 = d;

    LOG_DURATION("Budget time for " + to_string(ITERATIONS) + " iterations by " + to_string(DAYS) + " days");

    for (int i = 0; i < ITERATIONS; ++i)
    {
        budget.Earn(d1, d, 5);

        for (int j = 0; j < DAYS; ++j)
        {
            d = d.NextDay();
        }
    }

    d = d1;

    for (int i = 0; i < ITERATIONS; ++i)
    {
        budget.Income(d1, d);

        for (int j = 0; j < DAYS; ++j)
        {
            d = d.NextDay();
        }
    }
}

void TestBudget()
{
    TestRunner tr;
    RUN_TEST(tr, TestEarnDay);
    RUN_TEST(tr, TestEarnYear);
    RUN_TEST(tr, TestPayTax);
    RUN_TEST(tr, TestCycle);
    RUN_TEST(tr, TestSpeed);
}

/**
 * @brief      Main function with budget processing via
 * standard input and output.
 *
 * @return     Exit code.
 */
int main()
{
    // TestBudget();
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    BudgetCycle(cin, cout);
    return 0;
}