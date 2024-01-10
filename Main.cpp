// ============================================================================
//
// This file are where the Conditions/Actions/Expressions are defined.
// You can manually enter these, or use CICK (recommended)
// See the Extension FAQ in this SDK for more info and where to download it
//
// ============================================================================

// Common Include
#include	"fcommon.h"
#include "FusionXR.h"

// Quick memo: content of the eventInformations arrays
// ---------------------------------------------------
// Menu ID
// String ID
// Code
// Flags
// Number_of_parameters
// Parameter_type [Number_of_parameters]
// Parameter_TitleString [Number_of_parameters]

// Definitions of parameters for each condition
short conditionsInfos[] =
{
	
ID__ISHEADSETCONNECTED,					M_IsHeadsetConnected    	,CND_IsHeadsetConnected    	,			EVFLAGS_ALWAYS, 0,																
ID__ISHEADSETASLEEP,					M_IsHeadsetAsleep    		,CND_IsHeadsetAsleep    		,		EVFLAGS_ALWAYS, 0,																			
ID_CONTROLS_ONCONTROLLERCONNECTED,		M_OnControllerConnected 	,CND_OnControllerConnected 	,			EVFLAGS_ALWAYS, 0,																			
ID_CONTROLS_ONCONTROLLERDISCONNECTED,	M_OnControllerDisconnected	,CND_OnControllerDisconnected	,		EVFLAGS_ALWAYS, 0,																			
ID_CONTROLS_ONBUTTONPRESSED,			M_OnButtonPressed 			,CND_OnButtonPressed 			,		EVFLAGS_ALWAYS, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_CND_ENTER_CONTROLLER_ID,M_CND_ENTER_BUTTON_ID,
ID_CONTROLS_ONBUTTONRELEASED,			M_OnButtonReleased 			,CND_OnButtonReleased 		,			EVFLAGS_ALWAYS, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_CND_ENTER_CONTROLLER_ID,M_CND_ENTER_BUTTON_ID,
ID_CONTROLS_BUTTONBEINGTOUCHED,			M_ButtonBeingTouched 		,CND_ButtonBeingTouched 		,		EVFLAGS_ALWAYS, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_CND_ENTER_CONTROLLER_ID,M_CND_ENTER_BUTTON_ID,
ID_CONTROLS_BUTTONBEINGHELD,			M_ButtonBeingHeld 			,CND_ButtonBeingHeld 			,		EVFLAGS_ALWAYS, 2,PARAM_EXPRESSION,PARAM_EXPRESSION,M_CND_ENTER_CONTROLLER_ID,M_CND_ENTER_BUTTON_ID,
ID_CONTROLS_ONANYBUTTONPRESSED,			M_OnAnyButtonPressed 		,CND_OnAnyButtonPressed 		,		EVFLAGS_ALWAYS, 1,PARAM_EXPRESSION,M_CND_ENTER_CONTROLLER_ID,
ID_CONTROLS_ONANYBUTTONRELEASED,		M_OnAnyButtonReleased 		,CND_OnAnyButtonReleased 		,		EVFLAGS_ALWAYS, 1,PARAM_EXPRESSION,M_CND_ENTER_CONTROLLER_ID,
ID_CONTROLS_ANYBUTTONBEINGTOUCHED,		M_AnyButtonBeingTouched 	,CND_AnyButtonBeingTouched 	,			EVFLAGS_ALWAYS, 1,PARAM_EXPRESSION,M_CND_ENTER_CONTROLLER_ID,
ID_CONTROLS_ANYBUTTONBEINGHELD,			M_AnyButtonBeingHeld 		,CND_AnyButtonBeingHeld 		,		EVFLAGS_ALWAYS, 0,0,0
 };

// Definitions of parameters for each action
short actionsInfos[]=
		{
		IDMN_ACTION, M_ACTION,	ACT_ACTION,0,0
		};

// Definitions of parameters for each expression
short expressionsInfos[]=
		{

		ID_TRANSFORMWORLD_POSITIONX,			M_HWorldPosX, EXP_HWorldPosX, EXPFLAG_DOUBLE, 0,
		ID_TRANSFORMWORLD_POSITIONY,			M_HWorldPosY, EXP_HWorldPosY, EXPFLAG_DOUBLE, 0,
		ID_TRANSFORMWORLD_POSITIONZ,			M_HWorldPosZ, EXP_HWorldPosZ, EXPFLAG_DOUBLE, 0,

		ID_TRANSFORMWORLD_ROTATIONX,			M_HWorldRotX, EXP_HWorldRotX, EXPFLAG_DOUBLE, 0,
		ID_TRANSFORMWORLD_ROTATIONY,			M_HWorldRotY, EXP_HWorldRotY, EXPFLAG_DOUBLE, 0,
		ID_TRANSFORMWORLD_ROTATIONZ,			M_HWorldRotZ, EXP_HWorldRotZ, EXPFLAG_DOUBLE, 0,

		ID_TRANSFORM_POSITIONX,					M_HLocalPosX, EXP_HLocalPosX, EXPFLAG_DOUBLE, 0,
		ID_TRANSFORM_POSITIONY,					M_HLocalPosY, EXP_HLocalPosY, EXPFLAG_DOUBLE, 0,
		ID_TRANSFORM_POSITIONZ,					M_HLocalPosZ, EXP_HLocalPosZ, EXPFLAG_DOUBLE, 0,
																			  
		ID_TRANSFORM_ROTATIONX,					M_HLocalRotX, EXP_HLocalRotX, EXPFLAG_DOUBLE, 0,
		ID_TRANSFORM_ROTATIONY,					M_HLocalRotY, EXP_HLocalRotY, EXPFLAG_DOUBLE, 0,
		ID_TRANSFORM_ROTATIONZ,					M_HLocalRotZ, EXP_HLocalRotZ, EXPFLAG_DOUBLE, 0,

		ID_CONTROLLERTRANSFORMWORLD_POSITIONX,	M_CWorldPosX, EXP_CWorldPosX, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, M_CND_ENTER_CONTROLLER_ID,
		ID_CONTROLLERTRANSFORMWORLD_POSITIONY,	M_CWorldPosY, EXP_CWorldPosY, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, M_CND_ENTER_CONTROLLER_ID,
		ID_CONTROLLERTRANSFORMWORLD_POSITIONZ,	M_CWorldPosZ, EXP_CWorldPosZ, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, M_CND_ENTER_CONTROLLER_ID, 

		ID_CONTROLLERTRANSFORMWORLD_ROTATIONX,	M_CWorldRotX, EXP_CWorldRotX, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, M_CND_ENTER_CONTROLLER_ID, 
		ID_CONTROLLERTRANSFORMWORLD_ROTATIONY,	M_CWorldRotY, EXP_CWorldRotY, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, M_CND_ENTER_CONTROLLER_ID, 
		ID_CONTROLLERTRANSFORMWORLD_ROTATIONZ,	M_CWorldRotZ, EXP_CWorldRotZ, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG, M_CND_ENTER_CONTROLLER_ID, 

		ID_CONTROLLERTRANSFORM_POSITIONX,		M_CLocalPosX, EXP_CLocalPosX, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG,  M_CND_ENTER_CONTROLLER_ID, 
		ID_CONTROLLERTRANSFORM_POSITIONY,		M_CLocalPosY, EXP_CLocalPosY, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG,  M_CND_ENTER_CONTROLLER_ID, 
		ID_CONTROLLERTRANSFORM_POSITIONZ,		M_CLocalPosZ, EXP_CLocalPosZ, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG,  M_CND_ENTER_CONTROLLER_ID, 

		ID_CONTROLLERTRANSFORM_ROTATIONX,		M_CLocalRotX, EXP_CLocalRotX, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG,  M_CND_ENTER_CONTROLLER_ID, 
		ID_CONTROLLERTRANSFORM_ROTATIONY,		M_CLocalRotY, EXP_CLocalRotY, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG,  M_CND_ENTER_CONTROLLER_ID, 
		ID_CONTROLLERTRANSFORM_ROTATIONZ,		M_CLocalRotZ, EXP_CLocalRotZ, EXPFLAG_DOUBLE, 1, EXPPARAM_LONG,  M_CND_ENTER_CONTROLLER_ID, 

		ID_CONTROLS_GETLASTBUTTON,				M_LastButton, EXP_LastButton, 0, 2, EXPPARAM_LONG, EXPPARAM_LONG, 0, 0,
		ID_CONTROLS_GETAXIS,					M_AxisValue,  EXP_AxisValue,  EXPFLAG_DOUBLE, 2, EXPPARAM_LONG,EXPPARAM_LONG,0, 0,

		



		};



// ============================================================================
//
// CONDITION ROUTINES
// 
// ============================================================================

// -----------------
// Sample Condition
// -----------------
// Returns TRUE when the two values are equal!
// 

long WINAPI DLLExport Condition(LPRDATA rdPtr, long param1, long param2)
{

//  **** Still use this method for 1 or 2 parameters ****	
//	if (param1==param2)	
//		return TRUE;

	long p1 = CNC_GetParameter(rdPtr);
	long p2 = CNC_GetParameter(rdPtr);
	long p3 = CNC_GetParameter(rdPtr);

	if ((p1 + p2)==p3)
		return TRUE;
		 
	return FALSE;
}


// ============================================================================
//
// ACTIONS ROUTINES
// 
// ============================================================================

// -----------------
// Sample Action
// -----------------
// Does nothing!
// 
short WINAPI DLLExport Action(LPRDATA rdPtr, long param1, long param2)
{
	// Actions work just like Conditions

	// Use directly param1 and/or param2 if this action has 1 or 2 parameters

	// Use this if this action has 3 parameters or more
//	long p1 = CNC_GetParameter(rdPtr);
//	long p2 = CNC_GetParameter(rdPtr);
//	long p3 = CNC_GetParameter(rdPtr);
//	etc.

	return 0;
}


// ============================================================================
//
// EXPRESSIONS ROUTINES
// 
// ============================================================================

// -----------------
// Sample expression
// -----------------
// Add three values
// 
//long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
//long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
//long p3 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);


long WINAPI DLLExport CamWorldPosX(LPRDATA rdPtr,long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&fusionXrData.headXPos;
}
long WINAPI DLLExport CamWorldPosY(LPRDATA rdPtr, long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&fusionXrData.headYPos;
}
long WINAPI DLLExport CamWorldPosZ(LPRDATA rdPtr, long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&fusionXrData.headZPos;
}
long WINAPI DLLExport CamWorldRotX(LPRDATA rdPtr, long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&fusionXrData.headXRot;
}
long WINAPI DLLExport CamWorldRotY(LPRDATA rdPtr, long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&fusionXrData.headYRot;
}
long WINAPI DLLExport CamWorldRotZ(LPRDATA rdPtr, long param1)
{
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&fusionXrData.headZRot;
}
long WINAPI DLLExport ControllerWorldPosX(LPRDATA rdPtr, long param1)
{
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	FusionXRHandData* hand = p1 == 0 ? &fusionXrData.leftHand : &fusionXrData.rightHand;
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&hand->xPos;
}
long WINAPI DLLExport ControllerWorldPosY(LPRDATA rdPtr, long param1)
{
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	FusionXRHandData* hand = p1 == 0 ? &fusionXrData.leftHand : &fusionXrData.rightHand;
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&hand->yPos;
}
long WINAPI DLLExport ControllerWorldPosZ(LPRDATA rdPtr, long param1)
{
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	FusionXRHandData* hand = p1 == 0 ? &fusionXrData.leftHand : &fusionXrData.rightHand;
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&hand->zPos;
}
long WINAPI DLLExport ControllerWorldRotX(LPRDATA rdPtr, long param1)
{
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	FusionXRHandData* hand = p1 == 0 ? &fusionXrData.leftHand : &fusionXrData.rightHand;
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&hand->xRot;
}
long WINAPI DLLExport ControllerWorldRotY(LPRDATA rdPtr, long param1)
{
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	FusionXRHandData* hand = p1 == 0 ? &fusionXrData.leftHand : &fusionXrData.rightHand;
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&hand->yRot;
}
long WINAPI DLLExport ControllerWorldRotZ(LPRDATA rdPtr, long param1)
{
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	FusionXRHandData* hand = p1 == 0 ? &fusionXrData.leftHand : &fusionXrData.rightHand;
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	return *(int*)&hand->zRot;
}
long WINAPI DLLExport GetAxisValue(LPRDATA rdPtr, long param1)
{
	long p1 = CNC_GetFirstExpressionParameter(rdPtr, param1, TYPE_INT);
	long p2 = CNC_GetNextExpressionParameter(rdPtr, param1, TYPE_INT);
	FusionXRHandData* hand = p1 == 0 ? &fusionXrData.leftHand : &fusionXrData.rightHand;
	rdPtr->rHo.hoFlags |= HOF_FLOAT;
	float returnValue = 0;
	switch (p2)
	{
	case 0:
		returnValue = hand->yAxis;
		break;
	case 1:
		returnValue = hand->xAxis;
		break;
	case 2:
		returnValue = hand->trigger;
		break;
	case 3:
		returnValue = hand->grip;
		break;
	}
	return *(int*)&returnValue;
}
// ----------------------------------------------------------
// Condition / Action / Expression jump table
// ----------------------------------------------------------
// Contains the address inside the extension of the different
// routines that handle the action, conditions and expressions.
// Located at the end of the source for convinience
// Must finish with a 0
//
long (WINAPI * ConditionJumps[])(LPRDATA rdPtr, long param1, long param2) = 
			{ 
			Condition,
			Condition,
			Condition,
			Condition,
			Condition,
			Condition,
			Condition,
			Condition,
			Condition,
			Condition,
			Condition,
			Condition,
			0
			};
	
short (WINAPI * ActionJumps[])(LPRDATA rdPtr, long param1, long param2) =
			{
			Action,
			0
			};

long (WINAPI * ExpressionJumps[])(LPRDATA rdPtr, long param) = 
			{     
			CamWorldPosX,
			CamWorldPosY,
			CamWorldPosZ,
			CamWorldRotX,
			CamWorldRotY,
			CamWorldRotZ,
			CamWorldPosX,
			CamWorldPosY,
			CamWorldPosZ,
			CamWorldRotX,
			CamWorldRotY,
			CamWorldRotZ,
			ControllerWorldPosX,
			ControllerWorldPosY,
			ControllerWorldPosZ,
			ControllerWorldRotX,
			ControllerWorldRotY,
			ControllerWorldRotZ,
			ControllerWorldPosX,
			ControllerWorldPosY,
			ControllerWorldPosZ,
			ControllerWorldRotX,
			ControllerWorldRotY,
			ControllerWorldRotZ,
			0, //last button
			GetAxisValue, //axis value
			0,
			};