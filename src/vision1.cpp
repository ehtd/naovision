/**
 * @author ehtd
 *
 * Version : $Id$
 * This file was generated by Aldebaran Robotics ModuleGenerator
 */

#include <alvalue.h>
#include <alptr.h>
#include <albroker.h>
#include <almodule.h>
#include <string.h>
#include <stdlib.h>

#include "vision1.h"
#include <iostream>
#include <stdio.h>
#include "alproxy.h"

#include "alvision/alvisiondefinitions.h"




//______________________________________________
// constructor
//______________________________________________
vision1::vision1(AL::ALPtr<AL::ALBroker> broker, const std::string& name ): 
AL::ALModule(broker, name ),
fRegisteredToVim(false),
img(NULL),
ball_area(0),
ball_centroid_x(0),
ball_centroid_y(0)
{
	setModuleDescription( "This is an autogenerated module, this descriptio needs to be updated. " );

	functionName( "takePicture", "vision1" ,  "Takes a picture and saves it." );
	addParam( "imgName", "Image name" );
	BIND_METHOD( vision1::takePicture );

	functionName( "registerToVIM", "vision1" ,  "Takes a picture and saves it." );
	addParam( "imgName", "Image name" );
	BIND_METHOD( vision1::registerToVIM );

	functionName( "init", "vision1" ,  "Takes a picture and saves it." );
	addParam( "imgName", "Image name" );
	BIND_METHOD( vision1::init );

	functionName( "processBall", "vision1" ,  "Takes a picture finds ball and saves it." );
	addParam( "imgName", "Image name" );
	BIND_METHOD( vision1::processBall );

	functionName( "saveData", "vision1" ,  "Takes a picture finds ball and saves it." );
	addParam( "imgName", "Image name" );
	BIND_METHOD( vision1::saveData );

	functionName( "stiffnessOn", getName(), "Set the stiffness on for all joints" );
	BIND_METHOD( vision1::stiffnessOn );

	functionName( "stiffnessOff", getName(), "Set the stiffness off for all joints" );
	BIND_METHOD( vision1::stiffnessOff );

	functionName( "trackBall", getName(), "Method for tracking the ball" );
	BIND_METHOD( vision1::trackBall );

	functionName( "initPos", getName(), "Set the init pose on the robot" );
	BIND_METHOD( vision1::initPos );

	fov = 0.8;
	  vfov = 0.6;

	  ballDirectionAngle = UNKNOWN;
	  ballElevationAngle = UNKNOWN;

}

//______________________________________________
// destructor
//______________________________________________
vision1::~vision1()
{
	// Release Image Header if client code forgot to call unregisterFromVim.
	if (img)
		cvReleaseImage(&img);
}

void vision1::init() {
	// Create a proxy to the logger module.
	// If this fails, we throw an exception and the module will not be registered.
	try {
		fLogProxy = getParentBroker()->getLoggerProxy();
	} catch (const AL::ALError& e) {
		throw AL::ALError(getName(), "Vision()", "Fail to create a proxy to ALLogger "
				"module. Msg " + e.toString() + "\nModule will abort.");
	}

	  // Create a proxy to the video input module.
	  try {
	    fMotionProxy = getParentBroker()->getMotionProxy();
	  } catch (const AL::ALError& e) {
	    throw AL::ALError(getName(), "Vision()", "Could not create a proxy to ALMotion " + e.toString());
	  }

	// Create a proxy to the video input module.
	try {
		fCamProxy = getParentBroker()->getProxy( "ALVideoDevice" );
	} catch (const AL::ALError& e) {
		throw AL::ALError(getName(), "Vision()", "Could not create a proxy to ALVideoDevice " + e.toString());
	}

}

/**
 * registerToVIM
 */
void vision1::registerToVIM(const int &pResolution, const int &pColorSpace) {

	// If we've already registered a module, we need to unregister it first !
	if (fRegisteredToVim) {
		throw AL::ALError(getName(), "registerToVIM()", "A video module has already been "
				"registered. Call unRegisterFromVIM() before trying to register a new module.");
	}

	// GVM Name that we're going to use to register.
	const std::string kOriginalName = "Borregos_VIM";
	int imgWidth = 0;
	int imgHeight = 0;
	int imgNbLayers = 0;
	const int kImgDepth = 8;
	const int kFps = 30;

	// Release Image Header if it has been allocated before.
	if (img)
		cvReleaseImage(&img);

	AL::getSizeFromResolution(pResolution, imgWidth, imgHeight);
	imgNbLayers = AL::getNumLayersInColorSpace(pColorSpace);

	if (imgWidth == -1 || imgWidth == -1 || imgNbLayers == -1) {
		throw AL::ALError(getName(), "registerToVIM()", "Invalid resolution or color space.");
	}


	// Allocate our Image header.
	img = cvCreateImage(cvSize(imgWidth, imgHeight), kImgDepth,imgNbLayers);
	/*
  if (!img) {
    throw AL::ALError(getName(), "registerToVIM()", "Fail to allocate OpenCv image header.");
  }*/

	// Call the "subscribe" function with the given parameters.
	fGvmName = fCamProxy->call<std::string>("subscribe", kOriginalName,
			pResolution, pColorSpace, kFps );

	fLogProxy->info(getName(), " module registered as " + fGvmName);

	// Registration is successful, set fRegisteredToVim to true.
	fRegisteredToVim = true;
}





/**
 * unRegisterFromVIM
 */
void vision1::unRegisterFromVIM() {

	if (!fRegisteredToVim) {
		throw AL::ALError(getName(), "unRegisterFromVIM()", "No video module is currently "
				"registered! Call registerToVIM first.");
	}

	// Release Image Header if it has been allocated.
	if (img)
		cvReleaseImage(&img);

	fLogProxy->info(getName(), "try to unregister " + fGvmName + " module." );
	fCamProxy->callVoid("unsubscribe", fGvmName);
	fLogProxy->info(getName(), "Done.");

	// UnRegistration is successful, set fRegisteredToVim to false.
	fRegisteredToVim = false;
}

void vision1::stiffnessOn(){
   AL::ALValue fNameJoint;
   AL::ALValue stiffness = AL::ALValue(1.0f);
   fNameJoint.arrayPush(AL::ALValue("Body"));
   fMotionProxy->setStiffnesses(fNameJoint, stiffness);
}

void vision1::stiffnessOff(){
   AL::ALValue fNameJoint;
   AL::ALValue stiffness = AL::ALValue(0.0f);
   fNameJoint.arrayPush(AL::ALValue("Body"));
   fMotionProxy->setStiffnesses(fNameJoint, stiffness);
}


void vision1::getImage(){
	if (!fRegisteredToVim) {
		throw AL::ALError(getName(), "saveImage()",  "No video module is currently "
				"registered! Call registerToVIM() first.");
	}

	imageIn = NULL;

	// Now you can get the pointer to the video structure.
	imageIn = (AL::ALImage*) (fCamProxy->call<int>("getImageLocal", fGvmName));

	if (!imageIn) {
		throw AL::ALError(getName(), "saveImage", "Invalid image returned.");
	}


	//fLogProxy->info(getName(), imageIn->toString());

	// Set the buffer we received to our IplImage header.
	img->imageData = (char*)imageIn->getFrame();
	/*
  int imgWidth = imageIn->fWidth;
  int imgHeight = imageIn->fHeight;
  int imgNbLayers = imageIn->fNbLayers;

  img = cvCreateImage(cvSize(imgWidth, imgHeight), 8 ,imgNbLayers);*/

}

void vision1::saveImage(const std::string& path){
	try {
		const int seconds = (int)(imageIn->fTimeStamp/1000000LL);
		const std::string kImageNameFull = path + DecToString(seconds) + ".jpg";
		cvSaveImage(kImageNameFull.c_str(), img);
		//		 detectBall(img);
		fLogProxy->info(getName(), "Image saved as foto.jpg");
	}
	catch(...) {
		throw AL::ALError(getName(), "saveIplImage()", "OpenCV can't save the image with "
				"this format.");
	}
}

void vision1::saveImage(const std::string& path, IplImage* imagen){
	try {
		const int seconds = (int)(imageIn->fTimeStamp/1000000LL);
		const std::string kImageNameFull = path + DecToString(seconds) + ".jpg";
		cvSaveImage(kImageNameFull.c_str(), imagen);
		//		 detectBall(img);
		fLogProxy->info(getName(), "Image saved as foto.jpg");
	}
	catch(...) {
		throw AL::ALError(getName(), "saveIplImage()", "OpenCV can't save the image with "
				"this format.");
	}
}

void vision1::write(){
	FILE *pFile;
	char buffer[] = {'x','y','z'};
	pFile = fopen ("myfile.bin","wb");
	fwrite(buffer ,1, sizeof(buffer),pFile);
	fclose(pFile);
}


//IplImage* vision1::detectBall(IplImage* img1){
//			IplImage* dst = cvCreateImage(cvSize(img1->width, img1->height), img1->depth, img1->nChannels);
//			//cvPyrMeanShiftFiltering(frame,segmented,10,70);
//			CvMemStorage* storage = cvCreateMemStorage(0);
//			CvSeq* comp = NULL;
//			cvPyrSegmentation(img1,dst,storage,&comp,4,150,30);
//			int n_comp = comp->total;
//			for( int i=0; i<n_comp; i++ ) {
//				CvConnectedComp* cc = (CvConnectedComp*) cvGetSeqElem( comp, i );
//				if (cc->value.val[0] < 100 && cc->value.val[0] > 20 && cc->value.val[1] > 100 && cc->value.val[1] < 170 && cc->value.val[2] > 150 && cc->value.val[2] < 220){
//					fLogProxy->info(getName(), "Pelota encontrada");
//					CvPoint pt1 = cvPoint(cc->rect.x,cc->rect.y);
//					CvPoint pt2 = cvPoint(cc->rect.x+cc->rect.width,cc->rect.y+cc->rect.height);
//					cvRectangle(dst,pt1,pt2,CV_RGB(255,0,0));
////					cvSaveImage("/home/nao/data/foto.jpg", img1);
//					//printf("%f - %f %f %f \n",cc->area,cc->value.val[0],cc->value.val[1],cc->value.val[2]);
//				}
//			}
//			cvReleaseMemStorage( &storage );
//}


void vision1::releaseImage(){
	cvReleaseImage(&img);
}

void vision1::takePicture(const std::string& path){
	init();
	fLogProxy->info(getName(), "registerToVIM");
	registerToVIM(AL::kVGA, AL::kRGBColorSpace);
	fLogProxy->info(getName(), "getImage");
	getImage();
	fLogProxy->info(getName(), "saveImage");
	saveImage(path);
	fLogProxy->info(getName(), "releaseImage");
	releaseImage();
	fLogProxy->info(getName(), "unRegisterFromVIM");
	unRegisterFromVIM();
}

void vision1::processBall(const std::string& path){
	init();
	fLogProxy->info(getName(), "registerToVIM");
	registerToVIM(AL::kQQVGA, AL::kHSYColorSpace);
	fLogProxy->info(getName(), "getImage");
	getImage();
	fLogProxy->info(getName(), "detectBall");
	detectBall();
	fLogProxy->info(getName(), "Erode");
	erode();
	fLogProxy->info(getName(), "Dilate");
	dilate();
	fLogProxy->info(getName(), "getBallInfo");
	getBallInfo();
	fLogProxy->info(getName(), "trackBall");
	trackBall();
	fLogProxy->info(getName(), "saveImage");
	saveImage(path);
	fLogProxy->info(getName(), "releaseImage");
	releaseImage();
	fLogProxy->info(getName(), "unRegisterFromVIM");
	unRegisterFromVIM();
}



void vision1::detectBall(){
	for( int y=0; y<img->height; y++ ) {
		uchar* ptr = (uchar*) (img->imageData + y * img->widthStep);
		for( int x=0; x<img->width; x++ ) {
			if (ptr [3*x+0]< 70 and ptr [3*x+0]> 10 /*and ptr [3*x+1]> 70*/){
				ptr[3*x+0] = 30;
				ptr[3*x+1] = 255;
				ptr[3*x+2] = 255;
			}
			else{
				ptr[3*x+0] = 0;
				ptr[3*x+1] = 0;
				ptr[3*x+2] = 0;
			}
		}
	}
}


double vision1::M(int p, int q, int id)
{
	double momento = 0;
	int num = 0;

	for( int y=0; y<img->height; y++ ) {
		uchar* ptr = (uchar*) (img->imageData + y * img->widthStep);
		for( int x=0; x<img->width; x++ ) {
			if (ptr[3*x+0] > 0/*id*/ )
			{
				num = 1;
			}
			else { num = 0; }

			momento += pow(x, p) * pow(y, q) * num;
		}
	}
	return momento;
}

void vision1::erode(){

	IplImage* dst = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);

	cvErode( img,dst, NULL, 4);
//deallocate old img?
	img = dst;
}

void vision1::dilate(){

	IplImage* dst = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);

	cvDilate( img, dst, NULL, 4);

//deallocate old img?
	img = dst;
}



void vision1::getBallInfo(){

	  int height     = img->height;
	  int width      = img->width;

	ball_area = M(0,0,30);

	if( ball_area > 0){
		ball_centroid_x= (M(1,0,30)/ball_area);
		ball_centroid_y= (M(0,1,30)/ball_area);
	    ballDirectionAngle = (ball_centroid_x / width - 0.5) * fov;
	    ballElevationAngle = -(ball_centroid_y / height - 0.5) * vfov;
	    fLogProxy->info(getName(), "Ball detected" );
	}
	std::cout << "Area: " << ball_area << std::endl;
	std::cout << "x : " << ball_centroid_x << std::endl;
	std::cout << "y : " << ball_centroid_y << std::endl;


	uchar* ptr = (uchar*) (img->imageData + (int)ball_centroid_y * img->widthStep);
	ptr[3*(int)(ball_centroid_x)+0] = 0;
}

void vision1::saveData(const std::string& pName) {

	FILE *output;
   const char* fileName = pName.c_str();
   output = fopen(fileName,"w");

   double a = 5.5;
   fprintf(output,"Area:%f ",ball_area);
   fprintf(output,"\n");
   fprintf(output,"X:%f ",ball_centroid_x);
   fprintf(output,"\n");
   fprintf(output,"Y:%f ",ball_centroid_y);
   fprintf(output,"\n");
   fprintf(output,"ballDirectionAngle:%f ",ballDirectionAngle);
   fprintf(output,"\n");
   fprintf(output,"ballElevationAngle:%f ",ballElevationAngle);
   fprintf(output,"\n");


   fclose(output);
}

void vision1::trackBall(){

   double ballDir = ballDirectionAngle;
   double ballElev = ballElevationAngle;

   if (ballDir == UNKNOWN) return;

   fLogProxy->info(getName(),"Tracking");

   std::vector<float> PitchSensor = fMotionProxy->getAngles(AL::ALValue("HeadPitch"), true);
   std::vector<float> YawSensor  = fMotionProxy->getAngles(AL::ALValue("HeadYaw"), true);

   double pitch = PitchSensor.at(0) - ballElev;
   double yaw = YawSensor.at(0) - ballDir;

   printf("%4.2f, %4.2f, %4.2f \n",YawSensor.at(0),ballDir,yaw);

   AL::ALValue fNameJoint;
   AL::ALValue fAngleJoint;
   fNameJoint.arrayPush(AL::ALValue("HeadYaw"));
   fNameJoint.arrayPush(AL::ALValue("HeadPitch"));
   fAngleJoint.arrayPush(AL::ALValue(yaw));
   fAngleJoint.arrayPush(AL::ALValue(pitch));
   const float fractionMaxSpeed  = 0.3f;
   fMotionProxy->setAngles(fNameJoint, fAngleJoint, fractionMaxSpeed);
}


/**
 * dummy Function
 * @param pMsg Message to show on screen
 * @param pNawak the function will return this parameter
 * @return an alvalue
 */
AL::ALValue vision1::dummyFunction( const std::string& pMsg, const AL::ALValue& pFoo )
{
	std::cout << "vision1 say : " << pMsg << std::endl;
	if( pFoo.isValid() )
		std::cout << "Return value: " << pFoo.toString( AL::VerbosityMini ) << std::endl;
	else
		std::cout << "The value you send me is invalid" << std::endl;

	return pFoo;
}

void vision1::initPos(){

   AL::ALValue names, times, keys;
   names.arraySetSize(21);
   times.arraySetSize(21);
   keys.arraySetSize(21);

   names[0] = "HeadYaw";
   times[0].arraySetSize(1);
   keys[0].arraySetSize(1);

   times[0][0] = 2;
   keys[0][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[1] = "HeadPitch";
   times[1].arraySetSize(1);
   keys[1].arraySetSize(1);

   times[1][0] = 2;
   keys[1][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[2] = "LShoulderPitch";
   times[2].arraySetSize(1);
   keys[2].arraySetSize(1);

   times[2][0] = 2;
   keys[2][0] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[3] = "LShoulderRoll";
   times[3].arraySetSize(1);
   keys[3].arraySetSize(1);

   times[3][0] = 2;
   keys[3][0] = AL::ALValue::array(0.349066, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[4] = "LElbowYaw";
   times[4].arraySetSize(1);
   keys[4].arraySetSize(1);

   times[4][0] = 2;
   keys[4][0] = AL::ALValue::array(-1.39626, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[5] = "LElbowRoll";
   times[5].arraySetSize(1);
   keys[5].arraySetSize(1);

   times[5][0] = 2;
   keys[5][0] = AL::ALValue::array(-1.0472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[6] = "RShoulderPitch";
   times[6].arraySetSize(1);
   keys[6].arraySetSize(1);

   times[6][0] = 2;
   keys[6][0] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[7] = "RShoulderRoll";
   times[7].arraySetSize(1);
   keys[7].arraySetSize(1);

   times[7][0] = 2;
   keys[7][0] = AL::ALValue::array(-0.349066, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[8] = "RElbowYaw";
   times[8].arraySetSize(1);
   keys[8].arraySetSize(1);

   times[8][0] = 2;
   keys[8][0] = AL::ALValue::array(1.39626, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[9] = "RElbowRoll";
   times[9].arraySetSize(1);
   keys[9].arraySetSize(1);

   times[9][0] = 2;
   keys[9][0] = AL::ALValue::array(1.0472, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[10] = "LHipYawPitch";
   times[10].arraySetSize(1);
   keys[10].arraySetSize(1);

   times[10][0] = 2;
   keys[10][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[11] = "LHipRoll";
   times[11].arraySetSize(1);
   keys[11].arraySetSize(1);

   times[11][0] = 2;
   keys[11][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[12] = "LHipPitch";
   times[12].arraySetSize(1);
   keys[12].arraySetSize(1);

   times[12][0] = 2;
   keys[12][0] = AL::ALValue::array(-0.436332, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[13] = "LKneePitch";
   times[13].arraySetSize(1);
   keys[13].arraySetSize(1);

   times[13][0] = 2;
   keys[13][0] = AL::ALValue::array(0.698132, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[14] = "LAnklePitch";
   times[14].arraySetSize(1);
   keys[14].arraySetSize(1);

   times[14][0] = 2;
   keys[14][0] = AL::ALValue::array(-0.349066, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[15] = "LAnkleRoll";
   times[15].arraySetSize(1);
   keys[15].arraySetSize(1);

   times[15][0] = 2;
   keys[15][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[16] = "RHipRoll";
   times[16].arraySetSize(1);
   keys[16].arraySetSize(1);

   times[16][0] = 2;
   keys[16][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[17] = "RHipPitch";
   times[17].arraySetSize(1);
   keys[17].arraySetSize(1);

   times[17][0] = 2;
   keys[17][0] = AL::ALValue::array(-0.436332, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[18] = "RKneePitch";
   times[18].arraySetSize(1);
   keys[18].arraySetSize(1);

   times[18][0] = 2;
   keys[18][0] = AL::ALValue::array(0.698132, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[19] = "RAnklePitch";
   times[19].arraySetSize(1);
   keys[19].arraySetSize(1);

   times[19][0] = 2;
   keys[19][0] = AL::ALValue::array(-0.349066, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   names[20] = "RAnkleRoll";
   times[20].arraySetSize(1);
   keys[20].arraySetSize(1);

   times[20][0] = 2;
   keys[20][0] = AL::ALValue::array(0.0, AL::ALValue::array(2, -0.666667, -0.0), AL::ALValue::array(2, 0.0, 0.0));

   fMotionProxy->post.angleInterpolationBezier(names, times, keys);

}
