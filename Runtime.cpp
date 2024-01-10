// ============================================================================
//
// This file contains routines that are handled during the Runtime.
//
// Including creating, display, and handling your object.
// 
// ============================================================================

// Common Include
#pragma once
#include	"fcommon.h"
#include "fusionxr.h"
#include "detours.h"
#include <xr_linear.h>

// DEBUGGER /////////////////////////////////////////////////////////////////

#if !defined(RUN_ONLY)
// Identifiers of items displayed in the debugger
enum
{
// Example
// -------
//	DB_CURRENTSTRING,
//	DB_CURRENTVALUE,
//	DB_CURRENTCHECK,
//	DB_CURRENTCOMBO
};

// Items displayed in the debugger
WORD DebugTree[]=
{
// Example
// -------
//	DB_CURRENTSTRING|DB_EDITABLE,
//	DB_CURRENTVALUE|DB_EDITABLE,
//	DB_CURRENTCHECK,
//	DB_CURRENTCOMBO,

	// End of table (required)
	DB_END
};

#endif // !defined(RUN_ONLY)


// --------------------
// GetRunObjectDataSize
// --------------------
// Returns the size of the runtime datazone of the object
// 
ushort WINAPI DLLExport GetRunObjectDataSize(fprh rhPtr, LPEDATA edPtr)
{
	return(sizeof(RUNDATA));
}


// ---------------
// CreateRunObject
// ---------------
// The routine where the object is actually created
// 
//short WINAPI DLLExport DisplayRunObject(LPRDATA rdPtr)
struct VseHotyatOtMenyaSeks
{
	void* a;
	void* b;
	void* c;

};
typedef bool(__thiscall* beginScene_t)(int*,bool,bool,int,const VseHotyatOtMenyaSeks&,int*);
beginScene_t beginScene_o;
typedef bool(__thiscall* endScene_t)(int*);
endScene_t endScene_o;

void gayBeginScene()
{
	int ptr = (int)lastRdata;
	DWORD opengl_render_this = *(DWORD*)(ptr + 0x280);

	DWORD dwPtr = opengl_render_this + 12;
	DWORD sceneManager = opengl_render_this + 16;

	__asm {
		mov ecx, [dwPtr]
		mov ecx, [ecx]
		push 0 // this argument (блять ебал нахуй этот вывов)
		push offset shit
		push 0
		push 0x3F800000 // 1.0f 
		push 0xFFFF00FF
		push 3
		mov eax, [ecx]
		mov eax, [eax + 0x00] // где 0x00 - оффсет функции в таблице
		call eax
	}
}
void gayEndScene()
{
	int ptr = (int)lastRdata;
	DWORD opengl_render_this = *(DWORD*)(ptr + 0x280);

	DWORD dwPtr = opengl_render_this + 12;
	DWORD sceneManager = opengl_render_this + 16;
	__asm {
		mov ecx, [dwPtr]
		mov ecx, [ecx]
		mov eax, [ecx]
		mov eax, [eax + 0x04]
		call eax
	}

}
void gayDrawAll()
{
	int ptr = (int)lastRdata;
	DWORD opengl_render_this = *(DWORD*)(ptr + 0x280);

	DWORD dwPtr = opengl_render_this + 12;
	DWORD sceneManager = opengl_render_this + 16;
	int retard = (int)GetModuleHandleA("FireflyEN.mfx") + 0x111730;
	__asm {
		mov ecx, [sceneManager]
		mov ecx, [ecx]
		mov eax, [ecx]
		mov eax, retard//[eax + 0x04]
		call eax
	}
	
}

typedef void(__fastcall* drawVertexPrimitiveList_t)(int* retard,
	int* self,
	const void* pointer,
	int a3,
	const void* a4,
	int primitiveCount,
	int vType,
	int pType,
	int iType);
drawVertexPrimitiveList_t drawVertexPrimitiveList_o;
void __fastcall drawVertexPrimitiveList_h(int* retard,
	int* self,
	const void* pointer,
	int a3,
	const void* a4,
	int primitiveCount,
	int vType,
	int pType,
	int iType)
{
	// unused
	return drawVertexPrimitiveList_o(retard, self, pointer, a3, a4, primitiveCount, vType, pType, iType);
}
void transformTarget(float* targetX, float* targetY, float* targetZ, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
	const float DEGTORAD = 3.14159265358979323846264338 / 180;
	const float cr = cos(DEGTORAD * rotX);
	const float sr = sin(DEGTORAD * rotX);
	const float cp = cos(DEGTORAD * rotY);
	const float sp = sin(DEGTORAD * rotY);
	const float cy = cos(DEGTORAD * rotZ);
	const float sy = sin(DEGTORAD * rotZ);

	const float srsp = sr * sp;
	const float crsp = cr * sp;

	const float pseudoMatrix[] = {
		(cp * cy), (cp * sy), (-sp),
		(srsp * cy - cr * sy), (srsp * sy + cr * cy), (sr * cp),
		(crsp * cy + sr * sy), (crsp * sy - sr * cy), (cr * cp) };

	*targetX = posX + (0 * pseudoMatrix[0] + 0 * pseudoMatrix[3] + 1 * pseudoMatrix[6]);
	*targetY = posY + (0 * pseudoMatrix[1] + 0 * pseudoMatrix[4] + 1 * pseudoMatrix[7]);
	*targetZ = posZ + (0 * pseudoMatrix[2] + 0 * pseudoMatrix[5] + 1 * pseudoMatrix[8]);


}
typedef void(__fastcall* cameraRender_t)(int* self);
cameraRender_t cameraRender_o;
void __fastcall cameraRender_h(int* self)
{
	auto& pose = currentLayerInfo.pose;

	
	float* xPos = (float*)(self + 22);
	float* yPos = (float*)(self + 23);
	float* zPos = (float*)(self + 24);
	float* xRot = (float*)(self + 45);
	float* yRot = (float*)(self + 46);
	float* zRot = (float*)(self + 47);
	float* xTarget = (float*)(self + 74);
	float* yTarget = (float*)(self + 75);
	float* zTarget = (float*)(self + 76);
	const float RADTODEG = 180/3.14159265358979323846264338;
	const float DEGTORAD = 3.14159265358979323846264338 / 180;

	XrQuaternionf orientation = XrQuaternionf{ pose.orientation.w,-pose.orientation.z,pose.orientation.y,pose.orientation.x };
	XrVector3f position = XrVector3f{ -pose.position.x,pose.position.y,-pose.position.z }; //TODO might need to do -y here

	//*xRot = orientation.x* RADTODEG;
	//*yRot = orientation.y* RADTODEG;
	//*zRot = orientation.z* RADTODEG;

	//transformTarget(xTarget, yTarget, zTarget, *xPos, *yPos, *zPos,*xRot,*yRot,*zRot);

	cameraRender_o(self);

	DWORD opengl_render_this = *(DWORD*)((int)lastRdata + 0x280);
	DWORD driver = opengl_render_this + 12;
	XrMatrix4x4f toProj;
	XrMatrix4x4f temp;
	XrMatrix4x4f proj;
	XrMatrix4x4f_CreateProjectionFov(&toProj, GRAPHICS_OPENGL, currentLayerInfo.fov, 0.05f, 100.0f);
	//XrMatrix4x4f_InvertRigidBody(&proj, &toProj);
	XrMatrix4x4f_CreateScale(&temp, -1, -1, -1);
	XrMatrix4x4f_Multiply(&proj, &toProj, &temp);
	XrVector3f scale{ 1.0f, 1.0f, 1.0f };
	printf("camera: %X\n", self);
	DWORD sceneManager = opengl_render_this + 16;
	float* camera = (*(float***)sceneManager)[145];

	XrMatrix4x4f toView;
	XrMatrix4x4f toViewTranslated;
	XrMatrix4x4f toViewRotated;

	XrMatrix4x4f view;
	XrMatrix4x4f_CreateTranslationRotationScale(&toView, &position, &orientation, &scale);
	XrMatrix4x4f translation;
	XrMatrix4x4f_CreateTranslation(&translation, -*xPos, -*yPos, -*zPos);


	XrMatrix4x4f_Multiply(&toViewTranslated, &toView, &translation);

	XrMatrix4x4f rotation;
	XrMatrix4x4f_CreateRotation(&rotation, -*xRot,-*yRot,-*zRot);
	XrMatrix4x4f_Multiply(&view, &toViewTranslated, &rotation);

	//XrMatrix4x4f_InvertRigidBody(&view, &toView);
	setTransform_o(*(int**)driver, 2, (int*)&proj);
	setTransform_o(*(int**)driver, 0, (int*)&view);
	//return 
}
int lastRdata=0;
setTransform_t setTransform_o;
short __stdcall CFile_Open_h(LPRDATA rdPtr)
{
	lastRdata = (int)rdPtr;
	bool exitRenderLoop = false;
	bool requestRestart = false;
	
	OpenXRPollEvents(&exitRenderLoop, &requestRestart);
	//if (exitRenderLoop) {
	//	break;
	//}

	if (true) {//g_sessionRunning) {
		OpenXRPollActions();
		OpenXRRenderFrame();
	}
	else {
		// Throttle loop since xrWaitFrame won't be called.
		//std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
	
	return 0;//CFile_Open_o(lastRdata);
}

short WINAPI DLLExport CreateRunObject(LPRDATA rdPtr, LPEDATA edPtr, fpcob cobPtr)
{

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	CFile_Open_o = (CFile_Open_t)((int)GetModuleHandleA("FireflyEN.mfx") + 0x77890);
	DetourAttach((PVOID*)&CFile_Open_o, CFile_Open_h);
	DetourTransactionCommit();


	beginScene_o = (beginScene_t)((int)GetModuleHandleA("FireflyEN.mfx") + 0xAD140);
	endScene_o = (endScene_t)((int)GetModuleHandleA("FireflyEN.mfx") + 0xB0E30);
	setTransform_o = (setTransform_t)((int)GetModuleHandleA("FireflyEN.mfx") + 0xB5F90);


	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	drawVertexPrimitiveList_o = (drawVertexPrimitiveList_t)((int)GetModuleHandleA("FireflyEN.mfx") + 0xb0980);
	DetourAttach((PVOID*)&drawVertexPrimitiveList_o, drawVertexPrimitiveList_h);
	DetourTransactionCommit();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	cameraRender_o = (cameraRender_t)((int)GetModuleHandleA("FireflyEN.mfx") + 0x1E1230);
	DetourAttach((PVOID*)&cameraRender_o, cameraRender_h);
	DetourTransactionCommit();


	printf("XR init\n");
	OpenXRCreateInstance();
	printf("Instance created\n");
	OpenXRInitializeSystem();
	printf("System initialized\n");
	OpenXRInitializeSession();
	printf("Session Initialize\n");
	OpenXRCreateSwapchains();
	printf("Swapchains created\n");
/*
   This routine runs when your object is created, as you might have guessed.
   It is here that you must transfer any data you need in rdPtr from edPtr,
   because after this has finished you cannot access it again!
   Also, if you have anything to initialise (e.g. dynamic arrays, surface objects)
   you should do it here, and free your resources in DestroyRunObject.
*/
	// No errors
	return 0;
}


// ----------------
// DestroyRunObject
// ----------------
// Destroys the run-time object
// 
short WINAPI DLLExport DestroyRunObject(LPRDATA rdPtr, long fast)
{
	printf("XR stop\n");
	OpenXRTearDown();

/*
   When your object is destroyed (either with a Destroy action or at the end of
   the frame) this routine is called. You must free any resources you have allocated!
*/
	// No errors
	return 0;
}


// ----------------
// HandleRunObject
// ----------------
// Called (if you want) each loop, this routine makes the object live
// 
short WINAPI DLLExport HandleRunObject(LPRDATA rdPtr)
{
    
    return 0;//CFile_Open_o(rdPtr);
	return REFLAG_DISPLAY;
/*
   If your extension will draw to the MMF window you should first 
   check if anything about its display has changed :

       if (rdPtr->roc.rcChanged)
          return REFLAG_DISPLAY;
       else
          return 0;

   You will also need to make sure you change this flag yourself 
   to 1 whenever you want to redraw your object
 
   If your extension won't draw to the window, but it still needs 
   to do something every MMF loop use :

        return 0;

   If you don't need to do something every loop, use :

        return REFLAG_ONESHOT;

   This doesn't mean this function can never run again. If you want MMF
   to handle your object again (causing this code to run) use this function:

        callRun-timeFunction(rdPtr, RFUNCTION_REHANDLE, 0, 0);

   At the end of the loop this code will run
*/
	// Will not be called next loop	
	//return REFLAG_ONESHOT;
}

// ----------------
// DisplayRunObject
// ----------------
// Draw the object in the application screen.
// 
short WINAPI DLLExport DisplayRunObject(LPRDATA rdPtr)
{
	
/*
   If you return REFLAG_DISPLAY in HandleRunObject this routine will run.
*/
	// Ok
	return 0;
}

// -------------------
// GetRunObjectSurface
// -------------------
// Implement this function instead of DisplayRunObject if your extension
// supports ink effects and transitions. Note: you can support ink effects
// in DisplayRunObject too, but this is automatically done if you implement
// GetRunObjectSurface (MMF applies the ink effect to the surface).
//
// Note: do not forget to enable the function in the .def file 
// if you remove the comments below.
/*
cSurface* WINAPI DLLExport GetRunObjectSurface(LPRDATA rdPtr)
{
	return NULL;
}
*/

// -------------------------
// GetRunObjectCollisionMask
// -------------------------
// Implement this function if your extension supports fine collision mode (OEPREFS_FINECOLLISIONS),
// Or if it's a background object and you want Obstacle properties for this object.
//
// Should return NULL if the object is not transparent.
//
// Note: do not forget to enable the function in the .def file 
// if you remove the comments below.
//
/*
LPSMASK WINAPI DLLExport GetRunObjectCollisionMask(LPRDATA rdPtr, LPARAM lParam)
{
	// Typical example for active objects
	// ----------------------------------
	// Opaque? collide with box
	if ( (rdPtr->rs.rsEffect & EFFECTFLAG_TRANSPARENT) == 0 )	// Note: only if your object has the OEPREFS_INKEFFECTS option
		return NULL;

	// Transparent? Create mask
	LPSMASK pMask = rdPtr->m_pColMask;
	if ( pMask == NULL )
	{
		if ( rdPtr->m_pSurface != NULL )
		{
			DWORD dwMaskSize = rdPtr->m_pSurface->CreateMask(NULL, lParam);
			if ( dwMaskSize != 0 )
			{
				pMask = (LPSMASK)calloc(dwMaskSize, 1);
				if ( pMask != NULL )
				{
					rdPtr->m_pSurface->CreateMask(pMask, lParam);
					rdPtr->m_pColMask = pMask;
				}
			}
		}
	}

	// Note: for active objects, lParam is always the same.
	// For background objects (OEFLAG_BACKGROUND), lParam maybe be different if the user uses your object
	// as obstacle and as platform. In this case, you should store 2 collision masks
	// in your data: one if lParam is 0 and another one if lParam is different from 0.

	return pMask;
}
*/

// ----------------
// PauseRunObject
// ----------------
// Enters the pause mode
// 
short WINAPI DLLExport PauseRunObject(LPRDATA rdPtr)
{
	// Ok
	return 0;
}


// -----------------
// ContinueRunObject
// -----------------
// Quits the pause mode
//
short WINAPI DLLExport ContinueRunObject(LPRDATA rdPtr)
{
	// Ok
	return 0;
}

// -----------------
// SaveRunObject
// -----------------
// Saves the object to disk
// 

BOOL WINAPI SaveRunObject(LPRDATA rdPtr, HANDLE hf)
{            
	BOOL bOK = FALSE;

#ifndef VITALIZE

	// Save the object's data here

	// OK
	bOK = TRUE;

#endif // VITALIZE

	return bOK;
}

// -----------------
// LoadRunObject
// -----------------
// Loads the object from disk
// 
BOOL WINAPI LoadRunObject(LPRDATA rdPtr, HANDLE hf)
{            
	BOOL bOK=FALSE;

	// Load the object's data here

	// OK
	bOK = TRUE;

	return bOK; 
}




// ============================================================================
//
// START APP / END APP / START FRAME / END FRAME routines
// 
// ============================================================================

// -------------------
// StartApp
// -------------------
// Called when the application starts or restarts.
// Useful for storing global data
// 
void WINAPI DLLExport StartApp(mv _far *mV, CRunApp* pApp)
{
	// Example
	// -------
	// Delete global data (if restarts application)
//	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
//	if ( pData != NULL )
//	{
//		delete pData;
//		mV->mvSetExtUserData(pApp, hInstLib, NULL);
//	}
}

// -------------------
// EndApp
// -------------------
// Called when the application ends.
// 
void WINAPI DLLExport EndApp(mv _far *mV, CRunApp* pApp)
{
	// Example
	// -------
	// Delete global data
//	CMyData* pData = (CMyData*)mV->mvGetExtUserData(pApp, hInstLib);
//	if ( pData != NULL )
//	{
//		delete pData;
//		mV->mvSetExtUserData(pApp, hInstLib, NULL);
//	}
}

// -------------------
// StartFrame
// -------------------
// Called when the frame starts or restarts.
// 
void WINAPI DLLExport StartFrame(mv _far *mV, DWORD dwReserved, int nFrameIndex)
{
}

// -------------------
// EndFrame
// -------------------
// Called when the frame ends.
// 
void WINAPI DLLExport EndFrame(mv _far *mV, DWORD dwReserved, int nFrameIndex)
{
}

// ============================================================================
//
// TEXT ROUTINES (if OEFLAG_TEXT)
// 
// ============================================================================

// -------------------
// GetRunObjectFont
// -------------------
// Return the font used by the object.
// 
/*

  // Note: do not forget to enable the functions in the .def file 
  // if you remove the comments below.

void WINAPI GetRunObjectFont(LPRDATA rdPtr, LOGFONT* pLf)
{
	// Example
	// -------
	// GetObject(rdPtr->m_hFont, sizeof(LOGFONT), pLf);
}

// -------------------
// SetRunObjectFont
// -------------------
// Change the font used by the object.
// 
void WINAPI SetRunObjectFont(LPRDATA rdPtr, LOGFONT* pLf, RECT* pRc)
{
	// Example
	// -------
//	HFONT hFont = CreateFontIndirect(pLf);
//	if ( hFont != NULL )
//	{
//		if (rdPtr->m_hFont!=0)
//			DeleteObject(rdPtr->m_hFont);
//		rdPtr->m_hFont = hFont;
//		SendMessage(rdPtr->m_hWnd, WM_SETFONT, (WPARAM)rdPtr->m_hFont, FALSE);
//	}

}

// ---------------------
// GetRunObjectTextColor
// ---------------------
// Return the text color of the object.
// 
COLORREF WINAPI GetRunObjectTextColor(LPRDATA rdPtr)
{
	// Example
	// -------
	return 0;	// rdPtr->m_dwColor;
}

// ---------------------
// SetRunObjectTextColor
// ---------------------
// Change the text color of the object.
// 
void WINAPI SetRunObjectTextColor(LPRDATA rdPtr, COLORREF rgb)
{
	// Example
	// -------
	rdPtr->m_dwColor = rgb;
	InvalidateRect(rdPtr->m_hWnd, NULL, TRUE);
}
*/


// ============================================================================
//
// WINDOWPROC (interception of messages sent to hMainWin and hEditWin)
//
// Do not forget to enable the WindowProc function in the .def file if you implement it
// 
// ============================================================================
/*
// Get the pointer to the object's data from its window handle
// Note: the object's window must have been subclassed with a
// callRunTimeFunction(rdPtr, RFUNCTION_SUBCLASSWINDOW, 0, 0);
// See the documentation and the Simple Control example for more info.
//
LPRDATA GetRdPtr(HWND hwnd, LPRH rhPtr)
{
	return (LPRDATA)GetProp(hwnd, (LPCSTR)rhPtr->rh4.rh4AtomRd);
}

// Called from the window proc of hMainWin and hEditWin.
// You can intercept the messages and/or tell the main proc to ignore them.
//
LRESULT CALLBACK DLLExport WindowProc(LPRH rhPtr, HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	LPRDATA rdPtr = NULL;

	switch (nMsg) {

	// Example
	case WM_CTLCOLORSTATIC:
		{
			// Get the handle of the control
			HWND hWndControl = (HWND)lParam;

			// Get a pointer to the RUNDATA structure (see GetRdptr function above for more info)
			rdPtr = GetRdPtr(hWndControl, rhPtr);

			// Check if the rdPtr pointer is valid and points to an object created with this extension
			if ( rdPtr == NULL || rdPtr->rHo.hoIdentifier != IDENTIFIER )
				break;

			// OK, intercept the message
			HDC hDC = (HDC)wParam;
			SetBkColor(hDC, rdPtr->backColor);
			SetTextColor(hDC, rdPtr->fontColor);
			rhPtr->rh4.rh4KpxReturn = (long)rdPtr->hBackBrush;
			return REFLAG_MSGRETURNVALUE;
		}
		break;
	}

	return 0;
}
*/

// ============================================================================
//
// DEBUGGER ROUTINES
// 
// ============================================================================

// -----------------
// GetDebugTree
// -----------------
// This routine returns the address of the debugger tree
//
LPWORD WINAPI DLLExport GetDebugTree(LPRDATA rdPtr)
{
#if !defined(RUN_ONLY)
	return DebugTree;
#else
	return NULL;
#endif // !defined(RUN_ONLY)
}

// -----------------
// GetDebugItem
// -----------------
// This routine returns the text of a given item.
//
void WINAPI DLLExport GetDebugItem(LPSTR pBuffer, LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

	// Example
	// -------
/*
	char temp[DB_BUFFERSIZE];

	switch (id)
	{
	case DB_CURRENTSTRING:
		LoadString(hInstLib, IDS_CURRENTSTRING, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->text);
		break;
	case DB_CURRENTVALUE:
		LoadString(hInstLib, IDS_CURRENTVALUE, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->value);
		break;
	case DB_CURRENTCHECK:
		LoadString(hInstLib, IDS_CURRENTCHECK, temp, DB_BUFFERSIZE);
		if (rdPtr->check)
			wsprintf(pBuffer, temp, "TRUE");
		else
			wsprintf(pBuffer, temp, "FALSE");
		break;
	case DB_CURRENTCOMBO:
		LoadString(hInstLib, IDS_CURRENTCOMBO, temp, DB_BUFFERSIZE);
		wsprintf(pBuffer, temp, rdPtr->combo);
		break;
	}
*/

#endif // !defined(RUN_ONLY)
}

// -----------------
// EditDebugItem
// -----------------
// This routine allows to edit editable items.
//
void WINAPI DLLExport EditDebugItem(LPRDATA rdPtr, int id)
{
#if !defined(RUN_ONLY)

	// Example
	// -------
/*
	switch (id)
	{
	case DB_CURRENTSTRING:
		{
			EditDebugInfo dbi;
			char buffer[256];

			dbi.pText=buffer;
			dbi.lText=TEXT_MAX;
			dbi.pTitle=NULL;

			strcpy(buffer, rdPtr->text);
			long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITTEXT, 0, (LPARAM)&dbi);
			if (ret)
				strcpy(rdPtr->text, dbi.pText);
		}
		break;
	case DB_CURRENTVALUE:
		{
			EditDebugInfo dbi;

			dbi.value=rdPtr->value;
			dbi.pTitle=NULL;

			long ret=callRunTimeFunction(rdPtr, RFUNCTION_EDITINT, 0, (LPARAM)&dbi);
			if (ret)
				rdPtr->value=dbi.value;
		}
		break;
	}
*/
#endif // !defined(RUN_ONLY)
}


