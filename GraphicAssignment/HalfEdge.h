#include "Vector3Point.h"

struct HE_vert;
struct HE_face;
struct HE_edge;

struct HE_edge
{
	HE_vert* vert;
	HE_edge* pair;
	HE_face* face;
	HE_edge* prev;
	HE_edge* next;
};

struct HE_vert
{
	Vector3Point position;
	Vector3Point normal;
	float nx, ny, nz;
	HE_edge* edge;
	std::map<HE_vert*, HE_edge*> tempEdges;//use destination vertex as key
	HE_edge *tempEdge;
};

struct HE_face
{
	Vector3Point normal;
	HE_edge* edge;
};