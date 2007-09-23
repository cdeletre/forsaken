

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
		Include File...	
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
#include <stdio.h>
#include "typedefs.h"
#include <dplay.h>
#include "new3d.h"
#include "quat.h"
#include "CompObjects.h"
#include "bgobjects.h"
#include "Object.h"
#include "mydplay.h"

#include "d3ddemo.h"
#include "mload.h"

#include "camera.h"
#include "magic.h"
#include "XMem.h"


#define	CAM_VERSION_NUMBER 1

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
		Externals ...
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
		Globals ...
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/

CAMERA	CurrentCamera;

CAMERA	MainCamera;			// the main viewing screen...



int32	NumOfCams = 0;
REMOTECAMERA * ActiveRemoteCamera = NULL;
REMOTECAMERA * RemoteCameras = NULL;


/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
	Procedure	:		Load .Cam File
	Input		:		char	*	Filename 
	Output		:		Nothing
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
BOOL Cameraload( char * Filename )
{
	char		*	Buffer;
	char		*	OrgBuffer;
	int32		*	int32Pnt;
	uint32		*	uint32Pnt;
	int16		*	int16Pnt;
	float		*	FloatPnt;
	int			e;
	long			File_Size;
	long			Read_Size;
	REMOTECAMERA	*	CamPnt;
	uint32		MagicNumber;
	uint32		VersionNumber;
	VECTOR	Tempv;

	ActiveRemoteCamera = NULL;
	RemoteCameras = NULL;


	File_Size = Get_File_Size( Filename );	

	if( !File_Size )
	{
		// Doesnt Matter.....
		return TRUE;
	}
	Buffer = calloc( 1, File_Size );
	OrgBuffer = Buffer;
	if( !Buffer )
	{
		Msg( "Camload failed to Allocate buffer for %s failed\n", Filename );
		return FALSE;
	}
	Read_Size = Read_File( Filename, Buffer, File_Size );
	if( Read_Size != File_Size )
	{
		Msg( "Camload Error reading file %s\n", Filename );
		return FALSE;
	}
	uint32Pnt = (uint32 *) Buffer;
	MagicNumber = *uint32Pnt++;
	VersionNumber = *uint32Pnt++;
	Buffer = (char *) uint32Pnt;

	if( ( MagicNumber != MAGIC_NUMBER ) || ( VersionNumber != CAM_VERSION_NUMBER  ) )
	{
		Msg( "CamLoad() Incompatible .CAM file %s", Filename );
		return( FALSE );
	}

	int32Pnt = (int32*) Buffer;
	NumOfCams = *int32Pnt++;
 	RemoteCameras = (REMOTECAMERA*) calloc( NumOfCams, sizeof(REMOTECAMERA) );
 	CamPnt = RemoteCameras;
	if( !CamPnt )
	{
		Msg( "Camload failed allocate Cam Pointer in %s \n", Filename );
		return FALSE;
	}
	Buffer = (char*) int32Pnt;
	
	for( e = 0 ; e < NumOfCams ; e++ )
	{
		CamPnt->enable = FALSE;

		int16Pnt = (int16*) Buffer;
		CamPnt->Group = *int16Pnt++;
		FloatPnt = (float*) int16Pnt;
		
		CamPnt->Pos.x = *FloatPnt++;
		CamPnt->Pos.y = *FloatPnt++;
		CamPnt->Pos.z = *FloatPnt++;
		CamPnt->Dir.x = *FloatPnt++;
		CamPnt->Dir.y = *FloatPnt++;
		CamPnt->Dir.z = *FloatPnt++;
		CamPnt->Up.x = *FloatPnt++;
		CamPnt->Up.y = *FloatPnt++;
		CamPnt->Up.z = *FloatPnt++;
		Buffer = (char*) FloatPnt;
		Tempv.x = CamPnt->Pos.x + CamPnt->Dir.x;
		Tempv.y = CamPnt->Pos.y + CamPnt->Dir.y;
		Tempv.z = CamPnt->Pos.z + CamPnt->Dir.z;
		MakeViewMatrix( &CamPnt->Pos, &Tempv, &CamPnt->Up, &CamPnt->Mat);
		MatrixTranspose( &CamPnt->Mat, &CamPnt->InvMat );
		CamPnt++;
	}
	// All Cameras Networks have been loaded...
	free(OrgBuffer);
	return TRUE;
}

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
	Procedure	:		Release NodeHeader..
	Input		:		NOTHING
	Output		:		Nothing
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void CameraRelease( void)
{
	if( RemoteCameras )
	{
		free( RemoteCameras );
		ActiveRemoteCamera = NULL;
		RemoteCameras = NULL;
	}
	NumOfCams = 0;
}

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
 	Procedure	:		Enable Remote Camera
	Input		:		uint16	* Data
	Output		:		Nothing
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void EnableRemoteCamera( uint16 * Data )
{
	REMOTECAMERA * CamPnt;

	CamPnt = RemoteCameras;
	if( !CamPnt )
		return;
	CamPnt += *Data;
	CamPnt->enable = TRUE;
	ActiveRemoteCamera = CamPnt;
}
/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
 	Procedure	:		Disable Remote Camera
	Input		:		uint16	* Data
	Output		:		Nothing
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
void DisableRemoteCamera( uint16 * Data )
{
	REMOTECAMERA * CamPnt;

	CamPnt = RemoteCameras;
	if( !CamPnt )
		return;
	CamPnt += *Data;
	CamPnt->enable = FALSE;
	if( ActiveRemoteCamera == CamPnt )
		ActiveRemoteCamera = NULL;
}

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
	Procedure	:	Save RemoteCameras arrays & Connected Global Variables
	Input		:	FILE	*	File Pointer
	Output		:	FILE	*	Updated File Pointer
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
FILE * SaveRemoteCameras( FILE * fp )
{
	uint16	TempIndex;

	if( fp )
	{
		if( ActiveRemoteCamera ) TempIndex = (uint16) ( ActiveRemoteCamera - RemoteCameras );
		else TempIndex = (uint16) -1;
		fwrite( &TempIndex, sizeof( uint16 ), 1, fp );
	}

	return( fp );
}

/*ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
	Procedure	:	Load RemoteCameras Arrays & Connected Global Variables
	Input		:	FILE	*	File Pointer
	Output		:	FILE	*	Updated File Pointer
ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ*/
FILE * LoadRemoteCameras( FILE * fp )
{
	uint16	i;
	uint16	TempIndex;
	REMOTECAMERA	*	CamPnt;

 	CamPnt = RemoteCameras;

	for( i = 0; i < NumOfCams ; i++ )
	{
		CamPnt->enable = FALSE;
		CamPnt++;
	}

	if( fp )
	{
		fread( &TempIndex, sizeof( uint16 ), 1, fp );
		if( TempIndex != (uint16) -1 )
		{
			ActiveRemoteCamera = ( RemoteCameras + TempIndex );
			ActiveRemoteCamera->enable = TRUE;
		}
		else
		{
			ActiveRemoteCamera = NULL;
		}
	}

	return( fp );
}


