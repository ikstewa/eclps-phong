#include "Scene.h"

// TEMP BAD PLACE
float lightRot = 0.0;
GLfloat lightPos[3] = {-200.0f, 10.0f, -80.0f};
GLfloat lightPos2[3] = {200.0f, 10.0f, -100.0f};
GLfloat Al[4] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat Dl[4] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat Sl[4] = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat att[3] = { 1.0f, 0.0f, 0.00001f }; // constant, linear, quadratic


Scene::~Scene()
{
   for each(Drawable* d in m_dynamicObjects)
   {
      delete d;
   }
   for each(Drawable* d in m_staticObjects)
   {
      delete d;
   }
   for each(Light* l in m_lights)
   {
      delete l;
   }

   if (m_transform)
   {
      delete m_transform;
   }
}

float angle = -0;
float cameraRot = 0.0;

void Scene::setupCamera()
{
   if (startMove)
   {
      if (m_lights[2]->m_diffuse.x < 1.0)
      {
         m_lights[2]->m_diffuse += Vec3f(0.001,0.001,0.001);
         m_lights[2]->m_specular.x += 0.001;
         m_lights[2]->m_specular.y += 0.001;
      }
      else
      {
         m_cameraLoc.y += 0.01;
         m_cameraLoc.x += 0.005;
         m_cameraLoc.z += 0.01;
          cameraRot += 0.01;

          m_lights[0]->m_enabled = true;
          m_lights[1]->m_enabled = true;
      }
   }
   // Setup camera
   glMatrixMode(GL_PROJECTION);
   //glLoadIdentity();
   gluLookAt(  m_cameraLoc.x, m_cameraLoc.y, m_cameraLoc.z, // camera
               0.0f, 0.0f, 0.0f,    // look at point
               0.0f, 1.0f, 0.0f);   // up vector
   glMatrixMode(GL_MODELVIEW);
   glRotatef(cameraRot, 0.0, 1.0, 0.0);
}

GLenum counterToEnum(unsigned int cnt)
{
   switch(cnt)
   {
   case 0:
      return GL_LIGHT0;
   case 1:
      return GL_LIGHT1;
   case 2:
      return GL_LIGHT2;
   case 3:
      return GL_LIGHT3;
   case 4:
      return GL_LIGHT4;
   case 5:
      return GL_LIGHT5;
   case 6:
      return GL_LIGHT6;
   case 7:
      return GL_LIGHT7;
   default:
      return -1;
   }

}

void Scene::setupLights()
{
   unsigned int lightCount = 0;
   for each(Light* l in m_lights)
   {
      if (l->m_enabled)
      {
         l->applyLight(counterToEnum(lightCount++));
      }
   }

   if (lightCount > 0)
   {
      activeShader->attach();
      GLint cutoff = glGetUniformLocation(activeShader->shaderProg, "cos_outer_cone_angle");
      glUniform1f(cutoff, m_lights[0]->m_spotOuterCutoff);
   }

   /*

   lightRot += 0.01;
   lightPos[1] -= 0.01;
   lightPos2[1] -= 0.01;
   glPushMatrix();
   //glRotatef(lightRot, 1.0, 0.0, 0.0);
   
   // Light 0
   glLightfv( GL_LIGHT0, GL_POSITION, lightPos );
   glLightfv( GL_LIGHT0, GL_AMBIENT, Al );
   glLightfv( GL_LIGHT0, GL_DIFFUSE, Dl );
   glLightfv( GL_LIGHT0, GL_SPECULAR, Sl );
   glLightf( GL_LIGHT0, GL_CONSTANT_ATTENUATION, att[0] );
   glLightf( GL_LIGHT0, GL_LINEAR_ATTENUATION, att[1] );
   glLightf( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att[2] );

   // Light 1
   glLightfv( GL_LIGHT1, GL_POSITION, lightPos2 );
   glLightfv( GL_LIGHT1, GL_AMBIENT, Al );
   glLightfv( GL_LIGHT1, GL_DIFFUSE, Dl );
   glLightfv( GL_LIGHT1, GL_SPECULAR, Sl );
   glLightf( GL_LIGHT1, GL_CONSTANT_ATTENUATION, att[0] );
   glLightf( GL_LIGHT1, GL_LINEAR_ATTENUATION, att[1] );
   glLightf( GL_LIGHT1, GL_QUADRATIC_ATTENUATION, att[2] );
   

   glColor3f(1.0, 0.0, 0.0);
   glPushMatrix();
   glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
   glutSolidCube(1);
   glPopMatrix();
   glPushMatrix();
   glTranslatef(lightPos2[0], lightPos2[1], lightPos2[2]);
   glutSolidCube(1);
   glPopMatrix();

   glPopMatrix();
   */


}

void Scene::draw()
{
   setupCamera();

   setupLights();

   // TODO: apply transform
   glRotatef(angle, 0.0, 1.0, 1.0); //angle += 0.02;


   // enable the shader program
   activeShader->attach();

   // Render static elements
   for each (Drawable* d in m_staticObjects)
   {
      d->draw();
   }

   // Render dynamic elements
   for each (Drawable* d in m_dynamicObjects)
   {
      //if (d->id.find("Shape") != std::string::npos)
         d->draw();
   }

   glUseProgram(0);
}

void Scene::addShape(Shape* shape, bool dynamic)
{
   // dynamic object
   if (dynamic)
   {
      m_dynamicObjects.push_back(shape);
   }
   // static object
   else
   {
      // TODO: Static elements??
   }
}

void Scene::addLight(Light* l)
{
   m_lights.push_back(l);
}

void Scene::printElements()
{
   printf("Scene:\n");
   printf("\tDynamic:\n");
   for each(Shape* s in m_dynamicObjects)
   {
      printf("\t\t%s\n", s->id.c_str() );
   }
   printf("\tStatic:\n");
   for each(Shape* s in m_staticObjects)
   {
      printf("\t\t%s\n", s->id.c_str() );
   }
}
