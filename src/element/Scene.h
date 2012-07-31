#ifndef _SCENE_H_
#define _SCENE_H_

#include <gl/glew.h>
#include "Drawable.h"
#include "Shape.h"
#include "Transform.h"
#include "Light.h"
#include "../shadelib/shader.h"
#include <vector>

using namespace::std;

class Scene : public Drawable
{
protected:
   vector<Drawable*> m_dynamicObjects;
   vector<Drawable*> m_staticObjects;

   vector<Light*> m_lights;

   // camera transform?
   Vec3f m_cameraLoc;

   void setupCamera();
   void setupLights();

public:

   Transform* m_transform;

   Shader* activeShader;

   bool startMove;


   Scene() : m_transform(NULL),
             m_cameraLoc(0.0f,0.0f,100.0f),
             startMove(false)
   {}
   ~Scene();

   void addShape(Shape* shape, bool dynamic);
   void addLight(Light* l);
   void draw();
   void printElements();


};

#endif