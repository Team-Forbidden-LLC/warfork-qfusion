/*
Copyright (C) 2016 Victor Luchits

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "NRIDescs.h"
#include "r_image.h"
#include "r_local.h"
#include "r_cmdque.h"
#include "r_nri.h"
#include "r_resource_upload.h"
#include "r_frontend.h"

#include "stb_ds.h"


static bool enableDebugValidation = true;

static ref_frontend_t rrf;
static ref_cmdbuf_t *RF_GetNextAdapterFrame( ref_frontendAdapter_t *adapter );

static void __R_InitVolatileAssets( void )
{
	// init volatile data
	R_InitSkeletalCache();
	R_InitCoronas();
	R_InitCustomColors();

	rsh.envShader = R_LoadShader( "$environment", SHADER_TYPE_OPAQUE_ENV, true );
	rsh.skyShader = R_LoadShader( "$skybox", SHADER_TYPE_SKYBOX, true );
	rsh.whiteShader = R_LoadShader( "$whiteimage", SHADER_TYPE_2D, true );
	rsh.emptyFogShader = R_LoadShader( "$emptyfog", SHADER_TYPE_FOG, true );

	if( !rsh.nullVBO ) {
		rsh.nullVBO = R_InitNullModelVBO();
	}
	else {
		R_TouchMeshVBO( rsh.nullVBO );
	}

	if( !rsh.postProcessingVBO ) {
		rsh.postProcessingVBO = R_InitPostProcessingVBO();
	}
	else {
		R_TouchMeshVBO( rsh.postProcessingVBO );
	}
}

static void RF_AdapterFrame( ref_frontendAdapter_t *adapter )
{
	static unsigned lastTime = 0;
	static int bias = 0;
	unsigned time = ri.Sys_Milliseconds();
	unsigned wait, frameTime;
	unsigned minMsec;
	ref_cmdbuf_t *frame;

	if( adapter->maxfps > 0 )
		minMsec = 1000 / adapter->maxfps;
	else
		minMsec = 1;
	frameTime = (int)(time - lastTime);
	
	bias += frameTime - minMsec;
	if( bias > (int)minMsec )
		bias = (int)minMsec;

    // Adjust minMsec if previous frame took too long to render so
    // that framerate is stable at the requested value.
	bias -= minMsec;

	wait = frameTime;
	do {
		if( wait >= minMsec )
			wait = 0;
		else
			wait = minMsec - wait;
		if( wait < 1 )
			ri.Sys_Sleep( 0 );
		else
			ri.Sys_Sleep( wait - 1 );
		wait = ri.Sys_Milliseconds() - lastTime;
	} while( wait < minMsec );
	
	lastTime = ri.Sys_Milliseconds();

	frame = RF_GetNextAdapterFrame( adapter );
	if( frame ) {
		frame->RunCmds( frame );
		adapter->readFrameId = frame->GetFrameId( frame );
	}

	adapter->cmdPipe->RunCmds( adapter->cmdPipe );
}

/*
* RF_AdapterThreadProc
*/
//static void *RF_AdapterThreadProc( void *param )
//{
//	ref_frontendAdapter_t *adapter = param;
//
//	GLimp_MakeCurrent( adapter->GLcontext, GLimp_GetWindowSurface( NULL ) );
//
//	while( !adapter->shutdown ) {
//		RF_AdapterFrame( adapter );
//	}
//
//	GLimp_MakeCurrent( NULL, NULL );
//
//	return NULL;
//}

static void RF_AdapterShutdown( ref_frontendAdapter_t *adapter )
{
	if( !adapter->cmdPipe ) {
		return;
	}

	adapter->cmdPipe->Shutdown( adapter->cmdPipe );
	adapter->cmdPipe->FinishCmds( adapter->cmdPipe );

	if( adapter->thread ) {
		adapter->shutdown = true;
		ri.Thread_Join( adapter->thread );
		ri.Mutex_Destroy( &adapter->frameLock );
	}

	RF_DestroyCmdPipe( &adapter->cmdPipe );

	//if( adapter->GLcontext ) {
	//	GLimp_SharedContext_Destroy( adapter->GLcontext, NULL );
	//}

	GLimp_EnableMultithreadedRendering( false );

	memset( adapter, 0, sizeof( *adapter ) );
}

/*
* RF_AdapterInit
*/
static bool RF_AdapterInit( ref_frontendAdapter_t *adapter )
{
	adapter->maxfps = 0;
	adapter->cmdPipe = RF_CreateCmdPipe( !glConfig.multithreading );

 // if( glConfig.multithreading ) {
 // 	adapter->frameLock = ri.Mutex_Create();

 // 	GLimp_EnableMultithreadedRendering( true );

 // 	if( !GLimp_SharedContext_Create( &adapter->GLcontext, NULL ) ) {
 // 		return false;
 // 	}
 // 	
 // 	adapter->shutdown = false;
 // 	adapter->thread = ri.Thread_Create( RF_AdapterThreadProc, adapter );
 // 	if( !adapter->thread ) {
 // 		GLimp_EnableMultithreadedRendering( false );
 // 		return false;
 // 	}
 // }

	adapter->cmdPipe->Init( adapter->cmdPipe );

	return true;
}

/*
* RF_AdapterWait
*
* Blocks the current thread until adapter is finished processing frame and inter-frame commands.
*/
//static void RF_AdapterWait( ref_frontendAdapter_t *adapter )
//{
//	if( adapter->thread == NULL ) {
//		return;
//	}
//
//	while( adapter->frameId != adapter->readFrameId ) {
//		ri.Sys_Sleep( 0 );
//	}
//	
//	adapter->cmdPipe->FinishCmds( adapter->cmdPipe );
//}

static ref_cmdbuf_t *RF_GetNextAdapterFrame( ref_frontendAdapter_t *adapter )
{
	//ref_cmdbuf_t *result = NULL;
	//ref_frontend_t *fe = adapter->owner;

	//ri.Mutex_Lock( adapter->frameLock );
	//if( adapter->frameNum != fe->lastFrameNum ) {
	//	adapter->frameId = fe->frameId;
	//	adapter->frameNum = fe->lastFrameNum;

	//	result = fe->frames[adapter->frameNum];
	//	result->SetFrameId( result, fe->frameId );
	//}
	//ri.Mutex_Unlock( adapter->frameLock );

	return NULL;
}


//TODO: cleanup leaking logic
rserr_t RF_Init( const char *applicationName, const char *screenshotPrefix, int startupColor,
	int iconResource, const int *iconXPM,
	void *hinstance, void *wndproc, void *parenthWnd, 
	bool verbose )
{
	memset( &rsh, 0, sizeof( rsh ) );
	memset( &rf, 0, sizeof( rf ) );
	memset( &rrf, 0, sizeof( rrf ) );
	
	rsh.registrationSequence = 1;
	rsh.registrationOpen = false;

	rsh.worldModelSequence = 1;

	rf.swapInterval = -1;
	rf.speedsMsgLock = ri.Mutex_Create();
	rf.debugSurfaceLock = ri.Mutex_Create();

	r_mempool = R_AllocPool( NULL, "Rendering Frontend" );
	rserr_t err = R_Init( applicationName, screenshotPrefix, startupColor,
		iconResource, iconXPM, hinstance, wndproc, parenthWnd, verbose );
	
	if( !applicationName ) applicationName = "Qfusion";
	if( !screenshotPrefix ) screenshotPrefix = "";
	
	R_WIN_Init(applicationName, hinstance, wndproc, parenthWnd, iconResource, iconXPM);
	nri_init_desc_t desc = {
		.enableApiValidation = true,
		.enableNriValidation = true,
		.api = NriGraphicsAPI_VULKAN
	};		
	
	if(!R_InitNriBackend(&desc, &rsh.nri)) {
		return rserr_unknown;
	}

	rf.applicationName= R_CopyString( applicationName );
	rf.screenshotPrefix = R_CopyString( screenshotPrefix );
	rf.startupColor = startupColor;

	// create vulkan window
	win_init_t winInit = { 
		.backend = VID_WINDOW_VULKAN, 
		.x = vid_xpos->integer, 
		.y = vid_ypos->integer, 
		.width = vid_xpos->integer, 
		.height = vid_ypos->integer
	};
	if(!R_WIN_InitWindow(&winInit)) {
		ri.Com_Error(ERR_DROP, "failed to create window" );
		return rserr_unknown;
	}

	win_handle_t handle = {};
	if(!R_WIN_GetWindowHandle( &handle ) ) {
		ri.Com_Error(ERR_DROP, "failed to resolve window handle" );
		return rserr_unknown;
	}
	NriWindow nriWindow = {};
	switch( handle.winType ) {
		case VID_WINDOW_WAYLAND:
			nriWindow.wayland.surface = handle.window.wayland.surface;
			nriWindow.wayland.display = handle.window.wayland.display;
			break;
		case VID_WINDOW_TYPE_X11:
			nriWindow.x11.window = handle.window.x11.window;
			nriWindow.x11.dpy = handle.window.x11.dpy;
			break;
		case VID_WINDOW_WIN32:
			nriWindow.windows.hwnd = handle.window.win.hwnd;
			break;
		default:
			assert( false );
			break;
	}

	NriSwapChainDesc swapChainDesc = { 
		.commandQueue = rsh.nri.graphicsCommandQueue,
		.width = winInit.width, 
		.height = winInit.height, 
		.format = DefaultSwapchainFormat,
		.textureNum = 3,
		.window = nriWindow
	};
	NRI_ABORT_ON_FAILURE( rsh.nri.swapChainI.CreateSwapChain( rsh.nri.device, &swapChainDesc, &rsh.swapchain) );
	NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateFence( rsh.nri.device, 0, &rsh.frameFence ) );
	NRI_ABORT_ON_FAILURE( rsh.nri.coreI.GetCommandQueue(rsh.nri.device, NriCommandQueueType_GRAPHICS, &rsh.cmdQueue) )
	NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateFence( rsh.nri.device, 0, &rsh.frameFence) );


	for(size_t i = 0; i < NUMBER_FRAMES_FLIGHT; i++) {
		NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateCommandAllocator( rsh.cmdQueue, &rsh.frames[i].allocator ) );
		NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateCommandBuffer( rsh.frames[i].allocator, &rsh.frames[i].cmd ) );
	}

	{
		uint32_t swapChainTextureNum;
		NriTexture *const *swapChainTextures = rsh.nri.swapChainI.GetSwapChainTextures( rsh.swapchain, &swapChainTextureNum );
		arrsetlen(rsh.backBuffers, swapChainTextureNum);
		for( uint32_t i = 0; i < swapChainTextureNum; i++ ) {
			rsh.backBuffers[i].texture = swapChainTextures[i];
			NriTexture2DViewDesc textureViewDesc = { 
				swapChainTextures[i], 
				NriTexture2DViewType_COLOR_ATTACHMENT, 
				DefaultSwapchainFormat };
			NRI_ABORT_ON_FAILURE( rsh.nri.coreI.CreateTexture2DView( &textureViewDesc, &rsh.backBuffers[i].colorAttachment) );
		}
	}

	if( err != rserr_ok ) {
		return err;
	}
	R_InitResourceUpload(&rsh.nri);

	RP_Init();

	R_InitVBO();

	R_InitImages();

	R_InitShaders();

	R_InitCinematics();

	R_InitSkinFiles();

	R_InitModels();

	R_ClearScene();

	__R_InitVolatileAssets();

	R_ClearRefInstStack();

	R_InitDrawLists();

	R_InitShaders();

	return rserr_ok;
}

rserr_t RF_SetMode( int x, int y, int width, int height, int displayFrequency, bool fullScreen, bool stereo )
{

	if( glConfig.width == width && glConfig.height == height && glConfig.fullScreen != fullScreen ) {
		return GLimp_SetFullscreenMode( displayFrequency, fullScreen );
	}

	rsh.frameIndex = 0;

	rsh.nri.helperI.WaitForIdle( rsh.cmdQueue );
	rsh.nri.swapChainI.DestroySwapChain( rsh.swapchain );


	win_handle_t handle = {};
	if(!R_WIN_GetWindowHandle( &handle ) ) {
		ri.Com_Error(ERR_DROP, "failed to resolve window handle" );
		return rserr_unknown;
	}
	NriWindow nriWindow = {};
	switch( handle.winType ) {
		case VID_WINDOW_WAYLAND:
			nriWindow.wayland.surface = handle.window.wayland.surface;
			nriWindow.wayland.display = handle.window.wayland.display;
			break;
		case VID_WINDOW_TYPE_X11:
			nriWindow.x11.window = handle.window.x11.window;
			nriWindow.x11.dpy = handle.window.x11.dpy;
			break;
		case VID_WINDOW_WIN32:
			nriWindow.windows.hwnd = handle.window.win.hwnd;
			break;
		default:
			assert( false );
			break;
	}


	//NriSwapChainDesc swapChainDesc = { 
	//	.commandQueue = rsh.nri.graphicsCommandQueue,
	//	.width = width, 
	//	.height = height, 
	//	.format = DefaultSwapchainFormat,
	//	.textureNum = 3,
	//	.window =nriWindow 
	//};
	//NRI_ABORT_ON_FAILURE( rsh.nri.swapChainI.CreateSwapChain( rsh.nri.device, &swapChainDesc, &rsh.swapchain) );

	//RF_AdapterShutdown( &rrf.adapter );

	// TODO: need to handle setting x,y and width,height
 // err = R_SetMode( x, y, width, height, displayFrequency, fullScreen, stereo );
 // if( err != rserr_ok ) {
 // 	return err;
 // }

	rrf.frameId = 0;
	rrf.frameNum = rrf.lastFrameNum = 0;

	//if( !rrf.frame ) {
	//	//if( glConfig.multithreading ) {
	//	//	int i;
	//	//	for( i = 0; i < 3; i++ )
	//	//		rrf.frames[i] = RF_CreateCmdBuf( false );
	//	//}
	//	//else {
	//		//rrf.frame = RF_CreateCmdBuf( true );
	//	//}
	//}

	//if( glConfig.multithreading ) {
	//	rrf.frame = rrf.frames[0];
	//}

	//rrf.frame->Clear( rrf.frame );
	memset( rrf.customColors, 0, sizeof( rrf.customColors ) );

	//rrf.adapter.owner = (void *)&rrf;
 // if( RF_AdapterInit( &rrf.adapter ) != true ) {
 // 	return rserr_unknown;
 // }

	return rserr_ok;	
}

rserr_t RF_SetWindow( void *hinstance, void *wndproc, void *parenthWnd )
{
 // rserr_t err;
 // bool surfaceChangePending = false;

 // err = GLimp_SetWindow( hinstance, wndproc, parenthWnd, &surfaceChangePending );

 // if( err == rserr_ok && surfaceChangePending )
 // 	rrf.adapter.cmdPipe->SurfaceChange( rrf.adapter.cmdPipe );

 // return err;
 assert( false );
 return rserr_ok;
}

void RF_AppActivate( bool active, bool destroy )
{
	//R_Flush();
	GLimp_AppActivate( active, destroy );
}

void RF_Shutdown( bool verbose )
{
	RF_AdapterShutdown( &rrf.adapter );

 // if( glConfig.multithreading ) {
 // 	int i;
 // 	for( i = 0; i < 3; i++ )
 // 		RF_DestroyCmdBuf( &rrf.frames[i] );
 // }
 // else {
		//RF_DestroyCmdBuf( &rrf.frame );
//	}
	memset( &rrf, 0, sizeof( rrf ) );

	R_Shutdown( verbose );
}

static void RF_CheckCvars( void )
{
	// disallow bogus r_maxfps values, reset to default value instead
	if( r_maxfps->modified ) {
		if( r_maxfps->integer <= 0 ) {
			ri.Cvar_ForceSet( r_maxfps->name, r_maxfps->dvalue );
		}
		r_maxfps->modified = false;
	}

	// update gamma
 // if( r_gamma->modified )
 // {
 // 	r_gamma->modified = false;
 // 	rrf.adapter.cmdPipe->SetGamma( rrf.adapter.cmdPipe, r_gamma->value );
 // }
 // 
 // if( r_texturefilter->modified )
 // {
 // 	r_texturefilter->modified = false;
 // 	rrf.adapter.cmdPipe->SetTextureFilter( rrf.adapter.cmdPipe, r_texturefilter->integer );
 // }

	if( r_wallcolor->modified || r_floorcolor->modified ) {
		vec3_t wallColor, floorColor;
		
		sscanf( r_wallcolor->string,  "%3f %3f %3f", &wallColor[0], &wallColor[1], &wallColor[2] );
		sscanf( r_floorcolor->string, "%3f %3f %3f", &floorColor[0], &floorColor[1], &floorColor[2] );
		
		r_wallcolor->modified = r_floorcolor->modified = false;
		
		for( size_t i = 0; i < 3; i++ ) {
			rsh.wallColor[i] = bound( 0, floor( wallColor[i] ) / 255.0, 1.0 );
			rsh.floorColor[i] = bound( 0, floor( floorColor[i] ) / 255.0, 1.0 );
		}
		//rrf.adapter.cmdPipe->SetWallFloorColors( rrf.adapter.cmdPipe, wallColor, floorColor );		
	}

	if( gl_drawbuffer->modified )
	{
		gl_drawbuffer->modified = false;
		Q_strncpyz( rf.drawBuffer, gl_drawbuffer->string, sizeof( rf.drawBuffer ) );
		rf.newDrawBuffer = true;
		//rrf.adapter.cmdPipe->SetDrawBuffer( rrf.adapter.cmdPipe, gl_drawbuffer->string );
	}
	
	// texturemode stuff
	if( r_texturemode->modified )
	{
		r_texturemode->modified = false;
		R_TextureMode(r_texturemode->string);
		//rrf.adapter.cmdPipe->SetTextureMode( rrf.adapter.cmdPipe, r_texturemode->string );
	}
	
	// keep r_outlines_cutoff value in sane bounds to prevent wallhacking
	if( r_outlines_scale->modified ) {
		if( r_outlines_scale->value < 0 ) {
			ri.Cvar_ForceSet( r_outlines_scale->name, "0" );
		}
		else if( r_outlines_scale->value > 3 ) {
			ri.Cvar_ForceSet( r_outlines_scale->name, "3" );
		}
		r_outlines_scale->modified = false;
	}
}

void RF_BeginFrame( float cameraSeparation, bool forceClear, bool forceVsync )
{
	RF_CheckCvars();

	// run cinematic passes on shaders
	R_RunAllCinematics();

	rrf.adapter.maxfps = r_maxfps->integer;
	
	const uint32_t bufferedFrameIndex = rsh.frameIndex % NUMBER_FRAMES_FLIGHT;
	if( rsh.frameIndex >= NUMBER_FRAMES_FLIGHT ) {
		rsh.nri.coreI.Wait( rsh.frameFence, 1 + rsh.frameIndex - NUMBER_FRAMES_FLIGHT );
		rsh.nri.coreI.ResetCommandAllocator( rsh.frames[bufferedFrameIndex].allocator);
	}
	rsh.backbufferIndex = rsh.nri.swapChainI.AcquireNextSwapChainTexture( rsh.swapchain);

	// take the frame the backend is not busy processing
 // if( glConfig.multithreading ) {
 // 	ri.Mutex_Lock( rrf.adapter.frameLock );
 // 	if( rrf.lastFrameNum == rrf.adapter.frameNum )
 // 		rrf.frameNum = (rrf.adapter.frameNum + 1) % 3;
 // 	else
 // 		rrf.frameNum = 3 - (rrf.adapter.frameNum + rrf.lastFrameNum);
 // 	if( rrf.frameNum == 3 ) {
 // 		rrf.frameNum = 1;
 // 	}
 // 	rrf.frame = rrf.frames[rrf.frameNum];
 // 	ri.Mutex_Unlock( rrf.adapter.frameLock );
 // }

	//rrf.frame->Clear( rrf.frame );
	rrf.cameraSeparation = cameraSeparation;

	//R_DataSync();



	memset( &rf.stats, 0, sizeof( rf.stats ) );

	// update fps meter
	// copy in changes from R_BeginFrame
	//	rrf.frame->BeginFrame( rrf.frame, cameraSeparation, forceClear, forceVsync );
	const unsigned int time = ri.Sys_Milliseconds();
	rf.fps.count++;
	rf.fps.time = time;
	if( rf.fps.time - rf.fps.oldTime >= 250 ) {
		rf.fps.average = time - rf.fps.oldTime;
		rf.fps.average = 1000.0f * (rf.fps.count - rf.fps.oldCount) / (float)rf.fps.average + 0.5f;
		rf.fps.oldTime = time;
		rf.fps.oldCount = rf.fps.count;
	}

}

void RF_EndFrame( void )
{
	// R_DataSync();

	// rrf.frame->EndFrame( rrf.frame );
	R_ResourceSubmit();

	// if( glConfig.multithreading ) {
	// 	ri.Mutex_Lock( rrf.adapter.frameLock );
	// 	rrf.lastFrameNum = rrf.frameNum;
	// 	rrf.frameId++;
	// 	ri.Mutex_Unlock( rrf.adapter.frameLock );
	// }
	const uint32_t bufferedFrameIndex = rsh.frameIndex % NUMBER_FRAMES_FLIGHT;

	{ // Submit
		const NriCommandBuffer *buffers[] = { rsh.frames[bufferedFrameIndex].cmd };
		NriQueueSubmitDesc queueSubmitDesc = {};
		queueSubmitDesc.commandBuffers = buffers;
		queueSubmitDesc.commandBufferNum = 1;
		rsh.nri.coreI.QueueSubmit(rsh.cmdQueue, &queueSubmitDesc);
	}

	// Present
	rsh.nri.swapChainI.QueuePresent( rsh.swapchain );
	{ // Signaling after "Present" improves D3D11 performance a bit
		NriFenceSubmitDesc signalFence = {};
		signalFence.fence = rsh.frameFence;
		signalFence.value = 1 + rsh.frameIndex;

		NriQueueSubmitDesc queueSubmitDesc = {};
		queueSubmitDesc.signalFences = &signalFence;
		queueSubmitDesc.signalFenceNum = 1;

		//rsh.nri.coreI.QueueSubmit( *m_CommandQueue, queueSubmitDesc );
	}
}

void RF_BeginRegistration( void )
{
	// sync to the backend thread to ensure it's not using old assets for drawing
	//RF_AdapterWait( &rrf.adapter );
	R_BeginRegistration();
	//rrf.adapter.cmdPipe->BeginRegistration( rrf.adapter.cmdPipe );
	//RF_AdapterWait( &rrf.adapter );
}

void RF_EndRegistration( void )
{
	// sync to the backend thread to ensure it's not using old assets for drawing
	//RF_AdapterWait( &rrf.adapter );
	//R_EndRegistration();
	//rrf.adapter.cmdPipe->EndRegistration( rrf.adapter.cmdPipe );
	//RF_AdapterWait( &rrf.adapter );

	//R_EndRegistration();

	// reset the cache of custom colors, otherwise RF_SetCustomColor might fail to do anything
	memset( rrf.customColors, 0, sizeof( rrf.customColors ) );
}

void RF_RegisterWorldModel( const char *model, const dvis_t *pvsData )
{
	//RF_AdapterWait( &rrf.adapter );
	R_RegisterWorldModel( model, pvsData );
}

void RF_ClearScene( void )
{
	//R_ClearScene_2(rrf.scene);	
	//rrf.frame->ClearScene( rrf.frame );
}

void RF_AddEntityToScene( const entity_t *ent )
{
	//rrf.frame->AddEntityToScene( rrf.frame, ent );
}

void RF_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b )
{
	//rrf.frame->AddLightToScene( rrf.frame, org, intensity, r, g, b );
}

void RF_AddPolyToScene( const poly_t *poly )
{
	//rrf.frame->AddPolyToScene( rrf.frame, poly );
}

void RF_AddLightStyleToScene( int style, float r, float g, float b )
{
	//rrf.frame->AddLightStyleToScene( rrf.frame, style, r, g, b );
}

void RF_RenderScene( const refdef_t *fd )
{
	//rrf.frame->RenderScene( rrf.frame, fd );
}

void RF_DrawStretchPic( int x, int y, int w, int h, float s1, float t1, float s2, float t2, 
	const vec4_t color, const shader_t *shader )
{
	//rrf.frame->DrawRotatedStretchPic( rrf.frame, x, y, w, h, s1, t1, s2, t2, 0, color, shader );
}

void RF_DrawRotatedStretchPic( int x, int y, int w, int h, float s1, float t1, float s2, float t2, float angle, 
	const vec4_t color, const shader_t *shader )
{
	//rrf.frame->DrawRotatedStretchPic( rrf.frame, x, y, w, h, s1, t1, s2, t2, angle, color, shader );
}

void RF_DrawStretchRaw( int x, int y, int w, int h, int cols, int rows, 
	float s1, float t1, float s2, float t2, uint8_t *data )
{
	//if( !cols || !rows )
	//	return;

	//if( data )
	//	R_UploadRawPic( rsh.rawTexture, cols, rows, data );

	//rrf.frame->DrawStretchRaw( rrf.frame, x, y, w, h, s1, t1, s2, t2 );
}

void RF_DrawStretchRawYUV( int x, int y, int w, int h, 
	float s1, float t1, float s2, float t2, ref_img_plane_t *yuv )
{
	//if( yuv )
	//	R_UploadRawYUVPic( rsh.rawYUVTextures, yuv );

	//rrf.frame->DrawStretchRawYUV( rrf.frame, x, y, w, h, s1, t1, s2, t2 );
}

void RF_DrawStretchPoly( const poly_t *poly, float x_offset, float y_offset )
{
	//rrf.frame->DrawStretchPoly( rrf.frame, poly, x_offset, y_offset );
}

void RF_SetScissor( int x, int y, int w, int h )
{
	//rrf.frame->SetScissor( rrf.frame, x, y, w, h );
	//Vector4Set( rrf.scissor, x, y, w, h );
}

void RF_GetScissor( int *x, int *y, int *w, int *h )
{
 // if( x )
 // 	*x = rrf.scissor[0];
 // if( y )
 // 	*y = rrf.scissor[1];
 // if( w )
 // 	*w = rrf.scissor[2];
 // if( h )
 // 	*h = rrf.scissor[3];
}

void RF_ResetScissor( void )
{
//	rrf.frame->ResetScissor( rrf.frame );
//	Vector4Set( rrf.scissor, 0, 0, glConfig.width, glConfig.height );
}

void RF_SetCustomColor( int num, int r, int g, int b )
{
 // byte_vec4_t rgba;

 // Vector4Set( rgba, r, g, b, 255 );
 // 
 // if( *(int *)rgba != *(int *)rrf.customColors[num] ) {
 // 	rrf.adapter.cmdPipe->SetCustomColor( rrf.adapter.cmdPipe, num, r, g, b );
 // 	*(int *)rrf.customColors[num] = *(int *)rgba;
 // }
}

void RF_ScreenShot( const char *path, const char *name, const char *fmtstring, bool silent )
{
 // if( RF_RenderingEnabled() )
 // 	rrf.adapter.cmdPipe->ScreenShot( rrf.adapter.cmdPipe, path, name, fmtstring, silent );
}

void RF_EnvShot( const char *path, const char *name, unsigned pixels )
{
//	if( RF_RenderingEnabled() )
//		rrf.adapter.cmdPipe->EnvShot( rrf.adapter.cmdPipe, path, name, pixels );
}

bool RF_RenderingEnabled( void )
{
	return GLimp_RenderingEnabled();
}

const char *RF_GetSpeedsMessage( char *out, size_t size )
{
	ri.Mutex_Lock( rf.speedsMsgLock );
	Q_strncpyz( out, rf.speedsMsg, size );
	ri.Mutex_Unlock( rf.speedsMsgLock );
	return out;
}

int RF_GetAverageFramerate( void )
{
	return rf.fps.average;
}

void RF_ReplaceRawSubPic( shader_t *shader, int x, int y, int width, int height, uint8_t *data )
{
	R_ReplaceRawSubPic( shader, x, y, width, height, data );
}

void RF_BeginAviDemo( void )
{
	//RF_AdapterWait( &rrf.adapter );
}

void RF_WriteAviFrame( int frame, bool scissor )
{
	int x, y, w, h;
	const char *writedir, *gamedir;
	size_t path_size;
	char *path;
	char name[32];
	
	if( !R_IsRenderingToScreen() )
		return;

	if( scissor )
	{
		x = rsc.refdef.x;
		y = glConfig.height - rsc.refdef.height - rsc.refdef.y;
		w = rsc.refdef.width;
		h = rsc.refdef.height;
	}
	else
	{
		x = 0;
		y = 0;
		w = glConfig.width;
		h = glConfig.height;
	}
	
	writedir = FS_WriteDirectory();
	gamedir = FS_GameDirectory();
	path_size = strlen( writedir ) + 1 + strlen( gamedir ) + strlen( "/avi/" ) + 1;
	path = alloca( path_size );
	Q_snprintfz( path, path_size, "%s/%s/avi/", writedir, gamedir );
	Q_snprintfz( name, sizeof( name ), "%06i", frame );
	
//	RF_AdapterWait( &rrf.adapter );
	
	rrf.adapter.cmdPipe->AviShot( rrf.adapter.cmdPipe, path, name, x, y, w, h );
}

void RF_StopAviDemo( void )
{
//	RF_AdapterWait( &rrf.adapter );
}

void RF_TransformVectorToScreen( const refdef_t *rd, const vec3_t in, vec2_t out )
{
	mat4_t p, m;
	vec4_t temp, temp2;
 	
	if( !rd || !in || !out )
		return;
 	
	temp[0] = in[0];
	temp[1] = in[1];
	temp[2] = in[2];
	temp[3] = 1.0f;
 	
	if( rd->rdflags & RDF_USEORTHO ) {
		Matrix4_OrthogonalProjection( rd->ortho_x, rd->ortho_x, rd->ortho_y, rd->ortho_y,
			-4096.0f, 4096.0f, p );
	}
	else {
		Matrix4_InfinitePerspectiveProjection( rd->fov_x, rd->fov_y, Z_NEAR, rrf.cameraSeparation,
			p, glConfig.depthEpsilon );
	}
 	
	if( rd->rdflags & RDF_FLIPPED ) {
		p[0] = -p[0];
	}
 	
	Matrix4_Modelview( rd->vieworg, rd->viewaxis, m );
 	
	Matrix4_Multiply_Vector( m, temp, temp2 );
	Matrix4_Multiply_Vector( p, temp2, temp );
 	
	if( !temp[3] )
 		return;
 	
	out[0] = rd->x + ( temp[0] / temp[3] + 1.0f ) * rd->width * 0.5f;
	out[1] = glConfig.height - (rd->y + ( temp[1] / temp[3] + 1.0f ) * rd->height * 0.5f);
}

bool RF_LerpTag( orientation_t *orient, const model_t *mod, int oldframe, int frame, float lerpfrac, const char *name )
{
	if( !orient )
		return false;
 	
	VectorClear( orient->origin );
	Matrix3_Identity( orient->axis );
 	
	if( !name )
		return false;
 	
	if( mod->type == mod_alias )
		return R_AliasModelLerpTag( orient, (const maliasmodel_t *)mod->extradata, oldframe, frame, lerpfrac, name );
 	
	return false;
}

void RF_LightForOrigin( const vec3_t origin, vec3_t dir, vec4_t ambient, vec4_t diffuse, float radius )
{
	R_LightForOrigin( origin, dir, ambient, diffuse, radius, false );
}

/*
* RF_GetShaderForOrigin
*
* Trace 64 units in all axial directions to find the closest surface
*/
shader_t *RF_GetShaderForOrigin( const vec3_t origin )
{
	int i, j;
	vec3_t dir, end;
	rtrace_t tr;
	shader_t *best = NULL;
	float best_frac = 1000.0f;

	for( i = 0; i < 3; i++ ) {
		VectorClear( dir );

		for( j = -1; j <= 1; j += 2 ) {
			dir[i] = j;
			VectorMA( origin, 64, dir, end );

			R_TraceLine( &tr, origin, end, 0 );
			if( !tr.shader ) {
				continue;
			}

			if( tr.fraction < best_frac ) {
				best = tr.shader;
				best_frac = tr.fraction;
			}
		}
	}

	return best;
}

struct cinematics_s *RF_GetShaderCinematic( shader_t *shader )
{
	if( !shader ) {
		return NULL;
	}
	return R_GetCinematicById( shader->cin );
}
