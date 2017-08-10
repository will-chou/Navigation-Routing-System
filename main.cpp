//#include "provided.h"
//#include "MyMap.h"
//#include <iostream>
//#include <string>
//#include <algorithm>
//#include <cmath>
//#include <cassert>
//using namespace std;
//
//int main()
//{
//    Navigator na;
//    
//    cout << "About to test MyMap" << endl;
//    {
//        MyMap<int, string> mm;
//        mm.associate(20, "Ethel");
//        mm.associate(10, "Fred");
//        const string* p = mm.find(10);
//        assert(p != nullptr  &&  *p == "Fred");
//        assert(mm.find(30) == nullptr);
//        assert(mm.size() == 2);
//    }
//    cout << "MyMap PASSED" << endl;
//    
//    cout << "About to test MapLoader" << endl;
//    {
//        MapLoader ml;
//        assert(ml.load("/Users/williamchou/Downloads/testBruinNav/testmap.txt"));
//        size_t numSegments = ml.getNumSegments();
//        assert(numSegments == 7);
//        bool foundAttraction = false;
//        for (size_t i = 0; i < numSegments; i++)
//        {
//            StreetSegment seg;
//            assert(ml.getSegment(i, seg));
//            if (seg.streetName == "Picadilly")
//            {
//                assert(seg.attractions.size() == 1);
//                assert(seg.attractions[0].name == "Eros Statue");
//                foundAttraction = true;
//                break;
//            }
//        }
//        assert(foundAttraction);
//    }
//    cout << "MapLoader PASSED" << endl;
//    
//    cout << "About to test AttractionMapper" << endl;
//    {
//        MapLoader ml;
//        assert(ml.load("/Users/williamchou/Downloads/testBruinNav/testmap.txt"));
//        AttractionMapper am;
//        am.init(ml);
//        GeoCoord gc;
//        assert(am.getGeoCoord("Hamleys Toy Store", gc));
//        assert(gc.latitudeText == "51.512812");
//        assert(gc.longitudeText == "-0.140114");
//    }
//    cout << "AttractionMapper PASSED" << endl;
//    
//    cout << "About to test SegmentMapper" << endl;
//    {
//        MapLoader ml;
//        assert(ml.load("/Users/williamchou/Downloads/testBruinNav/testmap.txt"));
//        SegmentMapper sm;
//        sm.init(ml);
//        GeoCoord gc("51.510087", "-0.134563");
//        vector<StreetSegment> vss = sm.getSegments(gc);
//        assert(vss.size() == 4);
//        string names[4];
//        for (size_t i = 0; i < 4; i++)
//            names[i] = vss[i].streetName;
//        sort(names, names+4);
//        const string expected[4] = {
//            "Coventry Street", "Picadilly", "Regent Street", "Shaftesbury Avenue"
//        };
//        assert(equal(names, names+4, expected));
//    }
//    cout << "SegmentMapper PASSED" << endl;
//    
//    cout << "About to test Navigator" << endl;
//    {
//        Navigator nav;
//        assert(nav.loadMapData("/Users/williamchou/Downloads/testBruinNav/testmap.txt"));
//        vector<NavSegment> directions;
//        assert(nav.navigate("Eros Statue", "Hamleys Toy Store", directions) == NAV_SUCCESS);
//        assert(directions.size() == 6);
//        struct ExpectedItem
//        {
//            NavSegment::NavCommand command;
//            string direction;
//            double distance;
//            string streetName;
//        };
//        const ExpectedItem expected[6] = {
//            { NavSegment::PROCEED, "northwest", 0.0138, "Picadilly" },
//            { NavSegment::TURN, "left", 0, "" },
//            { NavSegment::PROCEED, "west", 0.0119, "Regent Street" },
//            { NavSegment::PROCEED, "west", 0.0845, "Regent Street" },
//            { NavSegment::PROCEED, "west", 0.0696, "Regent Street" },
//            { NavSegment::PROCEED, "northwest", 0.1871, "Regent Street" },
//        };
//        for (size_t i = 0; i < 6; i++)
//        {
//            const NavSegment& ns = directions[i];
//            const ExpectedItem& exp = expected[i];
//            assert(ns.m_command == exp.command);
//            assert(ns.m_direction == exp.direction);
//            if (ns.m_command == NavSegment::PROCEED)
//            {
//                assert(abs(ns.m_distance - exp.distance) < 0.00051);
//                assert(ns.m_streetName == exp.streetName);
//            }
//        }
//    }
//    cout << "Navigator PASSED" << endl;
//}
//
//


/////////////////////////////////////////////////////////

#include "MyMap.h"
#include "provided.h"
#include "support.h"
#include <iostream>
using namespace std;

void printDirectionsRaw(string start, string end, vector<NavSegment>& navSegments);
void printDirections(string start, string end, vector<NavSegment>& navSegments);

int main(int argc, char *argv[])
{
    bool raw = false;
    if (argc == 5  &&  strcmp(argv[4], "-raw") == 0)
    {
        raw = true;
        argc--;
    }
    if (argc != 4)
    {
        cout << "Usage: BruinNav mapdata.txt \"start attraction\" \"end attraction name\"" << endl
        << "or" << endl
        << "Usage: BruinNav mapdata.txt \"start attraction\" \"end attraction name\" -raw" << endl;
        return 1;
    }
    
    Navigator nav;
    
    if ( ! nav.loadMapData(argv[1]))
    {
        cout << "Map data file was not found or has bad format: " << argv[1] << endl;
        return 1;
    }
    
    if ( ! raw)
        cout << "Routing..." << flush;
    
    string start = argv[2];
    string end = argv[3];
    vector<NavSegment> navSegments;
    
    NavResult result = nav.navigate(start, end, navSegments);
    if ( ! raw)
        cout << endl;
    
    switch (result)
    {
        case NAV_NO_ROUTE:
            cout << "No route found between " << start << " and " << end << endl;
            break;
        case NAV_BAD_SOURCE:
            cout << "Start attraction not found: " << start << endl;
            break;
        case NAV_BAD_DESTINATION:
            cout << "End attraction not found: " << end << endl;
            break;
        case NAV_SUCCESS:
            if (raw)
                printDirectionsRaw(start, end, navSegments);
            else
                printDirections(start, end, navSegments);
            break;
    }
}

void printDirectionsRaw(string start, string end, vector<NavSegment>& navSegments)
{
    cout << "Start: " << start << endl;
    cout << "End:   " << end << endl;
    cout.setf(ios::fixed);
    cout.precision(4);
    for (auto ns : navSegments)
    {
        switch (ns.m_command)
        {
            case NavSegment::PROCEED:
                cout << ns.m_geoSegment.start.latitudeText << ","
                << ns.m_geoSegment.start.longitudeText << " "
                << ns.m_geoSegment.end.latitudeText << ","
                << ns.m_geoSegment.end.longitudeText << " "
                << ns.m_direction << " "
                << ns.m_distance << " "
                << ns.m_streetName << endl;
                break;
            case NavSegment::TURN:
                cout << "turn " << ns.m_direction << " " << ns.m_streetName << endl;
                break;
        }
    }
}

void printDirections(string start, string end, vector<NavSegment>& navSegments)
{
    cout.setf(ios::fixed);
    cout.precision(2);
    
    cout << "You are starting at: " << start << endl;
    
    double totalDistance = 0;
    string thisStreet;
    GeoSegment effectiveSegment;
    double distSinceLastTurn = 0;
    
    for (auto ns : navSegments)
    {
        switch (ns.m_command)
        {
            case NavSegment::PROCEED:
                if (thisStreet.empty())
                {
                    thisStreet = ns.m_streetName;
                    effectiveSegment.start = ns.m_geoSegment.start;
                }
                effectiveSegment.end = ns.m_geoSegment.end;
                distSinceLastTurn += ns.m_distance;
                totalDistance += ns.m_distance;
                break;
            case NavSegment::TURN:
                if (distSinceLastTurn > 0)
                {
                    cout << "Proceed " << distSinceLastTurn << " miles "
                    << directionOfLine(effectiveSegment) << " on " << thisStreet << endl;
                    thisStreet.clear();
                    distSinceLastTurn = 0;
                }
                cout << "Turn " << ns.m_direction << " onto " << ns.m_streetName << endl;
                break;
        }
    }
    
    if (distSinceLastTurn > 0)
        cout << "Proceed " << distSinceLastTurn << " miles "
        << directionOfLine(effectiveSegment) << " on " << thisStreet << endl;
    cout << "You have reached your destination: " << end << endl;
    cout.precision(1);
    cout << "Total travel distance: " << totalDistance << " miles" << endl;
}


//
//  main.cpp
//  Project 4
//
//  Created by William Chou on 3/8/17.
//  Copyright © 2017 William Chou. All rights reserved.
//
//#include "MyMap.h"
//#include "provided.h"
//#include <iostream>
//using namespace std;
//
//void foo();
//
//int main() {
//    foo();
//}
//    MapLoader ml;
//    cerr << ml.getNumSegments() << endl;
//    ml.load("/Users/williamchou/Downloads/skeleton-2/mapdata.txt");
//    cerr << ml.getNumSegments() << endl;
//    StreetSegment ss;
//    ml.getSegment(0, ss);
//    cerr << ss.streetName << endl;
//    cerr << ss.attractions.size() << endl;
//    cerr << ss.segment.start.latitude << endl;
//    cerr << ss.segment.start.longitude << endl;
//    cerr << ss.segment.end.latitude << endl;
//    cerr << ss.segment.end.longitude << endl;
//    
////    ml.getSegment(107, ss);
////    cerr << ss.streetName << endl;
////    cerr << ss.attractions.size() << endl;
////    cerr << ss.attractions[0].name << endl;
////    cerr << ss.attractions[0].geocoordinates.latitude << endl;
////    cerr << ss.attractions[0].geocoordinates.longitude << endl;
//    
////    AttractionMapper am;
////    am.init(ml);
////    GeoCoord gc;
////    if(am.getGeoCoord("the wallis annenberg center for the performing arts", gc)) {
////        cerr << gc.latitude << endl;
////        cerr << gc.longitude << endl;
////    }
////    else {
////        cerr << "Not found" << endl;
////    }
////    
////    SegmentMapper sm;
////    sm.init(ml);
////    GeoCoord geoc("34.0630614", "-118.4468781");
////    vector<StreetSegment> temp = sm.getSegments(geoc);
////    for(int i = 0; i < temp.size(); i++) {
////        cerr << temp[i].streetName << endl;
////        cerr << temp[i].attractions.size() << endl;
////        cerr << temp[i].segment.start.latitude << endl;
////        cerr << temp[i].segment.start.longitude << endl;
////        cerr << temp[i].segment.end.latitude << endl;
////        cerr << temp[i].segment.end.longitude << endl;
////    }
////    if(temp.size() == 0) {
////        cerr << "nothing found" << endl;
////    }
//    
//    Navigator nv;
//    nv.loadMapData("/Users/williamchou/Downloads/skeleton-2/mapdata.txt");
//    
//    vector<NavSegment> navVector;
//    if(nv.navigate("1061 Broxton Avenue", "Headlines!", navVector) == NAV_SUCCESS) {
////    if(nv.navigate("Diddy Riese", "Barney's Beanery", navVector) == NAV_SUCCESS) {
////    if(nv.navigate("West Alumni Center", "1061 Broxton Avenue", navVector) == NAV_SUCCESS) {
//        cerr << "Navigation successful" << endl;
//    }
//    else {
//        cerr << "Fuck this shit" << endl;
//    }
//}
//
//void foo() {
//    MyMap<string,double> nameToGPA; // maps student name to GPA
//    // add new items to the binary search tree-based map
//    nameToGPA.associate("Carey", 3.5); // Carey has a 3.5 GPA
//    nameToGPA.associate("David", 3.99); // David beat Carey
//    nameToGPA.associate("Abe", 3.2); // Abe has a 3.2 GPA
//    
//    double* davidsGPA = nameToGPA.find("David");
//    if (davidsGPA != nullptr)
//        *davidsGPA = 1.5; // after a re-grade of David’s exam
//    nameToGPA.associate("Carey", 4.0); // Carey deserves a 4.0 // replaces old 3.5 GPA
//    double* lindasGPA = nameToGPA.find("Linda");
//    if (lindasGPA == nullptr)
//        cout << "Linda is not in the roster!" << endl;
//    else
//        cout << "Linda’s GPA is: " << *lindasGPA << endl;
//    
//    double* careysGPA = nameToGPA.find("Carey");
//    if (careysGPA == nullptr)
//        cout << "Carey is not in the roster!" << endl;
//    else
//        cout << "Carey's GPA is: " << *careysGPA << endl;
//    
//    //nameToGPA.clear();
//}