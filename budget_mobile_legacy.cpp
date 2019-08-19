#include "test_runner.h"

#include <array>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <utility>

using namespace std;

pair<string_view, optional<string_view>> SplitTwoStrict(string_view s, string_view delimiter = " ")
{
    const size_t pos = s.find(delimiter);
    if (pos == s.npos)
    {
        return {s, nullopt};
    }
    else
    {
        return {s.substr(0, pos), s.substr(pos + delimiter.length())};
    }
}

pair<string_view, string_view> SplitTwo(string_view s, string_view delimiter = " ")
{
    const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
    return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view &s, string_view delimiter = " ")
{
    const auto [lhs, rhs] = SplitTwo(s, delimiter);
    s = rhs;
    return lhs;
}

int ConvertToInt(string_view str)
{
    // use std::from_chars when available to git rid of string copy
    size_t pos;
    const int result = stoi(string(str), &pos);
    if (pos != str.length())
    {
        std::stringstream error;
        error << "string " << str << " contains " << (str.length() - pos) << " trailing chars";
        throw invalid_argument(error.str());
    }
    return result;
}

template <typename Number>
void ValidateBounds(Number number_to_check, Number min_value, Number max_value)
{
    if (number_to_check < min_value || number_to_check > max_value)
    {
        std::stringstream error;
        error << number_to_check << " is out of [" << min_value << ", " << max_value << "]";
        throw out_of_range(error.str());
    }
}

class Date
{
public:
    static Date FromString(string_view str)
    {
        const int year = ConvertToInt(ReadToken(str, "-"));
        const int month = ConvertToInt(ReadToken(str, "-"));
        ValidateBounds(month, 1, 12);
        const int day = ConvertToInt(str);
        ValidateBounds(day, 1, 31);
        return {year, month, day};
    }

    // Weird legacy, can't wait for std::chrono::year_month_day
    time_t AsTimestamp() const
    {
        std::tm t;
        t.tm_sec  = 0;
        t.tm_min  = 0;
        t.tm_hour = 0;
        t.tm_mday = day_;
        t.tm_mon  = month_ - 1;
        t.tm_year = year_ - 1900;
        t.tm_isdst = 0;
        return mktime(&t);
    }

private:
    int year_;
    int month_;
    int day_;

    Date(int year, int month, int day)
        : year_(year), month_(month), day_(day)
    {}
};

int ComputeDaysDiff(const Date &date_to, const Date &date_from)
{
    const time_t timestamp_to = date_to.AsTimestamp();
    const time_t timestamp_from = date_from.AsTimestamp();
    static constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
    return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

static const Date START_DATE = Date::FromString("2000-01-01");
static const Date END_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = ComputeDaysDiff(END_DATE, START_DATE);
static const size_t DAY_COUNT_P2 = 1 << 16;
static const size_t VERTEX_COUNT = DAY_COUNT_P2 * 2;

size_t ComputeDayIndex(const Date &date)
{
    return ComputeDaysDiff(date, START_DATE);
}

struct MoneyState
{
    double earned = 0.0;
    double spent = 0.0;

    MoneyState &ApplyTax(double coeff)
    {
        earned *= coeff;
        return *this;
    }

    MoneyState &operator*=(double coeff)
    {
        earned *= coeff;
        spent *= coeff;
        return *this;
    }

    MoneyState &operator+=(const MoneyState &other)
    {
        earned += other.earned;
        spent += other.spent;
        return *this;
    }

    double Income()
    {
        return earned - spent;
    }
};

ostream &operator<<(ostream &os, const MoneyState &m)
{
    return os << "[" << m.earned << ":" << m.spent << "]";
}

MoneyState operator+(const MoneyState &lhs, const MoneyState &rhs)
{
    return {lhs.earned + rhs.earned, lhs.spent + rhs.spent};
}

MoneyState operator*(const MoneyState &money, double coeff)
{
    return {money.earned * coeff, money.spent * coeff};
}

MoneyState operator*(double coeff, const MoneyState &money)
{
    return money * coeff;
}

array<MoneyState, VERTEX_COUNT> tree_values, tree_add;
array<double, VERTEX_COUNT> tree_factor;

void Init()
{
    tree_add.fill({});
    tree_values.fill({});
    tree_factor.fill(1);
}

void Push(size_t v, size_t l, size_t r)
{
    for (size_t w = v * 2; w <= v * 2 + 1; ++w)
    {
        if (w < VERTEX_COUNT)
        {
            tree_factor[w] *= tree_factor[v];
            (tree_add[w].ApplyTax(tree_factor[v])) += tree_add[v];
            (tree_values[w].ApplyTax(tree_factor[v])) += tree_add[v] * ((r - l) / 2);
        }
    }

    tree_factor[v] = 1;
    tree_add[v] = {};
}

MoneyState ComputeSum(size_t v, size_t l, size_t r, size_t ql, size_t qr)
{
    if (v >= VERTEX_COUNT || qr <= l || r <= ql)
    {
        return {};
    }
    Push(v, l, r);
    if (ql <= l && r <= qr)
    {
        return tree_values[v];
    }
    return ComputeSum(v * 2, l, (l + r) / 2, ql, qr)
           + ComputeSum(v * 2 + 1, (l + r) / 2, r, ql, qr);
}

void Add(size_t v, size_t l, size_t r, size_t ql, size_t qr, double value)
{
    if (v >= VERTEX_COUNT || qr <= l || r <= ql)
    {
        return;
    }
    Push(v, l, r);
    if (ql <= l && r <= qr)
    {
        tree_add[v].earned += value;
        tree_values[v].earned += value * (r - l);
        return;
    }
    Add(v * 2, l, (l + r) / 2, ql, qr, value);
    Add(v * 2 + 1, (l + r) / 2, r, ql, qr, value);
    tree_values[v] =
        (v * 2 < VERTEX_COUNT ? tree_values[v * 2] : MoneyState{})
        + (v * 2 + 1 < VERTEX_COUNT ? tree_values[v * 2 + 1] : MoneyState{});
}

void Subtract(size_t v, size_t l, size_t r, size_t ql, size_t qr, double value)
{
    if (v >= VERTEX_COUNT || qr <= l || r <= ql)
    {
        return;
    }
    Push(v, l, r);
    if (ql <= l && r <= qr)
    {
        tree_add[v].spent += value;
        tree_values[v].spent += value * (r - l);
        return;
    }
    Subtract(v * 2, l, (l + r) / 2, ql, qr, value);
    Subtract(v * 2 + 1, (l + r) / 2, r, ql, qr, value);
    tree_values[v] =
        (v * 2 < VERTEX_COUNT ? tree_values[v * 2] : MoneyState{})
        + (v * 2 + 1 < VERTEX_COUNT ? tree_values[v * 2 + 1] : MoneyState{});
}

void Multiply(size_t v, size_t l, size_t r, size_t ql, size_t qr, int percentage = 13)
{
    double factor = 1.0 - percentage / 100.0;

    if (v >= VERTEX_COUNT || qr <= l || r <= ql)
    {
        return;
    }
    Push(v, l, r);
    if (ql <= l && r <= qr)
    {
        tree_factor[v] *= factor;
        tree_add[v].ApplyTax(factor);
        tree_values[v].ApplyTax(factor);
        return;
    }
    Multiply(v * 2, l, (l + r) / 2, ql, qr, percentage);
    Multiply(v * 2 + 1, (l + r) / 2, r, ql, qr, percentage);
    tree_values[v] =
        (v * 2 < VERTEX_COUNT ? tree_values[v * 2] : MoneyState{})
        + (v * 2 + 1 < VERTEX_COUNT ? tree_values[v * 2 + 1] : MoneyState{});
}


void BudgetCycle(istream &in = cin, ostream &out = cout)
{
    assert(DAY_COUNT <= DAY_COUNT_P2 && DAY_COUNT_P2 < DAY_COUNT * 2);

    out.precision(25);
    Init();

    int q;
    in >> q;

    for (int i = 0; i < q; ++i)
    {
        string query_type;
        in >> query_type;

        string date_from_str, date_to_str;
        in >> date_from_str >> date_to_str;

        auto idx_from = ComputeDayIndex(Date::FromString(date_from_str));
        auto idx_to = ComputeDayIndex(Date::FromString(date_to_str)) + 1;

        if (query_type == "ComputeIncome")
        {
            out << ComputeSum(1, 0, DAY_COUNT_P2, idx_from, idx_to).Income() << endl;
        }
        else if (query_type == "PayTax")
        {
            int percentage;
            in >> percentage;
            Multiply(1, 0, DAY_COUNT_P2, idx_from, idx_to, percentage);
        }
        else if (query_type == "Earn")
        {
            double value;
            in >> value;
            Add(1, 0, DAY_COUNT_P2, idx_from, idx_to, value / (idx_to - idx_from));
        }
        else if (query_type == "Spend")
        {
            double value;
            in >> value;
            Subtract(1, 0, DAY_COUNT_P2, idx_from, idx_to, value / (idx_to - idx_from));
        }
    }
}

void TestCycle()
{
    {
        stringstream in(R"(8
Earn 2000-01-02 2000-01-06 20
ComputeIncome 2000-01-01 2001-01-01
PayTax 2000-01-02 2000-01-03 13
ComputeIncome 2000-01-01 2001-01-01
Spend 2000-12-30 2001-01-02 14
ComputeIncome 2000-01-01 2001-01-01
PayTax 2000-12-30 2000-12-30 13
ComputeIncome 2000-01-01 2001-01-01)");
        string expected(R"(20
18.96000000000000085265128
8.460000000000000852651283
8.460000000000000852651283
)");
        stringstream out;
        BudgetCycle(in, out);
        ASSERT_EQUAL(expected, out.str());
    }
}

void Test()
{
    TestRunner tr;
    RUN_TEST(tr, TestCycle);
}

int main()
{
    Test();
    BudgetCycle();
    return 0;
}
