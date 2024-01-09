#pragma once

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
static void* lastRdata;
static HWND glfwWindowHwnd;
void RetardDraw();
static int shit[3] = { 0 }; // сделал глобальным для __asm
void gayBeginScene();
void gayEndScene();
void gayDrawAll();


