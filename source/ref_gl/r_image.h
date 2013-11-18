/*
Copyright (C) 2013 Victor Luchits

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

#ifndef __R_IMAGE_H__
#define __R_IMAGE_H__

#endif // __R_IMAGE_H__

enum
{
	IT_NONE
	,IT_CLAMP			= 1<<0
	,IT_NOMIPMAP		= 1<<1
	,IT_NOPICMIP		= 1<<2
	,IT_SKY				= 1<<3
	,IT_CUBEMAP			= 1<<4
	,IT_HEIGHTMAP		= 1<<5
	,IT_FLIPX			= 1<<6
	,IT_FLIPY			= 1<<7
	,IT_FLIPDIAGONAL	= 1<<8
	,IT_NOCOMPRESS		= 1<<9
	,IT_DEPTH			= 1<<10
	,IT_NORMALMAP		= 1<<11
	,IT_FRAMEBUFFER		= 1<<12
	,IT_NOFILTERING		= 1<<13
	,IT_LUMINANCE		= 1<<14
	,IT_BGRA			= 1<<15
};

#define IT_CINEMATIC		( IT_NOPICMIP|IT_NOMIPMAP|IT_CLAMP|IT_NOCOMPRESS )
#define IT_PORTALMAP		( IT_NOMIPMAP|IT_NOCOMPRESS|IT_NOPICMIP|IT_CLAMP )
#define IT_SHADOWMAP		( IT_NOMIPMAP|IT_NOCOMPRESS|IT_NOPICMIP|IT_CLAMP|IT_NOCOMPRESS )

typedef struct image_s
{
	char			*name;						// game path, not including extension
	int				registrationSequence;

	char			extension[8];				// file extension
	int				flags;
	GLuint			texnum;						// gl texture binding
	int				width, height;				// source image
	int				upload_width,
					upload_height;				// after power of two and picmip
	int				samples;
	int				fbo;						// frame buffer object texture is attached to
	unsigned int	framenum;					// r_framecount texture was updated (rendered to)
	float			bumpScale;
	struct image_s	*next, *prev;
} image_t;

void R_InitImages( void );
void R_TouchImage( image_t *image );
void R_FreeUnusedImages( void );
void R_ShutdownImages( void );
void R_InitViewportTexture( image_t **texture, const char *name, int id, 
	int viewportWidth, int viewportHeight, int size, int flags, int samples );
int	R_GetPortalTextureId( const int viewportWidth, const int viewportHeight, const int flags );
image_t *R_GetPortalTexture( int id, int viewportWidth, int viewportHeight, int flags );
image_t *R_GetShadowmapTexture( int id, int viewportWidth, int viewportHeight, int flags );
void R_InitDrawFlatTexture( void );
void R_FreeImageBuffers( void );

void R_ScreenShot_f( void );
void R_EnvShot_f( void );
void R_ImageList_f( void );
void R_TextureMode( char *string );
void R_AnisotropicFilter( int value );

image_t *R_LoadImage( const char *name, qbyte **pic, int width, int height, int flags, int samples );
image_t	*R_FindImage( const char *name, const char *suffix, int flags, float bumpScale );
void R_ReplaceImage( image_t *image, qbyte **pic, int width, int height, int flags, int samples );

void R_BeginAviDemo( void );
void R_WriteAviFrame( int frame, qboolean scissor );
void R_StopAviDemo( void );
