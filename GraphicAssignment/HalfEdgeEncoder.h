#include <stdlib.h>;
#include "HalfEdge.h";
#include "Vector2.h";
#include "Vector3Point.h";
#include "TriangleFaceData.h"
#include<GL\glui.h>;
#include <map>
#include <string>
using std::string;
class HalfEdgeEncoder
{
public:
	//std::vector<HE_edge>* m_edges;
	std::vector<HE_vert*>* m_vertice;
	std::vector<HE_face>* m_faces;
	//use map with complexity O(1)
	std::map<std::string,HE_edge*> m_edges;
	HalfEdgeEncoder();
	~HalfEdgeEncoder();
	void setmode(int mode);
	void encode(std::vector<Vector3Point>* vertice,std::vector<TriangleFaceData>* faces);
	void render();
private:
	int render_mode;
	
	HE_vert* getVertAtIndex(unsigned int idx);
	HE_edge* getEdgeByKey(std::string key);
};

