/*
Copyright (C) 1997-2001 Id Software, Inc.
Copyright (C) 2002-2013 Victor Luchits

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
#ifndef R_RESOURCE_UPLOAD_H
#define R_RESOURCE_UPLOAD_H

#include "r_renderer.h"

static const uint32_t SizeOfStageBufferByte = ByteToMB * 8;


void R_InitResourceUpload();

typedef struct {
	NriAccessBits currentAccess;
	NriBuffer *target;
	size_t numBytes;
	size_t byteOffset;

	// begin mapping
	void *data;

	struct {
		NriBuffer *backing;
		size_t byteOffset;
	} internal;
} buffer_upload_desc_t;

void R_ResourceBeginCopyBuffer( buffer_upload_desc_t *action );
void R_ResourceEndCopyBuffer( buffer_upload_desc_t *action );

typedef struct {
	NriAccessAndLayout currentAccess;
	NriTexture *target;
	
	// https://github.com/microsoft/DirectXTex/wiki/Image
  uint32_t sliceNum;
  uint32_t rowPitch;
  uint32_t slicePitch;
	NriAccessAndLayout currentAccessAndLayout;

	uint32_t arrayOffset;
	uint32_t mipOffset;

	// begin mapping
	void *data;
  uint32_t alignRowPitch;
  uint32_t alignSlicePitch;

	struct {
		NriBuffer *backing;
		size_t byteOffset;
	} internal;
} texture_upload_desc_t;

void R_ResourceBeginCopyTexture( texture_upload_desc_t *desc );
void R_ResourceEndCopyTexture( texture_upload_desc_t* desc);

void R_ResourceSubmit();

#endif
