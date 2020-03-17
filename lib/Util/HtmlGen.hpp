#ifndef HTMLGEN_H
#define HTMLGEN_H

#include <string>

namespace EnjoLib
{
class HtmlGen
{
    public:
        HtmlGen();
        virtual ~HtmlGen();

        std::string GenNormalTagWithContent(const std::string & tag, const std::string & content) const;

        std::string GenHeader(const std::string & title, const std::string & bodyParams = "") const;
        std::string GenSpan(const std::string & content) const;
        std::string GenFooter() const;
        std::string GenScriptSrc(const std::string & scriptPath) const;
        std::string GenScriptListing(const std::string & scriptListing) const;

    protected:

    private:

};
}


#endif // HTMLGEN_H
