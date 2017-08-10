#ifndef SUPPORT_H
#define SUPPORT_H

#include "provided.h"
#include <string>

bool operator<(const GeoCoord& g1, const GeoCoord& g2);
bool operator==(const GeoCoord& g1, const GeoCoord& g2);
bool operator>(const GeoCoord& g1, const GeoCoord& g2);
std::string directionOfLine(const GeoSegment& gs);

#endif