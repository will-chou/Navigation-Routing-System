#include "provided.h"
#include "support.h"
#include <string>
using namespace std;

bool operator<(const GeoCoord& g1, const GeoCoord& g2) {
    return g1.latitudeText + g1.longitudeText < g2.latitudeText + g2.longitudeText;
}

bool operator==(const GeoCoord& g1, const GeoCoord& g2) {
    return g1.latitudeText + g1.longitudeText == g2.latitudeText + g2.longitudeText;
}

bool operator>(const GeoCoord& g1, const GeoCoord& g2) {
    return g1.latitudeText + g1.longitudeText > g2.latitudeText + g2.longitudeText;
}

string directionOfLine(const GeoSegment& gs) {
    double angle = angleOfLine(gs);
    
    if(angle >= 0 && angle <= 22.5) {
        return "east";
    }
    else if(angle > 22.5 && angle <= 67.5) {
        return "northeast";
    }
    else if(angle > 67.5 && angle <= 112.5) {
        return "north";
    }
    else if(angle > 112.5 && angle <= 157.5) {
        return "northwest";
    }
    else if(angle > 157.5 && angle <= 202.5) {
        return "west";
    }
    else if(angle > 202.5 && angle <= 247.5) {
        return "southwest";
    }
    else if(angle > 247.5 && angle <= 292.5) {
        return "south";
    }
    else if(angle > 292.5 && angle <= 337.5) {
        return "southeast";
    }
    else {
        return "east";
    }
}