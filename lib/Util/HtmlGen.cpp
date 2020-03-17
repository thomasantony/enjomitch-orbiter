#include "HtmlGen.hpp"
#include <sstream>

using namespace EnjoLib;

HtmlGen::HtmlGen(){}
HtmlGen::~HtmlGen(){}

std::string HtmlGen::GenHeader(const std::string & title, const std::string & bodyParams) const
{
    std::ostringstream html;

    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "<meta charset='UTF-8'>\n";
    html << "<title>" << title << "</title>\n";
    html << "</head>\n";
    html << "<body " << bodyParams << ">\n";

    return html.str();
}
std::string HtmlGen::GenFooter() const
{
    std::ostringstream html;

    html << "</body>\n";
    html << "</html>\n";

    return html.str();
}

std::string HtmlGen::GenScriptSrc(const std::string & scriptPath) const
{
    std::ostringstream html;
    html << "<script src='" << scriptPath << "'></script>\n";
    return html.str();
}

std::string HtmlGen::GenScriptListing(const std::string & scriptListing) const
{
    std::ostringstream html;
    html << "<script>\n" << scriptListing << "\n</script>\n";
    return html.str();
}

std::string HtmlGen::GenSpan(const std::string & content) const
{
    return GenNormalTagWithContent("span", content);
}

std::string HtmlGen::GenNormalTagWithContent(const std::string & tag, const std::string & content) const
{
    return "<" + tag + ">" + content + "</" + tag + ">\n";
}
