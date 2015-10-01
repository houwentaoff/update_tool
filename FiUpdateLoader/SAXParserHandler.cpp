#include "SAXParserHandler.h"

XmlParserEngine::XmlParserEngine()
{
}
XmlParserEngine::~XmlParserEngine()
{
}
int XmlParserEngine::load(std::string name)
{
	
    filename = name;
	
	return 1;
}


