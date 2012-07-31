/**
 * Main file for the "eclps" demo.
 */
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string>
#include "x3d/X3dParser.h"
#include "element/Scene.h"
#include "texlib/TextureLibrary.h"
#include "shadelib/Shader.h"
#include "util/text.h"
#include "settings.h"
#include "util/ScreenCap.h"

//#define _SCREEN_CAP_ // capture screen
#define CAPTURE_FILENAME "imgs\\test"

// Screen dimensions
int sWidth = 800;
int sHeight = 600;

// offscreen texture width
const int texWidth = 1024;
const int texHeight = 1024;

// shader and FBO
Shader flatShader;
Shader phongShader;
GLuint fbo;
GLuint depthBuffer;
GLuint imgTexture;

// Scene object
Scene *scene;

const GLfloat bgColor[] = { 0.0, 0.0, 0.0, 1.0 };




void init();
void initFBO();
void renderTextures(GLuint);
void reshape(int, int);
void display();
void idle();


void init()
{
   glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
   glutInitWindowSize(sWidth, sHeight);
   glutCreateWindow( "eclps" );

   //glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);
   glShadeModel(GL_SMOOTH);

   GLenum status = glewInit();
   if (status != GLEW_OK)
      fprintf(stderr, "ERROR: OpenGL 2.0 not supported!\n");
   else
      fprintf(stderr, "OpenGL 2.0 supported!\n");

   // reset the viewport
   reshape(sWidth, sHeight);
}

void initFBO()
{
   // grab the texture lib and disable mipmaps.. silly ATI
   TextureLibrary *texLib = TextureLibrary::getInstance();
   texLib->DisableMipmaps();

   GLint maxBuffers;
   glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxBuffers);
   fprintf(stderr, "MAX_COLOR_ATTACHMENTS: %d\n", maxBuffers);

   // setup the FBO
   glGenFramebuffersEXT(1, &fbo);
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);

   // create the render buffer for depth
   glGenRenderbuffersEXT(1, &depthBuffer);
   glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
   glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, texWidth, texHeight);
   // bind the render buffer
   glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);

   // Image Texture
   imgTexture = texLib->Load("imgTex");
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
   glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texLib->getGLID(imgTexture), 0);

   // define the render targets
   GLenum mrt[] = {GL_COLOR_ATTACHMENT0_EXT};
   glDrawBuffers(1, mrt);

   // check FBO status
   GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
   if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
   {
      fprintf(stderr, "ERROR: initFBO: FBO status not completed!\n");
      exit(1);
   }

   // unbind the FBO

   // Initialize the shaders
   flatShader.addFrag(SHADER_DIR("flat.fs"));
   flatShader.addVert(SHADER_DIR("mrt.vs"));
   flatShader.addFrag(SHADER_DIR("mrt.fs"));
   flatShader.build();
   flatShader.printProgramInfoLog();

   phongShader.addFrag(SHADER_DIR("phong.fs"));
   phongShader.addVert(SHADER_DIR("mrt.vs"));
   phongShader.addFrag(SHADER_DIR("mrt.fs"));
   phongShader.build();
   phongShader.printProgramInfoLog();
}

void renderTextures(GLuint fb)
{
   // bind the FBO for rendering
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

   // setup the projection matrix
   glPushAttrib(GL_VIEWPORT_BIT);
   glViewport(0, 0, texWidth, texHeight);
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   gluPerspective(65, (float)sWidth/(float)sHeight, 1.0, 5000.0);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();


   // render to the FBO
   glClearColor( bgColor[0], bgColor[1], bgColor[2], bgColor[3] );
   glClearDepth(1.0f);
   glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   glLoadIdentity();



   //--------------------------------------------------------------------------
   // Render the scene
   scene->draw();
   //glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
   //glutSolidCone(1.0f, 1.0f, 10.0f, 10.0f);
   //glTranslatef(0.7, 0.0, 0.0);
   //glutSolidCube(1.0);
   //glTranslatef(-1.4, 0.0, 0.0);
   //glutSolidSphere(0.5, 10, 10);
   //
   //--------------------------------------------------------------------------

   // disable the shader program
   glUseProgram(0);

   // restore old viewport and projection
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   glPopAttrib();

   // disable framebuffer
   glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void reshape(int w, int h)
{
   sWidth = w;
   sHeight = h;

   glViewport( 0, 0, sWidth, sHeight );
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D( -100, 100, -100, 100 );
   //if ( h == 0 )
   //   gluPerspective(45, (float)sWidth, 1.0, 5000.0);
   //else
   //   gluPerspective(45, (float)sWidth/(float)sHeight, 1.0, 5000.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void idle()
{
   glutPostRedisplay();
}

void display()
{
   renderTextures(fbo);

   TextureLibrary *texLib = TextureLibrary::getInstance();

   // Texture Frame color
   glClearColor(0.5, 0.5, 0.5, 0.5);
   glClearDepth(1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();

   glColor4f(1.0, 1.0, 1.0, 1.0);
   glUseProgram(0);

   // draw the screen quad
   texLib->Activate(imgTexture);
   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex2f(-99.0, -99.0);
      glTexCoord2f(1.0f, 0.0f); glVertex2f( 99.0, -99.0);
      glTexCoord2f(1.0f, 1.0f); glVertex2f( 99.0,  99.0);
      glTexCoord2f(0.0f, 1.0f); glVertex2f(-99.0,  99.0);
   glEnd();
   texLib->Deactivate(imgTexture);

   glColor3f(1.0, 1.0, 0.1);
   //fps();

   glutSwapBuffers();

   // Screen capture
#ifdef _SCREEN_CAP_
   tgaGrabScreenSeries(CAPTURE_FILENAME, 0, 0, sWidth, sHeight);
#endif
}

void keyboard( unsigned char key, int x, int y )
{
   switch (key)
   {
      // ESC
      case 27:
         exit(0);
         break;
      case 'q':
         scene->activeShader = &flatShader;
         break;
      case 'w':
         scene->activeShader = &phongShader;
         break;
      case 'g':
         scene->startMove = true;
         break;
      default:
         break;
   }
}

void setupScene()
{
   // Parse the scene
   X3dParser *parser = new X3dParser();
   scene = parser->parseFile(SCENE_FILENAME);
   delete parser;

   if (!scene)
   {
      fprintf(stderr, "ERROR: Failed to load scene!\n");
      exit(1);
   }

   scene->activeShader = &phongShader;

   // lights
   Light* L1 = new Light( Vec3f(-200.0f, 10.0f, -80.0f), // position
                        Vec3f(0.5f, 1.0f, 0.5f), // diffuse
                        Vec3f(0.0f, 0.0f, 0.0f), // ambient
                        Vec3f(0.5f, 1.0f, 0.5f), // specular
                        1.0f,     // constant att
                        0.0f,     // constant att
                        0.00001f,     // constant att
                        Vec3f(0.0, 1.0, 0.0), // spotDir
                        50.0,      // inner cutoff
                        30.0);    // outer cutoff
   Light* L2 = new Light( Vec3f(200.0f, 10.0f, -100.0f), // position
                        Vec3f(1.0f, 0.5f, 0.5f), // diffuse
                        Vec3f(0.0f, 0.0f, 0.0f), // ambient
                        Vec3f(1.0f, 0.5f, 0.5f), // specular
                        1.0,     // constant att
                        0.0,     // constant att
                        0.00001f,     // constant att
                        Vec3f(0.0, 0.0, 0.0), // spotDir
                        180.0,      // inner cutoff
                        180.0);    // outer cutoff
   Light* L3 = new Light( Vec3f(0.0f, 500.0f, 0.0f), // position
                        Vec3f(0.0f, 0.0f, 0.0f), // diffuse
                        Vec3f(0.1f, 0.1f, 0.1f), // ambient
                        Vec3f(0.0f, 0.0f, 0.0f), // specular
                        1.0,     // constant att
                        0.0,     // constant att
                        0.0,     // constant att
                        Vec3f(0.0, 0.0, 0.0), // spotDir
                        180.0,      // inner cutoff
                        180.0);    // outer cutoff


               

   scene->addLight(L1);
   L1->m_enabled = false;
   scene->addLight(L2);
   L2->m_enabled = false;
   scene->addLight(L3);
}


int main(int argc, char* argv[])
{
   // initialize
   glutInit(&argc, argv);
   init();
   // TODO: init FBO
   initFBO();

   // register the callback functions
   glutDisplayFunc( display );
   glutReshapeFunc( reshape );
   glutKeyboardFunc( keyboard );
   glutIdleFunc( idle );

   setupScene();

   glutMainLoop();

   return 0;
}