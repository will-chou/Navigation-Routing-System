#include "provided.h"
#include "MyMap.h"
#include "support.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    MyMap<GeoCoord, vector<StreetSegment>> mymap;
    void mapSegment(GeoCoord gc, StreetSegment s);
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    for(int i = 0; i < ml.getNumSegments(); i++) {
        StreetSegment s;
        ml.getSegment(i, s);
        
        mapSegment(s.segment.start, s);
        
        mapSegment(s.segment.end, s);
        
        for(int j = 0; j < s.attractions.size(); j++) {
            mapSegment(s.attractions[j].geocoordinates, s);
        }
    }
}

void SegmentMapperImpl::mapSegment(GeoCoord gc, StreetSegment s) {

    vector<StreetSegment>* segs = mymap.find(gc);
    
    if(segs == nullptr) {
        vector<StreetSegment> segments;
        segments.push_back(s);
        mymap.associate(gc, segments);
    }
    else {
        segs->push_back(s);
    }
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    const vector<StreetSegment>* segments = mymap.find(gc);
    
    if(segments == nullptr) {
        vector<StreetSegment> emptySegs;
        return emptySegs;
    }
    
    return *segments;
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
