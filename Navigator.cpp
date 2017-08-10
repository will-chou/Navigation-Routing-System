#include "provided.h"
#include "support.h"
#include <string>
#include <vector>
#include <list>
#include <iostream>
using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
    struct Node {
        Node(GeoCoord geo) {
            gc = geo;
            parent = nullptr;
            f = 0;
            g = 0;
            h = 0;
            streetname = "";
        }
        
        Node* parent;
        GeoCoord gc;
        string streetname;
        double f, g, h;
    };
    
    MapLoader ml;
    AttractionMapper am;
    SegmentMapper sm;
    void insertNode(list<Node*>& nodeList, Node* n) const;
    void loadNavigationVector(vector<NavSegment>& navVector, Node*& endNode, GeoCoord start, GeoCoord end) const;
    string turnDirection(GeoSegment g1, GeoSegment g2) const;
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    if(!ml.load(mapFile)) {
        return false;
    }
    
    am.init(ml);
    
    sm.init(ml);
    
    return true;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    list<Node*> open;
    list<Node*> closed;
    
    GeoCoord startGeoCoord;
    if(!am.getGeoCoord(start, startGeoCoord)) {
        return NAV_BAD_SOURCE;
    }
    
    GeoCoord endGeoCoord;
    if(!am.getGeoCoord(end, endGeoCoord)) {
        return NAV_BAD_DESTINATION;
    }
    
    vector<StreetSegment> endCoords = sm.getSegments(endGeoCoord);
    GeoCoord end1 = endCoords[0].segment.start;
    GeoCoord end2 = endCoords[0].segment.end;
    
    Node* startNode = new Node(startGeoCoord);
    startNode->parent = nullptr;
    
    vector<StreetSegment> startSegments = sm.getSegments(startGeoCoord);
    startNode->streetname = startSegments[0].streetName;
    
    open.push_back(startNode);
    
    while(open.size() > 0) {
        Node* q = open.front();
        open.pop_front();
        
        vector<StreetSegment> segs = sm.getSegments(q->gc);
        
        vector<Node*> successors;
        
        Node* par = q;
        
        //generate successors
        for(int i = 0; i < segs.size(); i++) {
            if(!(q->gc == segs[i].segment.start)) {
                Node* n = new Node(segs[i].segment.start);
                n->parent = par;
                n->streetname = segs[i].streetName;
                successors.push_back(n);
            }
            if(!(q->gc == segs[i].segment.end)) {
                Node* n = new Node(segs[i].segment.end);
                n->parent = par;
                n->streetname = segs[i].streetName;
                successors.push_back(n);
            }
        }
        
        for(int i = 0; i < successors.size(); i++) {
            //if found end destination
            if(successors[i]->gc == end1 || successors[i]->gc == end2) {
                //load directions vector
                loadNavigationVector(directions, successors[i], startGeoCoord, endGeoCoord);
                
                //free the memory allocated in here
                list<Node*>::iterator it = open.begin();
                while(it != open.end()) {
                    delete (*it);
                    (*it) = nullptr;
                    it = open.erase(it);
                }

                closed.push_back(q);

                list<Node*>::iterator it2 = closed.begin();
                while(it2 != closed.end()) {
                    if((*it2) != nullptr) {
                        delete (*it2);
                        (*it2) = nullptr;
                        it2 = closed.erase(it2);
                    }
                    else {
                        it2++;
                    }
                }
                
                for(int j = i; j < successors.size(); j++) {
                    delete successors[j];
                    successors[j] = nullptr;
                }
                
                return NAV_SUCCESS;
            }
            
            //calculate heuristics
            successors[i]->g = q->g + distanceEarthMiles(q->gc, successors[i]->gc);
            
            successors[i]->h = distanceEarthMiles(successors[i]->gc, endGeoCoord);
            
            successors[i]->f = successors[i]->g + successors[i]->h;
            
            bool shouldAdd = true;
            
            //check if Node with same coordinate is in open and closed lists with lower f
            list<Node*>::iterator it = open.begin();
            while(it != open.end()) {
                if((*it)->gc == successors[i]->gc) {
                    if((*it)->f < successors[i]->f) {
                        shouldAdd = false;
                        break;
                    }
                }
                it++;
            }
            
            list<Node*>::iterator it2 = closed.begin();
            if(shouldAdd) {
                while(it2 != closed.end()) {
                    if((*it2)->gc == successors[i]->gc) {
                        if((*it2)->f < successors[i]->f) {
                            shouldAdd = false;
                            break;
                        }
                    }
                    it2++;
                }
            }
            
            //insert node into open list
            if(shouldAdd)
                insertNode(open, successors[i]);
            else {
                delete successors[i];
                successors[i] = nullptr;
            }
        }
        
        closed.push_back(q);
    }
    
    //deallocate memory
    list<Node*>::iterator it = open.begin();
    while(it != open.end()) {
        delete (*it);
        (*it) = nullptr;
        it = open.erase(it);
    }
    
    list<Node*>::iterator it2 = closed.begin();
    while(it2 != closed.end()) {
        if((*it2) != nullptr) {
            delete (*it2);
            (*it2) = nullptr;
            it2 = closed.erase(it2);
        }
        else {
            it2++;
        }
    }

    
	return NAV_NO_ROUTE;
}

void NavigatorImpl::loadNavigationVector(vector<NavSegment>& navVector, Node*& endNode, GeoCoord start, GeoCoord end) const {
    vector<Node*> tempVector;
    
    Node* temp = endNode;
    while(temp != nullptr) {
        tempVector.push_back(temp);
        temp = temp->parent;
    }
    
    //reverse the vector to go from start to end
    vector<Node*> nodeVector;
    for(int i = (int)tempVector.size() - 1; i >= 0; i--) {
        nodeVector.push_back(tempVector[i]);
    }
    
    string dir;
    string street;
    double dist;
    
    for(int i = 0; i < nodeVector.size() - 1; i++) {
        //if streetname changes, add turn NavSegment
        if(nodeVector[i]->streetname != nodeVector[i + 1]->streetname) {
            GeoCoord coord;
            if(i - 1 < 0) {
                coord = start;
            }
            else {
                coord = nodeVector[i - 1]->gc;
            }
            GeoSegment g1(coord, nodeVector[i]->gc);
            
            if(i + 2 >= nodeVector.size()) {
                coord = end;
            }
            else {
                coord = nodeVector[i + 2]->gc;
            }
            GeoSegment g2(nodeVector[i + 1]->gc, coord);
            
            navVector.push_back(NavSegment(turnDirection(g1, g2), nodeVector[i + 1]->streetname));
        }
        //add proceed NavSegment
        GeoSegment gs(nodeVector[i]->gc, nodeVector[i + 1]->gc);
        dir = directionOfLine(gs);
        street = nodeVector[i + 1]->streetname;
        dist = distanceEarthMiles(nodeVector[i]->gc, nodeVector[i + 1]->gc);
        
        navVector.push_back(NavSegment(dir, street, dist, gs));
    }
    
    //add last proceed NavSegment
    GeoSegment last(nodeVector[nodeVector.size() - 1]->gc, end);
    dir = directionOfLine(last);
    street = nodeVector[nodeVector.size() - 1]->streetname;
    dist = distanceEarthMiles(nodeVector[nodeVector.size() - 1]->gc, end);
    
    navVector.push_back(NavSegment(dir, street, dist, last));
    
}

//insert node in increasing order of f (lowest f is in the front)
void NavigatorImpl::insertNode(list<Node*>& nodeList, Node* n) const {
    if(nodeList.size() == 0) {
        nodeList.push_back(n);
        return;
    }
    
    list<Node*>::iterator it = nodeList.begin();
    
    while(it != nodeList.end()) {
        if(n->f < (*it)->f) {
            nodeList.insert(it, n);
            return;
        }
        it++;
    }
    
    nodeList.push_back(n);
}

string NavigatorImpl::turnDirection(GeoSegment g1, GeoSegment g2) const {
    if(angleBetween2Lines(g1, g2) < 180) {
        return "left";
    }
    return "right";
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
