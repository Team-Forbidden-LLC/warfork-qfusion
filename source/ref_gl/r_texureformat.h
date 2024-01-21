#ifndef R_NRI_TEXFORMAT_H
#define R_NRI_TEXFORMAT_H

#include "r_nri.h"
#include <assert.h>

typedef enum {
    UNKNOWN,

    R_FORMAT_R8_UNORM,
    R_FORMAT_R8_SNORM,
    R_FORMAT_R8_UINT,
    R_FORMAT_R8_SINT,
    R_FORMAT_RG8_UNORM,
    R_FORMAT_RG8_SNORM,
    R_FORMAT_RG8_UINT,
    R_FORMAT_RG8_SINT,
    R_FORMAT_BGRA8_UNORM,
    R_FORMAT_BGRA8_SRGB,
    R_FORMAT_BGR8_UNORM,
    R_FORMAT_RGB8_UNORM,
    R_FORMAT_RGBA8_UNORM,
    R_FORMAT_RGBA8_SNORM,
    R_FORMAT_RGBA8_UINT,
    R_FORMAT_RGBA8_SINT,
    R_FORMAT_RGBA8_SRGB,
    R_FORMAT_R16_UNORM,
    R_FORMAT_R16_SNORM,
    R_FORMAT_R16_UINT,
    R_FORMAT_R16_SINT,
    R_FORMAT_R16_SFLOAT,
    R_FORMAT_RG16_UNORM,
    R_FORMAT_RG16_SNORM,
    R_FORMAT_RG16_UINT,
    R_FORMAT_RG16_SINT,
    R_FORMAT_RG16_SFLOAT,
    R_FORMAT_RGBA16_UNORM,
    R_FORMAT_RGBA16_SNORM,
    R_FORMAT_RGBA16_UINT,
    R_FORMAT_RGBA16_SINT,
    R_FORMAT_RGBA16_SFLOAT,
    R_FORMAT_R32_UINT,
    R_FORMAT_R32_SINT,
    R_FORMAT_R32_SFLOAT,
    R_FORMAT_RG32_UINT,
    R_FORMAT_RG32_SINT,
    R_FORMAT_RG32_SFLOAT,
    R_FORMAT_RGB32_UINT,
    R_FORMAT_RGB32_SINT,
    R_FORMAT_RGB32_SFLOAT,
    R_FORMAT_RGBA32_UINT,
    R_FORMAT_RGBA32_SINT,
    R_FORMAT_RGBA32_SFLOAT,
    R_FORMAT_R10_G10_B10_A2_UNORM,
    R_FORMAT_R10_G10_B10_A2_UINT,
    R_FORMAT_R11_G11_B10_UFLOAT,
    R_FORMAT_R9_G9_B9_E5_UFLOAT,
    R_FORMAT_BC1_RGBA_UNORM,
    R_FORMAT_BC1_RGBA_SRGB,
    R_FORMAT_BC2_RGBA_UNORM,
    R_FORMAT_BC2_RGBA_SRGB,
    R_FORMAT_BC3_RGBA_UNORM,
    R_FORMAT_BC3_RGBA_SRGB,
    R_FORMAT_BC4_R_UNORM,
    R_FORMAT_BC4_R_SNORM,
    R_FORMAT_BC5_RG_UNORM,
    R_FORMAT_BC5_RG_SNORM,
    R_FORMAT_BC6H_RGB_UFLOAT,
    R_FORMAT_BC6H_RGB_SFLOAT,
    R_FORMAT_BC7_RGBA_UNORM,
    R_FORMAT_BC7_RGBA_SRGB,
    R_FORMAT_D16_UNORM,
    R_FORMAT_D24_UNORM_S8_UINT,
    R_FORMAT_D32_SFLOAT,
    R_FORMAT_D32_SFLOAT_S8_UINT_X24,
    R_FORMAT_R24_UNORM_X8,
    R_FORMAT_X24_R8_UINT,
    R_FORMAT_X32_R8_UINT_X24,
    R_FORMAT_R32_SFLOAT_X8_X24,
} texture_format_t;


NriFormat R_ToNRIFormat(texture_format_t format);
uint32_t R_FormatChannelCount(texture_format_t format);
uint32_t R_FormatBitSizePerBlock(texture_format_t format);
#endif

