

#ifndef RI_TYPES_H
#define RI_TYPES_H

#include "../gameshared/q_arch.h"
#include "math/qmath.h"
#include "qtypes.h"
#include "ri_format.h"


#define DEVICE_SUPPORT_VULKAN

#ifdef DEVICE_SUPPORT_VULKAN
#include "volk.h"
#include "vk_mem_alloc.h"
#define DEVICE_IMPL_VULKAN 1
#else
#define DEVICE_IMPL_VULKAN 0
#endif

#ifdef DEVICE_SUPPORT_MTL
#define DEVICE_IMPL_MTL 1
#else
#define DEVICE_IMPL_MTL 0
#endif

#ifdef DEVICE_SUPPORT_D3D11
#define DEVICE_IMPL_D3D11 1
#else
#define DEVICE_IMPL_D3D11 0
#endif

#ifdef DEVICE_SUPPORT_D3D12
#define DEVICE_IMPL_D3D12 1
#else
#define DEVICE_IMPL_D3D12 0
#endif

#if ( ( DEVICE_IMPL_D3D12 + DEVICE_IMPL_D3D11 + DEVICE_IMPL_MTL + DEVICE_IMPL_VULKAN ) > 1 )
#define DEVICE_IMPL_MUTLI 1
#else
#define DEVICE_IMPL_MUTLI 0
#endif


#if(DEVICE_IMPL_MUTLI)


#if(DEVICE_IMPL_VULKAN)
  #define GPU_VULKAN_BLOCK(backend, block) if(backend->api == DEVICE_API_VK) { Q_UNPAREN(block) }
#else
  #define GPU_VULKAN_BLOCK(backend, block) 
#endif

#if(DEVICE_IMPL_D3D11 )
  #define GPU_D3D11_BLOCK(backend,  block) if(backend->api == DEVICE_API_D3D11) { Q_UNPAREN(block) }
#else
  #define GPU_D3D11_BLOCK(backend,  block) 
#endif

#if(DEVICE_IMPL_D3D12 )
  #define GPU_D3D12_BLOCK(backend,  block) if(backend->api == DEVICE_API_D3D12) { Q_UNPAREN(block) }
#else
  #define GPU_D3D12_BLOCK(backend,  block) 
#endif

#if(DEVICE_IMPL_MTL )
  #define GPU_MTL_BLOCK(backend, block) if(backend->api == DEVICE_API_MTL) { Q_UNPAREN(block) }
#else
  #define GPU_MTL_BLOCK(backend, block) 
#endif

#else

#if(DEVICE_IMPL_VULKAN)
  #define GPU_VULKAN_BLOCK(backend, block)  { assert(backend->api == DEVICE_API_VK);  Q_UNPAREN(block) }
#else
  #define GPU_VULKAN_BLOCK(backend, block) 
#endif

#if(DEVICE_IMPL_D3D11 )
  #define GPU_D3D11_BLOCK(backend, block)  { Q_UNPAREN(block) }
#else
  #define GPU_D3D11_BLOCK(backend, device, block) 
#endif

#if(DEVICE_IMPL_D3D12 )
  #define GPU_D3D12_BLOCK(backend, block) { Q_UNPAREN(block)}
#else
  #define GPU_D3D12_BLOCK(backend, device, block) 
#endif

#if(DEVICE_IMPL_MTL )
  #define GPU_MTL_BLOCK(backend, block) { Q_UNPAREN(block) }
#else
  #define GPU_MTL_BLOCK(backend, device, block) 
#endif

#endif

#define R_VK_ADD_STRUCT(current, next) { \
  void* __pNext = (void*)((current)->pNext); \
  (current)->pNext = (next); \
  (next)->pNext = __pNext; \
}
#define VK_WrapResult( res ) __vk_WrapResult( res, __FILE__, __FUNCTION__, __LINE__ )

static inline bool __vk_WrapResult(VkResult result, const char *sourceFilename, const char *functionName, int sourceLine) {
	if(result != VK_SUCCESS) {
		Com_Printf( "RI: VK %i, file %s:%i (%s)\n", result, sourceFilename, sourceLine, functionName);
		return false;
	}
	return true;
}


#define RI_QUEUE_GRAPHICS_BIT 0x1
#define RI_QUEUE_COMPUTE_BIT 0x2
#define RI_QUEUE_TRANSFER_BIT 0x4
#define RI_QUEUE_SPARSE_BINDING_BIT 0x8
#define RI_QUEUE_VIDEO_DECODE_BIT 0x10
#define RI_QUEUE_VIDEO_ENCODE_BIT 0x20
#define RI_QUEUE_PROTECTED_BIT 0x40
#define RI_QUEUE_OPTICAL_FLOW_BIT_NV 0x80
#define RI_QUEUE_INVALID 0x0

enum RITextureViewType_s {
    RI_VIEWTYPE_SHADER_RESOURCE_1D,
    RI_VIEWTYPE_SHADER_RESOURCE_1D_ARRAY,
    RI_VIEWTYPE_SHADER_RESOURCE_STORAGE_1D,
    RI_VIEWTYPE_SHADER_RESOURCE_STORAGE_1D_ARRAY,
  	RI_VIEWTYPE_SHADER_RESOURCE_2D,
    RI_VIEWTYPE_SHADER_RESOURCE_2D_ARRAY,
    RI_VIEWTYPE_SHADER_RESOURCE_CUBE,
    RI_VIEWTYPE_SHADER_RESOURCE_CUBE_ARRAY,
    RI_VIEWTYPE_SHADER_RESOURCE_STORAGE_2D,
    RI_VIEWTYPE_SHADER_RESOURCE_STORAGE_2D_ARRAY,
   
  	RI_VIEWTYPE_COLOR_ATTACHMENT,
    RI_VIEWTYPE_DEPTH_STENCIL_ATTACHMENT,
    RI_VIEWTYPE_DEPTH_READONLY_STENCIL_ATTACHMENT,
    RI_VIEWTYPE_DEPTH_ATTACHMENT_STENCIL_READONLY,
    RI_VIEWTYPE_DEPTH_STENCIL_READONLY,
    RI_VIEWTYPE_SHADING_RATE_ATTACHMENT

};

enum RIUsageBits_e {
	RI_USAGE_NONE,
	RI_USAGE_RESOURCE,
	RI_USAGE_RESOURCE_STORAGE,
	RI_USAGE_COLOR_ATTACHMENT,
	RI_USAGE_DEPTH_STENCIL_ATTACHMENT,
	RI_USAGE_SHADING_RATE,

};

enum RISampleCount_e
{
    RI_SAMPLE_COUNT_1 = 1,
    RI_SAMPLE_COUNT_2 = 2,
    RI_SAMPLE_COUNT_4 = 4,
    RI_SAMPLE_COUNT_8 = 8,
    RI_SAMPLE_COUNT_16 = 16,
    RI_SAMPLE_COUNT_COUNT = 5,
};


enum RIDeviceAPI_e { 
	RI_DEVICE_API_UNKNOWN, 
	RI_DEVICE_API_VK, 
	RI_DEVICE_API_D3D11, 
	RI_DEVICE_API_D3D12, 
	RI_DEVICE_API_MTL 
};

enum RISwapchainFormat_e { 
	RI_SWAPCHAIN_BT709_G10_16BIT, 
	RI_SWAPCHAIN_BT709_G22_8BIT, 
	RI_SWAPCHAIN_BT709_G22_10BIT, 
	RI_SWAPCHAIN_BT2020_G2084_10BIT 
};

enum RIQueueType_e { 
	RI_QUEUE_GRAPHICS, 
	RI_QUEUE_COMPUTE, 
	RI_QUEUE_COPY, 
	RI_QUEUE_LEN 
};

enum RIResult_e { 
	RI_FAIL = -1, 
	RI_SUCCESS = 0, 
	RI_INCOMPLETE 
};

enum RIBufferUsage_e {
	RI_BUFFER_USAGE_NONE = 0,
	RI_BUFFER_USAGE_SHADER_RESOURCE = 0x1,
	RI_BUFFER_USAGE_SHADER_RESOURCE_STORAGE = 0x2,
	RI_BUFFER_USAGE_VERTEX_BUFFER = 0x4,
	RI_BUFFER_USAGE_INDEX_BUFFER = 0x8,
	RI_BUFFER_USAGE_CONSTANT_BUFFER = 0x10,
	RI_BUFFER_USAGE_ARGUMENT_BUFFER = 0x20,
};

enum RITextureType_e { 
	RI_TEXTURE_1D, 
	RI_TEXTURE_2D, 
	RI_TEXTURE_3D 
};

enum RIVendor_e { 
	RI_UNKNOWN, 
	RI_NVIDIA, 
	RI_AMD, 
	RI_INTEL 
};

enum RIWindowType_e {
	RI_WINDOW_X11,
	RI_WINDOW_WIN32,
	RI_WINDOW_METAL,
	RI_WINDOW_WAYLAND
};

struct RIMemory_s {

};

struct RIBuffer_s {
	union {
    #if(DEVICE_IMPL_VULKAN)
    struct {
    	VkBuffer buffer;
    } vk;
    #endif
	};

};

struct RITexture_s {
	
	uint8_t type; // RITextureType_e
	uint8_t mipNum;
	uint8_t sampleNum;

	uint16_t width;
	uint16_t height;
	uint16_t depth;
	uint16_t layerNum;

	union {
    #if(DEVICE_IMPL_VULKAN)
    struct {
    	VkImage image;	
    } vk;
    #endif
	};

};

struct RISwapchain_s {
	uint16_t imageCount;
  struct RITexture_s images[8];
	union {
    #if(DEVICE_IMPL_VULKAN)
    struct {
    	VkSwapchainKHR swapchain;
    	VkSurfaceKHR surface;
    	VkSemaphore imageAcquireSem[8];
    	VkSemaphore finishSem[8];
    } vk;
    #endif
	};

};

struct RICmd_s {
	union {
    #if(DEVICE_IMPL_VULKAN)
    struct {
    	VkCommandBuffer cmd;  
    } vk;
    #endif
	};
};

struct RIQueue_s {
  union {
    #if(DEVICE_IMPL_VULKAN)
      struct {
        VkQueueFlags queueFlags;
        uint16_t queueFamilyIdx;
        uint16_t slotIdx;
        VkQueue queue;
      } vk;
    #endif
  };
};


struct RIRenderer_s {
  uint8_t api; // RIDeviceAPI_e  
  union {
    #if(DEVICE_IMPL_VULKAN)
      struct {
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessageUtils;
      } vk;
    #endif
  };
};


struct RIBackendInit_s {
  uint8_t api; // RIDeviceAPI_e 
  const char* applicationName;
  union {
    #if(DEVICE_IMPL_VULKAN)
    struct {
      uint32_t enableValidationLayer: 1;
      size_t numFilterLayers;
      const char* filterLayers[]; // filter layers for the renderer 
    } vk;
    #endif
    #if(DEVICE_IMPL_MTL)
    #endif
  };
};

struct RIDeviceFeatures_s {
};

struct RIPhysicalAdapter_s {
	char name[256];
	uint64_t luid;
	uint64_t videoMemorySize;
	uint64_t systemMemorySize;
	uint32_t deviceId;
	uint8_t vendor; // RIVendor_e
	
	// Viewports
	uint32_t viewportMaxNum;
	int32_t viewportBoundsRange[2];

	// Attachments
	uint16_t attachmentMaxDim;
	uint16_t attachmentLayerMaxNum;
	uint16_t colorAttachmentMaxNum;

	// Multi-sampling
	uint8_t colorSampleMaxNum;
	uint8_t depthSampleMaxNum;
	uint8_t stencilSampleMaxNum;
	uint8_t zeroAttachmentsSampleMaxNum;
	uint8_t textureColorSampleMaxNum;
	uint8_t textureIntegerSampleMaxNum;
	uint8_t textureDepthSampleMaxNum;
	uint8_t textureStencilSampleMaxNum;
	uint8_t storageTextureSampleMaxNum;

	// Resource dimensions
	uint16_t texture1DMaxDim;
	uint16_t texture2DMaxDim;
	uint16_t texture3DMaxDim;
	uint16_t textureArrayLayerMaxNum;
	uint32_t typedBufferMaxDim;

	// Memory
	uint64_t deviceUploadHeapSize; // ReBAR
	uint32_t memoryAllocationMaxNum;
	uint32_t samplerAllocationMaxNum;
	uint32_t constantBufferMaxRange;
	uint32_t storageBufferMaxRange;
	uint32_t bufferTextureGranularity;
	uint64_t bufferMaxSize;

	// Memory alignment
	uint32_t uploadBufferTextureRowAlignment;
	uint32_t uploadBufferTextureSliceAlignment;
	uint32_t bufferShaderResourceOffsetAlignment;
	uint32_t constantBufferOffsetAlignment;
	//uint32_t scratchBufferOffsetAlignment;
	//uint32_t shaderBindingTableAlignment;

	// Pipeline layout
	// D3D12 only: rootConstantSize + descriptorSetNum * 4 + rootDescriptorNum * 8 <= 256 (see "FitPipelineLayoutSettingsIntoDeviceLimits")
	uint32_t pipelineLayoutDescriptorSetMaxNum;
	uint32_t pipelineLayoutRootConstantMaxSize;
	uint32_t pipelineLayoutRootDescriptorMaxNum;

	// Descriptor set
	uint32_t descriptorSetSamplerMaxNum;
	uint32_t descriptorSetConstantBufferMaxNum;
	uint32_t descriptorSetStorageBufferMaxNum;
	uint32_t descriptorSetTextureMaxNum;
	uint32_t descriptorSetStorageTextureMaxNum;

	// Shader resources
	uint32_t perStageDescriptorSamplerMaxNum;
	uint32_t perStageDescriptorConstantBufferMaxNum;
	uint32_t perStageDescriptorStorageBufferMaxNum;
	uint32_t perStageDescriptorTextureMaxNum;
	uint32_t perStageDescriptorStorageTextureMaxNum;
	uint32_t perStageResourceMaxNum;

	// Vertex shader
	uint32_t vertexShaderAttributeMaxNum;
	uint32_t vertexShaderStreamMaxNum;
	uint32_t vertexShaderOutputComponentMaxNum;

	// Tessellation shaders
	float tessControlShaderGenerationMaxLevel;
	uint32_t tessControlShaderPatchPointMaxNum;
	uint32_t tessControlShaderPerVertexInputComponentMaxNum;
	uint32_t tessControlShaderPerVertexOutputComponentMaxNum;
	uint32_t tessControlShaderPerPatchOutputComponentMaxNum;
	uint32_t tessControlShaderTotalOutputComponentMaxNum;
	uint32_t tessEvaluationShaderInputComponentMaxNum;
	uint32_t tessEvaluationShaderOutputComponentMaxNum;

	// Geometry shader
	uint32_t geometryShaderInvocationMaxNum;
	uint32_t geometryShaderInputComponentMaxNum;
	uint32_t geometryShaderOutputComponentMaxNum;
	uint32_t geometryShaderOutputVertexMaxNum;
	uint32_t geometryShaderTotalOutputComponentMaxNum;

	// Fragment shader
	uint32_t fragmentShaderInputComponentMaxNum;
	uint32_t fragmentShaderOutputAttachmentMaxNum;
	uint32_t fragmentShaderDualSourceAttachmentMaxNum;

	// Compute shader
	uint32_t computeShaderSharedMemoryMaxSize;
	uint32_t computeShaderWorkGroupMaxNum[3];
	uint32_t computeShaderWorkGroupInvocationMaxNum;
	uint32_t computeShaderWorkGroupMaxDim[3];

	// Ray tracing
	//uint32_t rayTracingShaderGroupIdentifierSize;
	//uint32_t rayTracingShaderTableMaxStride;
	//uint32_t rayTracingShaderRecursionMaxDepth;
	//uint32_t rayTracingGeometryObjectMaxNum;

	// Mesh shaders
	//uint32_t meshControlSharedMemoryMaxSize;
	//uint32_t meshControlWorkGroupInvocationMaxNum;
	//uint32_t meshControlPayloadMaxSize;
	//uint32_t meshEvaluationOutputVerticesMaxNum;
	//uint32_t meshEvaluationOutputPrimitiveMaxNum;
	//uint32_t meshEvaluationOutputComponentMaxNum;
	//uint32_t meshEvaluationSharedMemoryMaxSize;
	//uint32_t meshEvaluationWorkGroupInvocationMaxNum;

	// Precision bits
	uint32_t viewportPrecisionBits;
	uint32_t subPixelPrecisionBits;
	uint32_t subTexelPrecisionBits;
	uint32_t mipmapPrecisionBits;

	// Other
	uint64_t timestampFrequencyHz;
	uint32_t drawIndirectMaxNum;
	float samplerLodBiasMin;
	float samplerLodBiasMax;
	float samplerAnisotropyMax;
	int32_t texelOffsetMin;
	uint32_t texelOffsetMax;
	int32_t texelGatherOffsetMin;
	uint32_t texelGatherOffsetMax;
	uint32_t clipDistanceMaxNum;
	uint32_t cullDistanceMaxNum;
	uint32_t combinedClipAndCullDistanceMaxNum;
	//uint8_t shadingRateAttachmentTileSize;
	//uint8_t shaderModel; // major * 10 + minor

	// Tiers (0 - unsupported)
	// 1 - 1/2 pixel uncertainty region and does not support post-snap degenerates
	// 2 - reduces the maximum uncertainty region to 1/256 and requires post-snap degenerates not be culled
	// 3 - maintains a maximum 1/256 uncertainty region and adds support for inner input coverage, aka "SV_InnerCoverage"
	//uint8_t conservativeRasterTier;

	// 1 - a single sample pattern can be specified to repeat for every pixel ("locationNum / sampleNum" must be 1 in "CmdSetSampleLocations")
	// 2 - four separate sample patterns can be specified for each pixel in a 2x2 grid ("locationNum / sampleNum" can be up to 4 in "CmdSetSampleLocations")
	//uint8_t sampleLocationsTier;

	// 1 - DXR 1.0: full raytracing functionality, except features below
	// 2 - DXR 1.1: adds - ray query, "CmdDispatchRaysIndirect", "GeometryIndex()" intrinsic, additional ray flags & vertex formats
	//uint8_t rayTracingTier;

	// 1 - shading rate can be specified only per draw
	// 2 - adds: per primitive shading rate, per "shadingRateAttachmentTileSize" shading rate, combiners, "SV_ShadingRate" support
	//uint8_t shadingRateTier;

	// 1 - unbound arrays with dynamic indexing
	// 2 - D3D12 dynamic resources: https://microsoft.github.io/DirectX-Specs/d3d/HLSL_SM_6_6_DynamicResources.html
	uint8_t bindlessTier;

	// Features
	uint32_t isTextureFilterMinMaxSupported : 1;
	uint32_t isLogicFuncSupported : 1;
	uint32_t isDepthBoundsTestSupported : 1;
	uint32_t isDrawIndirectCountSupported : 1;
	uint32_t isIndependentFrontAndBackStencilReferenceAndMasksSupported : 1;
	//uint32_t isLineSmoothingSupported : 1;
	uint32_t isCopyQueueTimestampSupported : 1;
	//uint32_t isMeshShaderPipelineStatsSupported : 1;
	uint32_t isEnchancedBarrierSupported : 1; // aka - can "Layout" be ignored?
	uint32_t isMemoryTier2Supported : 1;	  // a memory object can support resources from all 3 categories (buffers, attachments, all other textures)
	uint32_t isDynamicDepthBiasSupported : 1;
	//uint32_t isAdditionalShadingRatesSupported : 1;
	uint32_t isViewportOriginBottomLeftSupported : 1;
	uint32_t isRegionResolveSupported : 1;

	// Shader features
	uint32_t isShaderNativeI16Supported : 1;
	uint32_t isShaderNativeF16Supported : 1;
	uint32_t isShaderNativeI32Supported : 1;
	uint32_t isShaderNativeF32Supported : 1;
	uint32_t isShaderNativeI64Supported : 1;
	uint32_t isShaderNativeF64Supported : 1;
	uint32_t isShaderAtomicsI16Supported : 1;
	// uint32_t isShaderAtomicsF16Supported : 1;
	uint32_t isShaderAtomicsI32Supported : 1;
	// uint32_t isShaderAtomicsF32Supported : 1;
	uint32_t isShaderAtomicsI64Supported : 1;
	// uint32_t isShaderAtomicsF64Supported : 1;

	// Emulated features
	uint32_t isDrawParametersEmulationEnabled : 1;

	//// Extensions (unexposed are always supported)
	//uint32_t isSwapChainSupported : 1;	// swapchain Support
	//uint32_t isRayTracingSupported : 1; // raytracing support
	//uint32_t isMeshShaderSupported : 1; // meshshader support

	union {
#if ( DEVICE_IMPL_VULKAN )
		struct {
			uint32_t apiVersion;
			VkPhysicalDevice physicalDevice;
			
			uint32_t isSwapChainSupported : 1;	// swapchain Support
			uint32_t isBufferDeviceAddressSupported: 1;
			uint32_t isAMDDeviceCoherentMemorySupported: 1;
			//uint32_t YCbCrExtension : 1;
			//uint32_t FillModeNonSolid : 1;
			//uint32_t KHRRayQueryExtension : 1;
			//uint32_t AMDGCNShaderExtension : 1;
			//uint32_t AMDDrawIndirectCountExtension : 1;
			//uint32_t AMDShaderInfoExtension : 1;
			//uint32_t DescriptorIndexingExtension : 1;
			//uint32_t DynamicRenderingExtension : 1;
			//uint32_t ShaderSampledImageArrayDynamicIndexingSupported : 1;
			//uint32_t BufferDeviceAddressSupported : 1;
			//uint32_t DrawIndirectCountExtension : 1;
			//uint32_t DedicatedAllocationExtension : 1;
			//uint32_t DebugMarkerExtension : 1;
			//uint32_t MemoryReq2Extension : 1;
			//uint32_t FragmentShaderInterlockExtension : 1;
			//uint32_t BufferDeviceAddressExtension : 1;
			//uint32_t accelerationStructureExtension : 1;
			//uint32_t rayTracingPipelineExtension : 1;
			//uint32_t rayQueryExtension : 1;
			//uint32_t ShaderAtomicInt64Extension : 1;
			//uint32_t BufferDeviceAddressFeature : 1;
			//uint32_t ShaderFloatControlsExtension : 1;
			//uint32_t Spirv14Extension : 1;
			//uint32_t DeferredHostOperationsExtension : 1;
			//uint32_t DeviceFaultExtension : 1;
			//uint32_t DeviceFaultSupported : 1;
			//uint32_t ASTCDecodeModeExtension : 1;
			//uint32_t DeviceMemoryReportExtension : 1;
			//uint32_t AMDBufferMarkerExtension : 1;
			//uint32_t AMDDeviceCoherentMemoryExtension : 1;
			//uint32_t AMDDeviceCoherentMemorySupported : 1;
		} vk;
#endif
#if ( DEVICE_IMPL_MTL )
		struct {

		} mtl;
#endif
	};
};

struct RIDevice_s {
	struct RIPhysicalAdapter_s physicalAdapter;
  struct RIRenderer_s* renderer;
  struct RIQueue_s queues[RI_QUEUE_LEN];
  union {
    #if(DEVICE_IMPL_VULKAN)
    struct {
      uint32_t maintenance5Features: 1;
      uint32_t conservaitveRasterTier: 1;
      uint32_t swapchainMutableFormat: 1;
      uint32_t memoryBudget: 1;
      VkDevice device;
      VmaAllocator vmaAllocator;
    } vk; 
    #endif
    #if(DEVICE_IMPL_MTL)
    #endif
  };
};

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


#endif
