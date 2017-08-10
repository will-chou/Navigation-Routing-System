#include "provided.h"
#include "MyMap.h"
#include <cctype>
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    MyMap<string, GeoCoord> mymap;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    //for every segment
    for(int i = 0; i < ml.getNumSegments(); i++) {
        StreetSegment s;
        ml.getSegment(i, s);
        
        //if there are attractions, make lower case and associate
        if(s.attractions.size() > 0) {
            for(int j = 0; j < s.attractions.size(); j++) {
                string lowerCaseAttractionName;
                for(int k = 0; k < s.attractions[j].name.length(); k++) {
                    lowerCaseAttractionName += tolower(s.attractions[j].name[k]);
                }
                mymap.associate(lowerCaseAttractionName, s.attractions[j].geocoordinates);
            }
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    for(int i = 0; i < attraction.length(); i++) {
        attraction[i] = tolower(attraction[i]);
    }
    
    const GeoCoord* temp = mymap.find(attraction);
    if(temp == nullptr) {
        return false;
    }
    
    gc = *temp;
    return true;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
