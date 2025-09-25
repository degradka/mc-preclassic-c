// character/vertex.h

#ifndef VERTEX_H
#define VERTEX_H

#include "vec3.h"

typedef struct { Vec3 pos; float u, v; } Vertex;
static inline Vertex Vertex_make(float x,float y,float z,float u,float v) {
    return (Vertex){ {x,y,z}, u, v };
}
static inline Vertex Vertex_remap(Vertex base, float u, float v) {
    return (Vertex){ base.pos, u, v };
}

#endif
