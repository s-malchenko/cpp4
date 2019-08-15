#include "test_runner.h"
#include "profile.h"

#include <iomanip>
#include <iostream>
#include <ctime>
#include <map>
#include <numeric>
#include <ostream>
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
        tm* t = localtime(&ts);
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

/**
 * @brief      Class for budget.
 */
class Budget
{
public:
    void Earn(Date from, Date to, unsigned int value)
    {
        double dailyEarning = value / (ComputeDaysDiff(from, to) + 1.0);

        for (Date d = from; d <= to; d = d.NextDay())
        {
            _earnings[d] += dailyEarning;
        }
    }

    double Income(Date from, Date to)
    {
        return accumulate(_earnings.lower_bound(from),
                          _earnings.upper_bound(to),
                          0.0,
                          [](double a, const auto &b) { return a + b.second; });
    }

    void PayTax(Date from, Date to)
    {

    }

protected:
    std::map<Date, double> _earnings;
};

void BudgetCycle(std::istream &in, std::ostream &out)
{

}

/**
 * @brief      Functions for testing Budget class.
 */
void TestEarn()
{
    ASSERT(false);
}

void TestBudget()
{
    TestRunner tr;
    RUN_TEST(tr, TestEarn);
}

/**
 * @brief      Main function with budget processing via
 * standard input and output.
 *
 * @return     Exit code.
 */
int main()
{
    TestBudget();
    BudgetCycle(cin, cout);
    return 0;
}