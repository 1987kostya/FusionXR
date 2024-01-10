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
typedef short(__stdcall* CFile_Open_t)(void* rdPtr);
static CFile_Open_t CFile_Open_o;
static HWND glfwWindowHwnd;
static int shit[3] = { 0 }; // сделал глобальным для __asm
void gayBeginScene();
void gayEndScene();
void gayDrawAll();

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


