#include "MFileParser.h";

#include <GL\glut.h>;


MFileParser::MFileParser()
{
	m_vertices = new std::vector<Vector3>();
	m_faces = new std::vector<TriangleFaceData>();
}

MFileParser::~MFileParser()
{

}
void MFileParser::setmode(int mode)
{
	m_render_mode = mode;
}

bool MFileParser::parserFile(char* path)
{
	FILE * file;
	fopen_s(&file,path, "r");
	if (file == NULL){
		printf("cannot open the file !\n");
		return false;
	}
	printf("read ok");
	
	while (true)
	{
		char lineHeader[128];
		// read the first word of the line
		int res = fscanf_s(file, "%s",  lineHeader,128);
		if (res == EOF)
			break;
		// else : parse lineHeader
		else
		{
			if (strcmp(lineHeader, "Vertex") == 0)
			{
				Vector3 vertex;
				int *index;
				fscanf_s(file, "%d %f %f %f\n", &index, &vertex.x, &vertex.y, &vertex.z);
				m_vertices->push_back(vertex);
			}
			else if (strcmp(lineHeader, "Face") == 0){
				std::string vertex1, vertex2, vertex3;
				unsigned int index,p1, p2, p3;
				int matches = fscanf_s(file, "%d %d %d %d", &index, &p1,&p2,&p3);
				TriangleFaceData f;
				f.p1 = m_vertices->at(p1-1);
				f.p2 = m_vertices->at(p2 - 1);
				f.p3 = m_vertices->at(p3 - 1);
				m_faces->push_back(f);
				
			}
		}
		
	}
	return true;
}

void MFileParser::display()
{
	switch (m_render_mode)
	{
	case POINT_MODE:
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < m_vertices->size(); i++)
		{
			glColor3ub(0, 127, 255);
			glVertex3f(m_vertices->at(i).x, m_vertices->at(i).y, m_vertices->at(i).z);
		}
		glEnd();
		break;
	case LINE_MODE:
		
		for (unsigned int i = 0; i < m_faces->size(); i++)
		{
			glBegin(GL_TRIANGLE_STRIP);
			glColor3ub(127, 127, 127);
			glVertex3f(m_faces->at(i).p1.x, m_faces->at(i).p1.y, m_faces->at(i).p1.z);
			glVertex3f(m_faces->at(i).p2.x, m_faces->at(i).p2.y, m_faces->at(i).p2.z);
			glVertex3f(m_faces->at(i).p3.x, m_faces->at(i).p3.y, m_faces->at(i).p3.z);
			glEnd();

		}
		
		break;
	case FLAT_SHADING:
		glBegin(GL_TRIANGLES);
		for (unsigned int i = 0; i < m_faces->size(); i++)
		{
			glColor3ub(127, 127, 127);
			glVertex3f(m_faces->at(i).p1.x, m_faces->at(i).p1.y, m_faces->at(i).p1.z);
			glVertex3f(m_faces->at(i).p2.x, m_faces->at(i).p2.y, m_faces->at(i).p2.z);
			glVertex3f(m_faces->at(i).p3.x, m_faces->at(i).p3.y, m_faces->at(i).p3.z);
		}
		glEnd();
	default:
		break;
	}
	
}