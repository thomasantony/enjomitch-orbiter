#include "Tokenizer.hpp"

#include <sstream>
#include <fstream>
#include <istream>

using namespace std;
using namespace EnjoLib;

Tokenizer::Tokenizer(){}
Tokenizer::~Tokenizer(){}

vector<string> Tokenizer::Tokenize(const string & line, char token) const
{
    vector<string> ret;
    stringstream ss(line);
    string s;

    while (getline(ss, s, token))
    {
        ret.push_back(s);
    }
    return ret;
}

vector<string> Tokenizer::GetLines(const string & fileName, bool excludeFirstLine) const
{
    ifstream file(fileName.c_str());
    return GetLines(file, excludeFirstLine);
}

vector<string> Tokenizer::GetLines(istream & is, bool excludeFirstLine) const
{
    string line;
    vector<string> lines;
    if (excludeFirstLine)
        getline(is, line);
    while (getline(is, line))
    {
        lines.push_back(line);
    }
    return lines;
}

vector<string> Tokenizer::GetLinesStr(const string & strData, bool excludeFirstLine) const
{
    return Tokenize(strData, '\n');
}

void Tokenizer::WorkOnLines(const string & fileName, IWorksOnLine & worker, bool excludeFirstLine) const
{
    ifstream file(fileName.c_str());
    return WorkOnLines(file, worker, excludeFirstLine);
}

void Tokenizer::WorkOnLines(istream & is, IWorksOnLine & worker, bool excludeFirstLine) const
{
    string line;
    vector<string> lines;
    if (excludeFirstLine)
        getline(is, line);
    while (getline(is, line))
    {
        worker.Work(line);
    }
}
