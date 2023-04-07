#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
//#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>


struct preferences
{
    std::string font;                      // debug level
    void load(const std::string &filename);
    void save(const std::string &filename);
};
