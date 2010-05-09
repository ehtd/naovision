/**
 * @author ehtd
 *
 * This file was generated by Aldebaran Robotics ModuleGenerator
 */

#ifndef NAOVISION_VISION1_H
#define NAOVISION_VISION1_H
/*
#include <alptr.h>
#include <alloggerproxy.h>
#include <almodule.h>
#include "opencv/highgui.h"
#include <string>
#include "alimage.h"
 */
#include "alloggerproxy.h"
#include "alptr.h"
#include <string>
#include "alvision/alimage.h"
#include "opencv/highgui.h"
#include "opencv/cv.h"

struct Object{
       int Rmin;
       int Gmin;
       int Bmin;
       int Rmax;
       int Gmax;
       int Bmax;
    };

static const Object BALL = {20,100,100,170,150,220};

namespace AL
{
class ALBroker;
}

/**
 * DESCRIBE YOUR CLASS HERE
 */
class vision1 : public AL::ALModule
{

public:

	/**
	 * Default Constructor.
	 */
	vision1(AL::ALPtr<AL::ALBroker> pBroker, const std::string& pName );

	/**
	 * Destructor.
	 */
	virtual ~vision1();

	void setup();

	void registerToVIM(const int &pResolution, const int &pColorSpace);

	void unRegisterFromVIM();

	void getImage();

	void saveImage(const std::string& );

	void saveImage(const std::string& , IplImage* );

	void processBall(const std::string& );

	//IplImage* detectBall();

	void detectBall();

	void findColor();

	void releaseImage();

	void takePicture(const std::string& );

	void init();

	void write();

	//    void detectBall(IplImage* img1);
	/**
	 * dummy Function. An autogenerated example method.
	 * @param pMsg Message to show on screen
	 * @param pFoo the function will return this parameter
	 * @return an alvalue\n
	 */
	AL::ALValue dummyFunction(const std::string& pMsg, const AL::ALValue& pFoo );

private:

	// Proxy to the logger module.
	AL::ALPtr<AL::ALLoggerProxy> fLogProxy;

	// Proxy to the video input module.
	AL::ALPtr<AL::ALProxy> fCamProxy;

	// This is set to true when we have subscribed one module to the VideoDevice.
	bool fRegisteredToVim;

	std::string fGvmName;

	// Just a IplImage header to wrap around our camera image buffer.
	// This object doesn't own the image buffer.
	IplImage* img;

	AL::ALImage* imageIn;


};

#endif  // NAOVISION_VISION1_H

