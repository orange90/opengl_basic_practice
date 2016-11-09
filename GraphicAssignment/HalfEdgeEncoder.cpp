#include "HalfEdgeEncoder.h"
#include "RenderMode.h"

HalfEdgeEncoder::HalfEdgeEncoder()
{
	m_vertice = new std::vector<HE_vert*>();
	//m_edges = new std::map<std::string, HE_edge>();
	m_faces = new std::vector<HE_face*>();
}

HalfEdgeEncoder::~HalfEdgeEncoder()
{
}

void HalfEdgeEncoder::encode(std::vector<Vector3Point>* vertice, std::vector<TriangleFaceData>* faces)
{
	m_vertice->clear();
	m_faces->clear();
	m_edges.clear();
	//encode vert
	for (size_t i = 0; i < vertice->size(); i++)
	{
		//encode axis
		HE_vert *v = new HE_vert();
		v->position.x = vertice->at(i).x;
		v->position.y = vertice->at(i).y;
		v->position.z = vertice->at(i).z;
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
			m_edges[key1] = e1;
		}
		e1->vert = v1;
		v1->edge = e1;
		HE_edge *e2 = getEdgeByKey(key2);
		if (e2 == nullptr)
		{
			e2 = new HE_edge();
			m_edges[key2] = e2;
		}
		e2->vert = v2;
		v2->edge = e2;
		HE_edge *e3 = getEdgeByKey(key3);
		if (e3 == nullptr)
		{
			e3 = new HE_edge();
			m_edges[key3] = e3;
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
		m_faces->push_back(f);

	}
	initFaceNormal();
	initVertexNormal();
}

void HalfEdgeEncoder::initFaceNormal()
{
	for (int i = 0; i < m_faces->size(); i++)
	{
		float fnx, fny, fnz;
		
		HE_vert* p0 = m_faces->at(i)->edge->vert;
		HE_vert* p1 = m_faces->at(i)->edge->next->vert;
		HE_vert* p2 = m_faces->at(i)->edge->prev->vert;

		Vector3Point u = p1->position - p0->position;
		Vector3Point v = p2->position - p0->position;
		m_faces->at(i)->normal = u.crossProduct(v);
	}
}

void HalfEdgeEncoder::initVertexNormal()
{
	for (int i = 0; i < m_vertice->size(); i++)
	{
		bool meetBoundary = false;
		int faceNum = 0;
		Vector3Point vn;
		//1-ring traversal to get all faces
		HE_edge *startEdge = m_vertice->at(i)->edge;
		HE_edge *currentEdge = startEdge;
		HE_face *startFace = startEdge->face;
		do
		{
			if (currentEdge->pair == NULL)//meet boundary,reset states
			{
				meetBoundary = true;
				startEdge = currentEdge;
				vn = Vector3Point(0, 0, 0);
				faceNum = 0;
				break;
			}
			Vector3Point facenomral = currentEdge->pair->next->face->normal;
			vn = vn + facenomral;
			faceNum++;
			currentEdge = currentEdge->pair->next;

		} while (currentEdge != startEdge);
		if (meetBoundary)//change direction
		{
			do
			{
				if (currentEdge == NULL)
				{
					break;
				}
				Vector3Point facenomral = currentEdge->face->normal;
				vn = vn + facenomral;
				faceNum++;
				currentEdge = currentEdge->prev->pair;

			} while (currentEdge != startEdge);
		}
		vn = vn / faceNum;
		m_vertice->at(i)->normal = vn;
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

void HalfEdgeEncoder::drawPolygons(HE_face* face)
{
	HE_edge *startEdge = face->edge;
	HE_edge *currentEdge = startEdge;
	do
	{
		glNormal3f(currentEdge->vert->normal.x, currentEdge->vert->normal.y, currentEdge->vert->normal.z);
		glVertex3f(currentEdge->vert->position.x, currentEdge->vert->position.y, currentEdge->vert->position.z);
		currentEdge = currentEdge->next;
	} while (currentEdge != startEdge);
}
void HalfEdgeEncoder::render()
{
	HE_vert *startVertex = m_vertice->at(0);
	switch (render_mode)
	{
		
		case POINT_MODE:
		{
			for (int i = 0; i < m_faces->size();i++)
			{
				glBegin(GL_POINT);
				drawPolygons(m_faces->at(i));
				//glNormal3f(m_faces->at(i)->normal.x, m_faces->at(i)->normal.y, m_faces->at(i)->normal.z);
				glEnd();
			} 
			break;
		}
		case LINE_MODE:
			for (int i = 0; i < m_faces->size();i++)
			{
				glBegin(GL_LINE_LOOP);
				//glNormal3f(m_faces->at(i)->normal.x, m_faces->at(i)->normal.y, m_faces->at(i)->normal.z);
				drawPolygons(m_faces->at(i));
				glEnd();
			} 
			break;
		case FLAT_SHADING:
			for (int i = 0; i < m_faces->size(); i++)
			{
				glColor3f(0.5, 0.5, 0.5);
				glShadeModel(GL_FLAT);
				glBegin(GL_TRIANGLES);
				//glNormal3f(m_faces->at(i)->normal.x, m_faces->at(i)->normal.y, m_faces->at(i)->normal.z);
				drawPolygons(m_faces->at(i));
				glEnd();
			}
			break;
		case SMOOTH_SHADING:
			for (int i = 0; i < m_faces->size(); i++)
			{
				glColor3f(0.5, 0.5, 0.5);
				glShadeModel(GL_SMOOTH);
				glBegin(GL_TRIANGLES);
				//glNormal3f(m_faces->at(i)->normal.x, m_faces->at(i)->normal.y, m_faces->at(i)->normal.z);
				drawPolygons(m_faces->at(i));
				glEnd();
			}
			break;
		default:
			break;
	}
}

void HalfEdgeEncoder::setmode(int mode)
{
	render_mode = mode;
}