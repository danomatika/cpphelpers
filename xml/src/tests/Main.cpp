
#include <xmlframework/xmlframework.h>
#include <iostream>

using namespace std;
using namespace xml;

class TestObject : public XmlObject
{
	public:
		TestObject() : XmlObject("object") {}
    
    protected:
		bool readXml(TiXmlElement* e)
        {
        	cout << "	" << e->ValueStr()
            	 << " \"" << Xml::getAttrString(e, "name", "unknown") << "\" : "
                 << e->GetText() << endl;
        
        	return true;
        }
};

class XmlProcessor : public XmlObject
{
	public:
    	// set the object's element name
    	XmlProcessor() : XmlObject("xmltest") {}
        
    protected:
    
    	// derived callback, called when loading xml data for the object
    	bool readXml(TiXmlElement* e)
        {
        	TiXmlElement* child = e->FirstChildElement();
        	while(child != NULL)
    		{
                if(child->ValueStr() == "argtest")
                {
                	cout << "argtest" << endl;
                    
                    bool boolValT = Xml::getAttrBool(child, "boolT", false);
                    bool boolValF = Xml::getAttrBool(child, "boolF", true);
                    uint8_t byteVal = Xml::getAttrByte(child, "byte");
                    unsigned int uintVal = Xml::getAttrUInt(child, "uint");
                    int intVal = Xml::getAttrInt(child, "int");
                    float floatVal = Xml::getAttrFloat(child, "float");
                    double doubleVal = Xml::getAttrDouble(child, "double");
                    
                    cout << "	boolT:  " << boolValT << endl
                         << "	boolF:  " << boolValF << endl
                         << "	byte:   " << (int) byteVal << endl
                         << "	uint:   " << uintVal << endl
                         << "	int:    " << intVal << endl
                         << "	float:  " << floatVal << endl
                         << "	double: " << doubleVal << endl;
                }
                else if(child->ValueStr() == "elementtest")
                {
                    cout << "elementtest" << endl;
                
                    TiXmlElement* subchild = child->FirstChildElement();
                    while(subchild != NULL)
                    {
                        cout << "	" << subchild->ValueStr() << ": \"" 
                        	 << subchild->GetText() << "\"" << endl;
                        subchild = subchild->NextSiblingElement();
                    }
                }
                else if(child->ValueStr() == "objecttest")
                {
                	cout << "objecttest" << endl;
                    
                	TiXmlElement* subchild = child->FirstChildElement();
                    while(subchild != NULL)
                    {
                		TestObject o;
                    	o.loadXml(child->FirstChildElement());
                        subchild = subchild->NextSiblingElement();
                    }
                }
                
                child = child->NextSiblingElement();
        	}
        	return true;
        }
};

int main(int argc, char *argv[])
{
    cout << "Starting xmlframework test" << endl;
    
    // load xml file through object derived from XmlObject
    XmlProcessor processor;
    processor.loadXmlFile("../data/test.xml");
	
    cout << "Exited cleanly" << endl;

    return 0;
}
