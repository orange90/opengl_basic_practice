#include "HalfEdgeEncoder.h"
#include "RenderMode.h"

HalfEdgeEncoder::HalfEdgeEncoder()
{
	m_vertice = new std::vector<HE_vert*>();
	//m_edges = new std::map<std::string, HE_edge>();
	m_faces = new std::vector<HE_face>();
}

HalfEdgeEncoder::~HalfEdgeEncoder()
{
}

void HalfEdgeEncoder::encode(std::vector<Vector3Point>* vertice, std::vector<TriangleFaceData>* faces)
{
	//encode vert
	for (size_t i = 0; i < vertice->size(); i++)
	{
		//encode axis
		HE_vert *v = new HE_vert();;
		v->x = vertice->at(i).x;
		v->y = vertice->at(i).y;
		v->z = vertice->at(i).z;
		m_vertice->push_back(v);
	}
	//encode edge
	for (size_t i = 0; i < faces->size(); i++)
	{
		int v1Index = faces->at(i).p1Index;
		int v2Index = faces->at(i).p2Index;
		int v3Index = faces->at(i).p3Index;
		//should not be null
		HE_vert* v1 = getVertAtIndex(v1Index);
		HE_vert* v2 = getVertAtIndex(v2Index);
		HE_vert* v3 = getVertAtIndex(v3Index);
		string key1 = std::to_string(v1Index) + ":" + std::to_string(v2Index);
		string key2 = std::to_string(v2Index) + ":" + std::to_string(v3Index);
		string key3 = std::to_string(v3Index) + ":" + std::to_string(v1Index);
		//init edge's vert
		HE_edge *e1 = getEdgeByKey(key1);
		if (e1 == nullptr)
		{
			e1 = new HE_edge();
		}
		e1->vert = v1;
		v1->edge = e1;
		HE_edge *e2 = getEdgeByKey(key2);
		if (e2 == nullptr)
		{
			e2 = new HE_edge();
		}
		e2->vert = v2;
		v2->edge = e2;
		HE_edge *e3 = getEdgeByKey(key3);
		if (e3 == nullptr)
		{
			e3 = new HE_edge();
		}
		e3->vert = v3;
		v3->edge = e3;
		//init edges next and prev
		e1->next = e2;
		e2->next = e3;
		e3->next = e1;
		e1->prev = e3;
		e2->prev = e1;
		e3->prev = e2;
		//init edge's pair
		string pair1key = std::to_string(v2Index) + ":" + std::to_string(v1Index);
		string pair2key = std::to_string(v3Index) + ":" + std::to_string(v2Index);
		string pair3key = std::to_string(v1Index) + ":" + std::to_string(v3Index);
		HE_edge *e1pair = getEdgeByKey(pair1key);
		if (e1pair != nullptr)
		{
			e1->pair = e1pair;
			e1pair->pair = e1;
		}
		HE_edge *e2pair = getEdgeByKey(pair2key);
		if (e2pair != nullptr)
		{
			e2->pair = e2pair;
			e2pair->pair = e2;
		}
		HE_edge *e3pair = getEdgeByKey(pair3key);
		if (e3pair != nullptr)
		{
			e3->pair = e3pair;
			e3pair->pair = e3;
		}
		//encode face
		HE_face *f = new HE_face();
		f->edge = e1;
		e1->face = f;
		e2->face = f;
		e3->face = f;
	}
}

HE_vert* HalfEdgeEncoder::getVertAtIndex(unsigned int idx)
{
	return m_vertice->at(idx - 1);
}

HE_edge* HalfEdgeEncoder::getEdgeByKey(std::string key)
{
	return m_edges[key];
}

void HalfEdgeEncoder::render()
{
	HE_vert *startVertex = m_vertice->at(0);
	switch (render_mode)
	{
		case POINT_MODE:
		{
			glBegin(GL_POINT);
			HE_edge *currentEdge = startVertex->edge;
			do
			{
				glVertex3f(currentEdge->vert->x, currentEdge->vert->y, currentEdge->vert->z);
				currentEdge = currentEdge->pair->next;
			} while (currentEdge != startVertex->edge);
			/*for (size_t i = 0; i < m_vertice->size(); i++)
			{
			glColor3ub(0, 127, 255);
			glVertex3f(m_vertice->at(i)->x, m_vertice->at(i)->y, m_vertice->at(i)->z);
			}
			glEnd();*/
			glEnd();
			break;
		}
		case LINE_MODE:
		
			break;
		case FLAT_SHADING:
		
			break;
		case SMOOTH_SHADING:
		
			break;
		default:
			break;
	}
}

void HalfEdgeEncoder::setmode(int mode)
{
	render_mode = mode;
}