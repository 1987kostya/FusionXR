#pragma once
#include <openxr/openxr.h>

extern int lastRdata;
extern bool g_sessionRunning; 
extern bool renderForPc;
typedef void(__thiscall* setTransform_t)(int*, int, int*);
extern setTransform_t setTransform_o;
extern XrCompositionLayerProjectionView currentLayerInfo;
void OpenXrStartSession();
void OpenXrStopSession();

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


struct CameraPos
{
	float xPos;
	float yPos;
	float zPos;
	float xRot;
	float yRot;
	float zRot;
};
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
	CameraPos ogCameraData;

	

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
#define PATTERN_DISPLAY_RUN_OBJECT "E9 ?? ?? ?? ?? 68 ?? ?? ?? ?? 64 A1"
#define PATTERN_BEGIN_SCENE "55 8B EC F3 0F 10 45 10 56 FF 75 1C"
#define PATTERN_END_SCENE "56 8B F1 E8 ?? ?? ?? ?? FF 15 ?? ?? ?? ?? 8B 8E ?? ?? ?? ??"
#define PATTERN_DRAW_ALL "55 8B EC 6A FF 68 ?? ?? ?? ?? 64 A1 ?? ?? ?? ?? 50 64 89 25 ?? ?? ?? ?? 81 EC 14 01 00 00 56 8B F1"
#define PATTERN_SET_TRANSFORM "55 8B EC 56 8B F1 8B 4D 08 8B C1 C1 E0 06"
#define PATTERN_CAMERA_RENDER "56 57 8B F9 8B 07 FF 90 ?? ?? ?? ?? 8B 8F ?? ?? ?? ?? 8B 01"

#define USE_FIREFLY_BETA

#ifdef USE_FIREFLY_BETA
#define OFF_DISPLAY_RUN_OBJECT 0x7C290
#define OFF_BEGIN_SCENE 0xC6DA0
#define OFF_END_SCENE 0xCA9C0
#define OFF_DRAW_ALL 0x113080
#define OFF_SET_TRANSFORM 0xD0370
#define OFF_CAMERA_RENDER 0x1FB9F0
#else
#define OFF_DISPLAY_RUN_OBJECT 0x77890
#define OFF_BEGIN_SCENE 0xAD140
#define OFF_END_SCENE 0xB0E30
#define OFF_DRAW_ALL 0x111730
#define OFF_SET_TRANSFORM 0xB5F90
#define OFF_CAMERA_RENDER 0x1E1230
#endif
// THOSE ARE NOT ACTUALLY STATIC. DON'T USE THEM ANYWHERE OTHER THAN IN THE RUNTIME.CPP FILE
static void* displayRunObject_p;
static void* beginScene_p;
static void* endScene_p;
static void* drawAll_p;
static void* setTransform_p;
static void* cameraRender_p;
