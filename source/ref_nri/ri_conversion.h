#ifndef RI_CONVERSION_H
#define RI_CONVERSION_H

#include "ri_types.h"

static inline enum RIVendor_e VendorFromID(uint32_t vendorID) {
    switch (vendorID) {
		case 0x10DE:
			return RI_NVIDIA;
		case 0x1002:
			return RI_AMD;
		case 0x8086:
			return RI_INTEL;
	}
	return RI_UNKNOWN;
}

#if DEVICE_IMPL_VULKAN
static inline VkImageType ri_vk_RITextureTypeToVKImageType( enum RITextureType_e e )
{
	switch( e ) {
		case RI_TEXTURE_1D:
			return VK_IMAGE_TYPE_1D;
		case RI_TEXTURE_2D:
			return VK_IMAGE_TYPE_2D;
		case RI_TEXTURE_3D:
			return VK_IMAGE_TYPE_3D;
	}
	// this shouldn't happen
	assert( false );
	return VK_IMAGE_TYPE_MAX_ENUM;
}
#endif

#endif
