// character/polygon.h

#ifndef POLYGON_H
#define POLYGON_H

#include "vertex.h"

typedef struct { Vertex v[4]; } Polygon;

void Polygon_init_uv(Polygon* p, Vertex a, Vertex b, Vertex c, Vertex d,
                     int minU, int minV, int maxU, int maxV);
void Polygon_render(const Polygon* p);

#endif
