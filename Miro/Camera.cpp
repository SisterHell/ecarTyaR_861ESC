#include <stdio.h>
#include <stdlib.h>
#include "Miro.h"
#include "Camera.h"
#include "Image.h"
#include "Scene.h"
#include "Console.h" 
#include "OpenGL.h"

Camera * g_camera = 0;

static bool firstRayTrace = true; 

const float HalfDegToRad = DegToRad/2.0f;

Camera::Camera() :
    m_bgColor(0,0,0),
    m_renderer(RENDER_OPENGL),
    m_eye(0,0,0),
	m_viewDir(0,0,-1),
    m_up(0,1,0),
    m_lookAt(FLT_MAX, FLT_MAX, FLT_MAX),
    m_fov((45.)*(PI/180.)),
	m_focalPoint(0, 0, 0)
{
    calcLookAt();
}


Camera::~Camera()
{

}


void
Camera::click(Scene* pScene, Image* pImage)
{
    calcLookAt();
    static bool firstRayTrace = false;

    if (m_renderer == RENDER_OPENGL)
    {
        glDrawBuffer(GL_BACK);
        pScene->openGL(this);
        firstRayTrace = true;
    }
    else if (m_renderer == RENDER_RAYTRACE)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDrawBuffer(GL_FRONT);
        if (firstRayTrace)
        {
            pImage->clear(bgColor());
            pScene->raytraceImage(this, g_image);
            firstRayTrace = false;
        }
        
        g_image->draw();
    }
}


void
Camera::calcLookAt()
{
    // this is true when a "lookat" is not used in the config file
    //if (m_lookAt.x != FLT_MAX)
    //{
        //setLookAt(m_lookAt);
      //  m_lookAt.set(FLT_MAX, FLT_MAX, FLT_MAX);
    //}
}


void
Camera::drawGL()
{
    // set up the screen with our camera parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov(), g_image->width()/(float)g_image->height(),
                   0.01, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vector3 vCenter = eye() + viewDir();
    gluLookAt(eye().x, eye().y, eye().z,
              vCenter.x, vCenter.y, vCenter.z,
              up().x, up().y, up().z);
}


Ray
Camera::eyeRay(int x, int y, int imageWidth, int imageHeight)
{
    // first compute the camera coordinate system 
    // ------------------------------------------

    // wDir = e - (e+m_viewDir) = -m_vView
    const Vector3 wDir = Vector3(-m_viewDir).normalize(); 
	//std::cout << "m_viewDir: " << m_viewDir.x << ", " << m_viewDir.y << ", " << m_viewDir.z << std::endl;
    const Vector3 uDir = cross(m_up, wDir).normalize(); 
    const Vector3 vDir = cross(wDir, uDir);    



    // next find the corners of the image plane in camera space
    // --------------------------------------------------------

    const float aspectRatio = (float)imageWidth/(float)imageHeight; 


    const float top     = tan(m_fov*HalfDegToRad); 
    const float right   = aspectRatio*top; 

    const float bottom  = -top; 
    const float left    = -right; 



    // transform x and y into camera space 
    // -----------------------------------

	//original
    //const float imPlaneUPos = left   + (right - left)*(((float)x+0.5f)/(float)imageWidth); 
    //const float imPlaneVPos = bottom + (top - bottom)*(((float)y+0.5f)/(float)imageHeight); 

	//for anti-aliasing
	const float imPlaneUPos = left + (right - left)*(((float)x) / (float)imageWidth);
	const float imPlaneVPos = bottom + (top - bottom)*(((float)y) / (float)imageHeight);

    return Ray(m_eye, (imPlaneUPos*uDir + imPlaneVPos*vDir - wDir).normalize());
}

Ray
Camera::DOFeyeRay(int x, int y, int imageWidth, int imageHeight)
{
	// first compute the camera coordinate system 
	// ------------------------------------------

	// wDir = e - (e+m_viewDir) = -m_vView
	const Vector3 wDir = Vector3(-m_viewDir).normalize();
	//std::cout << "m_viewDir: " << m_viewDir.x << ", " << m_viewDir.y << ", " << m_viewDir.z << std::endl;
	//Vector3 newDir = Vector3(m_eye - m_lookAt);
	//std::cout << "newDir: " << newDir.x << ", " << newDir.y << ", " << newDir.z << std::endl;
	//std::cout << "m_eye: " << m_eye.x << ", " << m_eye.y << ", " << m_eye.z << std::endl;
	//std::cout << "m_lookAt: " << lookAt().x << ", " << lookAt().y << ", " << lookAt().z << std::endl;

	const Vector3 uDir = cross(m_up, wDir).normalize();
	const Vector3 vDir = cross(wDir, uDir).normalize();

	const float distance = (m_focalPoint - m_eye).length();

	// next find the corners of the image plane in camera space
	// --------------------------------------------------------

	const float aspectRatio = (float)imageWidth / (float)imageHeight;


	const float top = tan(m_fov*HalfDegToRad);
	const float right = aspectRatio*top;

	const float bottom = -top;
	const float left = -right;



	// transform x and y into camera space 
	// -----------------------------------

	//original
	//const float imPlaneUPos = left   + (right - left)*(((float)x+0.5f)/(float)imageWidth); 
	//const float imPlaneVPos = bottom + (top - bottom)*(((float)y+0.5f)/(float)imageHeight); 

	float d;
	float dx, dy;
	do{
		dx = -1.0f + (2.0f *((double)rand() / (RAND_MAX)));
		dy = -1.0f + (2.0f *((double)rand() / (RAND_MAX)));
		d = sqrtf(dx * dx + dy * dy);
		//std::cout << "I'm stuck in random choice." << std::endl;
		//std::cout << x << std::endl;
	} while (d > 1);
	Vector3 xApertureRadius = 0.8	 * uDir;
	Vector3 yApertureRadius = 0.8 * vDir;

	//for anti-aliasing
	const float imPlaneUPos = left + (right - left)*(((float)x) / (float)imageWidth);
	const float imPlaneVPos = bottom + (top - bottom)*(((float)y) / (float)imageHeight);


	Vector3 originalDir((imPlaneUPos*uDir + imPlaneVPos*vDir - wDir).normalize());

	Vector3 focusPt = m_eye + originalDir * distance;

	//const float newImPlaneUPos = left + (right - left)*(((float)x + dx*0.2) / (float)imageWidth);
	//const float newImPlaneVPos = bottom + (top - bottom)*(((float)y + dy*0.2) / (float)imageHeight);

	//return Ray(m_eye, (imPlaneUPos*uDir + imPlaneVPos*vDir - wDir).normalize());
	//Vector3 vRight = cross(g_camera->viewDir(), g_camera->up());
	// now rotate everything
	//Vector3 v = g_camera->viewDir();
	//v.rotate(-dx * PI / 180., vRight);
	//v.rotate(-dy * PI / 180., g_camera->up());
	Vector3 newEYE = m_eye;
	newEYE += dx*xApertureRadius;
	newEYE += dy*yApertureRadius;
	//Vector3 v = m_lookAt - newEYE;
	//std::cout << "v: " << v.x << ", " << v.y << ", " << v.z << std::endl;
	//Vector3 look(-0.5, 1, 0);
	
	//TODO
	//shoot ray from newImPlaneU,VPos to the focusPt
	Vector3 ddd = focusPt - newEYE;//(imPlaneUPos*uDir + imPlaneVPos*vDir) + (m_focalPoint - m_eye).normalize();

	return Ray(newEYE, (ddd).normalize());
}