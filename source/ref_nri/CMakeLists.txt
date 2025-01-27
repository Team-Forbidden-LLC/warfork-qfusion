project(ref_nri)

file(GLOB REF_NRI_HEADERS
	"*.h"
	"../gameshared/q_*.h"
	"../gameshared/anorms.h"
	"../gameshared/config.h"
	"../qcommon/bsp.h"
	"../qcommon/patch.h"
	"../qcommon/iqm.h"
	"../qcommon/qfiles.h"
	"../qalgo/*.h"
	"${REF_BASE_DIR}/*.h"
	)

file(GLOB REF_NRI_COMMON_SOURCES
	"*.c"
	"../ref_base/*.c"
	"../ref_base/*.cpp"
	"*.cpp"
	"../gameshared/q_*.c"
	"../qalgo/*.c"
	"../qcommon/bsp.c"
	"../qcommon/patch.c"
	)

file(GLOB STB_HEADERS
    "${STB_INCLUDE_DIR}/stb_image.h"
    "${STB_INCLUDE_DIR}/stb_image_write.h"
)


add_library(ref_nri SHARED ${REF_NRI_HEADERS} ${REF_NRI_COMMON_SOURCES} ${REF_NRI_PLATFORM_SOURCES})
target_include_directories(ref_nri PRIVATE ${STB_INCLUDE_DIR} "../ref_base" "${NRI_DIR}/External/vulkan/include")
target_link_libraries(ref_nri PRIVATE NRI)
target_link_libraries(ref_nri PRIVATE volk)
target_link_libraries(ref_nri PRIVATE VulkanMemoryAllocator)
target_link_libraries(ref_nri PRIVATE glslang::glslang glslang::glslang-default-resource-limits glslang::SPIRV glslang::SPVRemapper qcore volk)
target_include_directories(ref_nri PRIVATE ${NRI_INCLUDE_DIR})
target_include_directories(ref_nri PRIVATE ${MINIZ_INCLUDE_DIR})

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  target_compile_definitions (ref_nri PRIVATE VK_USE_PLATFORM_WIN32_KHR)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_compile_definitions (ref_nri PRIVATE VK_USE_PLATFORM_METAL_EXT VK_ENABLE_BETA_EXTENSIONS)
else ()
  if (USE_GRAPHICS_X11)
        find_path (XLIB_HEADERS NAMES "X11/Xlib.h")

        if (NOT XLIB_HEADERS)
            message (FATAL_ERROR "Can't find 'X11/xlib.h'.(libx11-dev is not installed?)")
        endif ()

    	target_compile_definitions (ref_nri PRIVATE VK_USE_PLATFORM_XLIB_KHR)
   endif ()

   if (USE_GRAPHICS_WAYLAND)
        find_path (WAYLAND_HEADERS NAMES "wayland-client.h")

        if (NOT WAYLAND_HEADERS)
            message (FATAL_ERROR "Can't find 'wayland-client.h'.(libwayland-dev is not installed?)")
        endif ()

        target_compile_definitions (ref_nri PRIVATE VK_USE_PLATFORM_WAYLAND_KHR)
    endif ()
endif ()

qf_set_output_dir(ref_nri libs)

