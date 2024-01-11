#pragma once
#include <openxr/openxr.h>

extern int lastRdata;
extern bool g_sessionRunning; 
extern bool renderForPc;
typedef void(__thiscall* setTransform_t)(int*, int, int*);
extern setTransform_t setTransform_o;
extern XrCompositionLayerProjectionView currentLayerInfo;
void OpenXRCreateInstance();
void OpenXRInitializeSystem();
void OpenXRInitializeSession();
void OpenXRCreateSwapchains();
void OpenXRTearDown();
void OpenXRPollEvents(bool* exitRenderLoop, bool* requestRestart);
void OpenXRPollActions();
void OpenXRRenderFrame();
typedef short(__stdcall* FileflyDisplayRunObject_t)(void* rdPtr);
static FileflyDisplayRunObject_t FireflyDisplayRunObject_o;
static HWND glfwWindowHwnd;
static int shit[3] = { 0 }; // сделал глобальным для __asm
void wrapBeginScene();
void wrapEndScene();
void wrapDrawAll();

struct FusionXRHandData
{
	float xPos;
	float yPos;
	float zPos;
	float xRot;
	float yRot;
	float zRot;
	bool primaryButton;
	bool secondaryButton;
	float xAxis;
	float yAxis;
	float trigger;
	float grip;
};
struct FusionXRData
{
	float headXPos;
	float headYPos;
	float headZPos;
	float headXRot;
	float headYRot;
	float headZRot;

	FusionXRHandData leftHand;
	FusionXRHandData rightHand;

};
extern FusionXRData fusionXrData;
const float RADTODEG = 180 / 3.14159265358979323846264338;
static void euler_to_quaternion(float yaw, float pitch, float roll, float* x, float* y, float* z, float* w) 
{
	*x = sin(roll / 2) * cos(pitch / 2) * cos(yaw / 2) - cos(roll / 2) * sin(pitch / 2) * sin(yaw / 2);
	*y = cos(roll / 2) * sin(pitch / 2) * cos(yaw / 2) + sin(roll / 2) * cos(pitch / 2) * sin(yaw / 2);
	*z = cos(roll / 2) * cos(pitch / 2) * sin(yaw / 2) - sin(roll / 2) * sin(pitch / 2) * cos(yaw / 2);
	*w = cos(roll / 2) * cos(pitch / 2) * cos(yaw / 2) + sin(roll / 2) * sin(pitch / 2) * sin(yaw / 2);	
}
static void quaternion_to_euler(float x, float y, float z, float w, float* yaw, float* pitch, float* roll)
{
	float t0 = +2.0 * (w * x + y * z);
	float t1 = +1.0 - 2.0 * (x * x + y * y);
	*roll = atan2(t0, t1)* RADTODEG;
	float t2 = +2.0 * (w * y - z * x);
	if (t2 > 1) t2 = +1.0;

	if (t2 < -1) t2 = -1;
	*pitch = asin(t2)* RADTODEG;
	float t3 = +2.0 * (w * z + x * y);
	float t4 = +1.0 - 2.0 * (y * y + z * z);
	*yaw = atan2(t3, t4)* RADTODEG;
}
	

