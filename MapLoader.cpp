
#include "provided.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    vector<StreetSegment> segVector;
};

MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream infile(mapFile);
    if(!infile) {
        return false;
    }
    
    while(infile) {
        //get streetname
        string streetName;
        if(!getline(infile, streetName)) {
            return true;
        }
        
        string s1, s2, e1, e2;
        
        getline(infile, s1, ',');
        if(infile.peek() == ' ') {
            infile.ignore(10000, ' ');
        }
        
        getline(infile, s2, ' ');
        
        getline(infile, e1, ',');
        if(infile.peek() == ' ') {
            infile.ignore(10000, ' ');
        }
        
        getline(infile, e2);
        
        GeoSegment gs(GeoCoord(s1, s2), GeoCoord(e1, e2));
        
        //get attractions and store in attraction vector
        vector<Attraction> attractionVector;
        
        int numAttractions;
        infile >> numAttractions;
        infile.ignore(10000, '\n');
        
        while(numAttractions > 0) {
            //string attraction;
            string attractionName;
            getline(infile, attractionName, '|');
            
            string c1, c2;
            
            getline(infile, c1, ',');
            if(infile.peek() == ' ') {
                infile.ignore(10000, ' ');
            }
            
            getline(infile, c2);
            
            Attraction a;
            a.name = attractionName;
            a.geocoordinates = GeoCoord(c1, c2);
            
            attractionVector.push_back(a);
            
            numAttractions--;
        }
        
        //create street segment vector and put it into vector
        StreetSegment ss;
        ss.streetName = streetName;
        ss.segment = gs;
        ss.attractions = attractionVector;
        
        segVector.push_back(ss);
    }
    
    return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
    return segVector.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if(segNum >= segVector.size()) {
        return false;
    }
    
    seg = segVector[segNum];
    
    return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
   return m_impl->getSegment(segNum, seg);
}
