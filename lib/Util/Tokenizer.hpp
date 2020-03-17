#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <vector>
#include <string>
#include <iosfwd>

namespace EnjoLib
{

class IWorksOnLine
{
public:
    virtual ~IWorksOnLine(){}
    virtual void Work(const std::string & line) = 0;
};

class Tokenizer
{
    public:
        Tokenizer();
        virtual ~Tokenizer();
        std::vector<std::string> Tokenize(const std::string & line, char token = ' ') const;
        std::vector<std::string> GetLines(const std::string & fileName, bool excludeFirstLine = false) const;
        std::vector<std::string> GetLinesStr(const std::string & strData, bool excludeFirstLine = false) const;
        std::vector<std::string> GetLines(std::istream & is, bool excludeFirstLine = false) const;
        void WorkOnLines(const std::string & fileName, IWorksOnLine & worker, bool excludeFirstLine = false) const;
        void WorkOnLines(std::istream & is, IWorksOnLine & worker, bool excludeFirstLine = false) const;
    protected:
    private:
};
}
#endif // TOKENIZE_H
