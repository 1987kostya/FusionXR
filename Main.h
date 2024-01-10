// Object identifier "SAM3"
#pragma message ("************************ WARNING ****************************")
#pragma message ("***** Do not forget to change the IDENTIFIER in Main.h! *****")
#pragma message ("*************************************************************")

#define IDENTIFIER	MAKEID(F,X,R,E)		// REQUIRED: you MUST replace the letters in the MAKEID macro by others
										// and then remove the #pragma message above. If you do not do this, MMF2
										// could confuse your object with another in the event editor.

// ------------------------------
// DEFINITION OF CONDITIONS CODES
// ------------------------------
#define CND_IsHeadsetConnected			0
#define CND_IsHeadsetAsleep    			1
#define CND_OnControllerConnected		2
#define CND_OnControllerDisconnected	3
#define CND_OnButtonPressed 			4
#define CND_OnButtonReleased 			5
#define CND_ButtonBeingTouched 			6
#define CND_ButtonBeingHeld 			7
#define CND_OnAnyButtonPressed 			8
#define CND_OnAnyButtonReleased 		9
#define CND_AnyButtonBeingTouched		10
#define CND_AnyButtonBeingHeld 			11
#define	CND_LAST						12

// ---------------------------
// DEFINITION OF ACTIONS CODES
// ---------------------------
#define	ACT_ACTION					0
#define	ACT_LAST					1

// -------------------------------
// DEFINITION OF EXPRESSIONS CODES
// -------------------------------

#define EXP_HWorldPosX				0
#define EXP_HWorldPosY				1
#define EXP_HWorldPosZ				2
#define EXP_HWorldRotX				3
#define EXP_HWorldRotY				4
#define EXP_HWorldRotZ				5
#define EXP_HLocalPosX				6
#define EXP_HLocalPosY				7
#define EXP_HLocalPosZ				8
#define EXP_HLocalRotX				9
#define EXP_HLocalRotY				10
#define EXP_HLocalRotZ				11
#define EXP_CWorldPosX				12
#define EXP_CWorldPosY				13
#define EXP_CWorldPosZ				14
#define EXP_CWorldRotX				15
#define EXP_CWorldRotY				16
#define EXP_CWorldRotZ				17
#define EXP_CLocalPosX				18
#define EXP_CLocalPosY				19
#define EXP_CLocalPosZ				20
#define EXP_CLocalRotX				21
#define EXP_CLocalRotY				22
#define EXP_CLocalRotZ				23
#define EXP_LastButton				24
#define EXP_AxisValue				25
#define EXP_LAST					26

// ---------------------
// OBJECT DATA STRUCTURE 
// ---------------------
// Used at edit time and saved in the MFA/CCN/EXE files

typedef struct tagEDATA_V1
{
	// Header - required
	extHeader		eHeader;

	// Object's data
//	short			swidth;
//	short			sheight;

} EDITDATA;
typedef EDITDATA *			LPEDATA;

// Object versions
#define	KCX_CURRENT_VERSION			1

// --------------------------------
// RUNNING OBJECT DATA STRUCTURE
// --------------------------------
// Used at runtime. Initialize it in the CreateRunObject function.
// Free any allocated memory or object in the DestroyRunObject function.
//
// Note: if you store C++ objects in this structure and do not store 
// them as pointers, you must call yourself their constructor in the
// CreateRunObject function and their destructor in the DestroyRunObject
// function. As the RUNDATA structure is a simple C structure and not a C++ object.

typedef struct tagRDATA
{
	// Main header - required
	headerObject	rHo;					// Header

	// Optional headers - depend on the OEFLAGS value, see documentation and examples for more info
//	rCom			rc;				// Common structure for movements & animations
//	rMvt			rm;				// Movements
//	rSpr			rs;				// Sprite (displayable objects)
//	rVal			rv;				// Alterable values

	// Object's runtime data

} RUNDATA;
typedef	RUNDATA	*			LPRDATA;

// Size when editing the object under level editor
// -----------------------------------------------
#define	MAX_EDITSIZE			sizeof(EDITDATA)

// Default flags - see documentation for more info
// -------------
#define	OEFLAGS      			0
#define	OEPREFS      			0


// If to handle message, specify the priority of the handling procedure
// 0= low, 255= very high. You should use 100 as normal.                                                
// --------------------------------------------------------------------
#define	WINDOWPROC_PRIORITY		100
