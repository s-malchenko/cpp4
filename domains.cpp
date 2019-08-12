#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

bool Subdomain(string_view subdomain, string_view domain)
{
    auto i = subdomain.size();
    auto j = domain.size();

    if (j == 0)
    {
        return false;
    }

    while (i > 0 && j > 0)
    {
        if (subdomain[--i] != domain[--j])
        {
            return false;
        }
    }

    return j == 0 && (i == 0 || subdomain[i - 1] == '.');
}

vector<string> ReadDomains(istream &in)
{
    size_t count;
    in >> count;

    vector<string> domains;
    for (size_t i = 0; i < count; ++i)
    {
        string domain;
        in >> domain;
        domains.push_back(move(domain));
    }

    return domains;
}

bool reversedCmpLess(const string_view &a, const string_view &b)
{
    size_t minLength = min(a.size(), b.size());
    auto itA = rbegin(a);
    auto itB = rbegin(b);

    for (size_t i = 0; i < minLength; ++i, ++itA, ++itB)
    {
        if (*itA != *itB)
        {
            return *itA < *itB;
        }
    }

    return a.size() < b.size();
}

void SortByReversed(vector<string> &v)
{
    sort(v.begin(), v.end(), reversedCmpLess);
}
bool DomainBanned(const string &domain, const vector<string> &banned)
{
    vector<string_view> subdomains;

    if (domain.empty() || banned.empty())
    {
        return false;
    }

    for (size_t i = domain.size(); i > 0; --i)
    {
        if (domain[i] == '.')
        {
            subdomains.push_back({&domain[i + 1], domain.size() - i - 1});
        }
    }

    vector<string>::const_iterator first, last;
    last = upper_bound(banned.begin(), banned.end(), domain, reversedCmpLess);

    if (!subdomains.empty())
    {
        first = lower_bound(banned.begin(), banned.end(), subdomains[0], reversedCmpLess);
    }
    else
    {
        first = last;
    }

    for (const auto &i : subdomains)
    {
        first = lower_bound(first, last, i, reversedCmpLess);

        if (*first == i)
        {
            return true;
        }
    }

    return *prev(last) == domain;
}

void mainCycle(istream &in, ostream &out)
{
    const vector<string> banned_domains = [&in]{
        auto v = ReadDomains(in);
        SortByReversed(v);
        return v;
    }();
    const vector<string> domains_to_check = ReadDomains(in);

    for (const auto &i : domains_to_check)
    {
        if (DomainBanned(i, banned_domains))
        {
            out << "Bad" << endl;
        }
        else
        {
            out << "Good" << endl;
        }
    }
}

void TestSubdomain()
{
    ASSERT_EQUAL(Subdomain("fhdjf.ya.ru", "ya.ru"), true);
    ASSERT_EQUAL(Subdomain("fhdjf.ya.ru", "yg.ru"), false);
    ASSERT_EQUAL(Subdomain("ya.ru", "ya.ru"), true);
    ASSERT_EQUAL(Subdomain("ya.ru", "ru"), true);
    ASSERT_EQUAL(Subdomain("ru.ya", "ru"), false);
    ASSERT_EQUAL(Subdomain("fhdjfya.ru", "ya.ru"), false);
    ASSERT_EQUAL(Subdomain("wow.such.domain.ru", "such.domain.ru"), true);
}

void testSorting(vector<string> source, const vector<string> &expected)
{
    SortByReversed(source);
    ASSERT_EQUAL(source, expected);
}

void TestSortByReversed()
{
    testSorting({"a"}, {"a"});
    testSorting({"ac", "ab"}, {"ab", "ac"});
    testSorting({"abcd", "abcdef", "fxy"}, {"abcd", "abcdef", "fxy"});
    testSorting({"fxy", "abcd", "abcdef"}, {"abcd", "abcdef", "fxy"});
}

void TestDomainBanned()
{
    vector<string> banned = {"ya.ru", "mail.mail.com", "tv", "ru.tv", "", "mail.ru.tv"};
    SortByReversed(banned);

    ASSERT_EQUAL(DomainBanned("mail.ya.ru", banned), true);
    ASSERT_EQUAL(DomainBanned("mail.mail.com", banned), true);
    ASSERT_EQUAL(DomainBanned("bazooka.tv", banned), true);
    ASSERT_EQUAL(DomainBanned("wow.mail.mail.com", banned), true);
    ASSERT_EQUAL(DomainBanned("wow.mail.mail.ru.tv", banned), true);
    ASSERT_EQUAL(DomainBanned("wow.mail.tu.tv", banned), true);

    ASSERT_EQUAL(DomainBanned("mail.ya.rf", banned), false);
    ASSERT_EQUAL(DomainBanned("mail.muil.com", banned), false);
    ASSERT_EQUAL(DomainBanned("bazooka.ttv", banned), false);
    ASSERT_EQUAL(DomainBanned("wow.ru.mail.com", banned), false);
    ASSERT_EQUAL(DomainBanned("", banned), false);

    vector<string> bannedEmpty;
    ASSERT_EQUAL(DomainBanned("wow.mail.mail.com", bannedEmpty), false);
}

void TestMainCycle()
{
    stringstream in("4\nya.ru\nmaps.me\nm.ya.ru\ncom\n7\nya.ru\nya.com\nm.maps.me\nmoscow.m.ya.ru\nmaps.com\nmaps.ru\nya.ya\n"), out;
    mainCycle(in, out);
    ASSERT_EQUAL(out.str(), "Bad\nBad\nBad\nBad\nBad\nGood\nGood\n");
    out = stringstream("");
    in = stringstream("0\n7\nya.ru\nya.com\nm.maps.me\nmoscow.m.ya.ru\nmaps.com\nmaps.ru\nya.ya\n");
    mainCycle(in, out);
    ASSERT_EQUAL(out.str(), "Good\nGood\nGood\nGood\nGood\nGood\nGood\n");
    out = stringstream("");
    in = stringstream("0\n0");
    mainCycle(in, out);
    ASSERT_EQUAL(out.str(), "");
}

int main()
{
    {
        TestRunner tr;
        RUN_TEST(tr,TestSubdomain);
        RUN_TEST(tr,TestSortByReversed);
        RUN_TEST(tr,TestDomainBanned);
        RUN_TEST(tr,TestMainCycle);
    }

    mainCycle(cin, cout);
    return 0;
}
