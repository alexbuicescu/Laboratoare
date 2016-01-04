 
#include <stdlib.h> // necesare pentru citirea shader-elor
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <GL/glew.h> // glew apare inainte de freeglut
#include <GL/freeglut.h> // nu trebuie uitat freeglut.h

#include "myHeader.h"

#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;
  
//////////////////////////////////////

GLuint
  VaoId,
  VboId,
  ColorBufferId,
  ProgramId,
  myMatrixLocation,
  matrScaleLocation,
  matrTranslLocation,
  matrRotlLocation,
  codColLocation;
 
glm::mat4 myMatrix, resizeMatrix, matrTransl, matrScale, matrRot; 


int codCol;
 float PI=3.141592;

 int width=1280, height=720;

float currentTime;

class myPoint {
private:
  GLfloat x, y, z1, z2;
public:
  myPoint()
  {
  }
  myPoint(GLfloat x, GLfloat y)
  {
    this->x = x;
    this->y = y;
    this->z1 = 0.0f;
    this->z2 = 1.0f;
  }

  myPoint(GLfloat x, GLfloat y, GLfloat z1, GLfloat z2)
  {
    this->x = x;
    this->y = y;
    this->z1 = z1;
    this->z2 = z2;
  }
};

class CelestialObject {
private:
  float x, y, radius, speed;
  int index;
  CelestialObject *satellite;
  int num_segments = 10;
  myPoint myPoints[10];
  void initPoints()
  {
    for(int i = 0; i < num_segments; i++)
    {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);//get the current angle

        GLfloat x = radius * cosf(theta);//calculate the x component
        GLfloat y = radius * sinf(theta);//calculate the y component

        myPoints[i] = myPoint(x, y);//output vertex
    }
  }
public:
  CelestialObject()
  {
    x = 0;
    y = 0;
  }
  CelestialObject(int index, float radius, float speed)
  {
    this->index = index;
    this->radius = radius;
    this->speed = speed;
    this->satellite = new CelestialObject();
    initPoints();
  }
  CelestialObject(int index, float radius, float speed, CelestialObject *satellite)
  {
    this->index = index;
    this->radius = radius;
    this->speed = speed;
    this->satellite = satellite;
    initPoints();
  }

  void update()
  {
    int distance = radius * index;
    x = cos(currentTime * speed) * distance + satellite->x;
    y = sin(currentTime * speed) * distance + satellite->y;
  }

  void draw()
  {
    // se creeaza un buffer nou
    glGenBuffers(1, &VboId);
    // este setat ca buffer curent
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    // punctele sunt "copiate" in bufferul curent
    glBufferData(GL_ARRAY_BUFFER, sizeof(myPoints), myPoints, GL_STATIC_DRAW);
    
    // se creeaza / se leaga un VAO (Vertex Array Object) - util cand se utilizeaza mai multe VBO
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);
    // se activeaza lucrul cu atribute; atributul 0 = pozitie
    glEnableVertexAttribArray(0);
    // 
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);  

    // matrTransl=glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0));
    matrTransl=glm::translate<GLfloat>(glm::vec3(x, y, 0.0f));
    myMatrix=resizeMatrix*matrTransl;
    
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE,  glm::value_ptr(myMatrix));

    glUniform1i(codColLocation, index);
    glDrawArrays(GL_LINE_LOOP, 0, num_segments);    
  }
};

CelestialObject *star = new CelestialObject(0, 150, 0.5f);
CelestialObject *planet = new CelestialObject(5, 100, 1, star);
CelestialObject *satellite = new CelestialObject(4, 50, 4, planet);

void DestroyVBO(void)
{
 

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &ColorBufferId);
  glDeleteBuffers(1, &VboId);

  glBindVertexArray(0);
  glDeleteVertexArrays(1, &VaoId);

   
}

void CreateShaders(void)
{
  ProgramId=LoadShaders("src/08_01_Shader.vert", "src/08_01_Shader.frag");
  glUseProgram(ProgramId);
}

 
void DestroyShaders(void)
{
  glDeleteProgram(ProgramId);
}
 
void Initialize(void)
{
  myMatrix = glm::mat4(1.0f);
  
  resizeMatrix= glm::scale(glm::mat4(1.0f), glm::vec3(1.f/width, 1.f/height, 1.0));

  CreateShaders(); 

  glClearColor(1.0f, 1.0f, 1.0f, 0.0f); // culoarea de fond a ecranului
}
void RenderFunction(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  currentTime += 0.0001f;
  satellite->update();
  satellite->draw();
  planet->update();
  planet->draw();
  star->update();
  star->draw();
  // myMatrix=resizeMatrix;
 
  // myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
  // glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE,  &myMatrix[0][0]);
  // // desenare puncte din colturi si axe
  // glPointSize(10.0);
  // codCol=0;

  // CreateVBO();
  // glUniform1i(codColLocation, codCol);
  // glDrawArrays(GL_POINTS, 0, nrOfPoints);
  
  // CreateVBO2();
  // glDrawArrays(GL_LINES, 0, nrOfPoints * 2);

  // // codCol=2;
  // // glUniform1i(codColLocation, codCol);
  // // glDrawArrays(GL_LINES, 1, nrOfPoints * 2);

  glutPostRedisplay();
  glutSwapBuffers();
  glFlush ( );
}
void Cleanup(void)
{
  DestroyShaders();
  DestroyVBO();
}

int main(int argc, char* argv[])
{

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
  glutInitWindowPosition (100,100); 
  glutInitWindowSize(width,height); 
  glutCreateWindow("Utilizarea glm pentru transformari"); 
  glewInit(); 
  Initialize( );
  glutDisplayFunc(RenderFunction);
  glutCloseFunc(Cleanup);
  glutMainLoop();
  
  
}

