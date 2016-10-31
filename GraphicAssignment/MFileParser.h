#include <stdlib.h>;
#include<GL\glui.h>;
#include "Vector3Point.h";
#include "Vector2.h";
#include "TriangleFaceData.h";


//using namespace std::stri;
using std::string;
class MFileParser
{
public:
	MFileParser();
	~MFileParser();
	void setmode(int mode);
	bool parserFile(char* path);
	void display();
	std::vector<Vector3Point> *m_vertices;
	std::vector<TriangleFaceData> *m_faces;
private:
	int m_render_mode;
	//std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	//std::vector<Vector3Point> *m_vertices;
	//std::vector<TriangleFaceData> *m_faces;
	//std::vector<Vector2> temp_uvs;
	//std::vector<Vector3Point> temp_normals;
};