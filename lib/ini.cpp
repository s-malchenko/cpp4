#include "ini.h"

#include <utility>
#include <string_view>

namespace Ini
{

Section &Document::AddSection(string name)
{
    return sections.insert(make_pair(move(name), Section{})).first->second;
}

const Section &Document::GetSection(const string &name) const
{
    auto it = sections.find(name);

    if (it == sections.end())
    {
        throw out_of_range("unknown section");
    }

    return it->second;
}

size_t Document::SectionCount() const
{
    return sections.size();
}

Document Load(istream &input)
{
    Document result;
    Section *section = nullptr;

    while (input)
    {
        string line;
        getline(input, line);

        if (line.empty())
        {
            continue;
        }
        else if (line.front() == '[' && line.size() > 2)
        {
            section = &result.AddSection(line.substr(1, line.size() - 2));
        }
        else if (section == nullptr)
        {
            continue;
        }
        else
        {
            auto delim = line.find('=');

            if (delim != 0 && delim != string::npos)
            {
                string name = move(line);
                string value = name.substr(delim + 1, string::npos);
                name.erase(delim, string::npos);
                section->insert(make_pair(move(name), move(value)));
            }
        }
    }

    return result;
}

}
