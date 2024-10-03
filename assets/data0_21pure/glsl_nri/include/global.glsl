#if defined(APPLY_FOG_COLOR)
  #define APPLY_ENV_MODULATE_COLOR
#else

  #if defined(APPLY_RGB_DISTANCERAMP) || defined(APPLY_RGB_CONST) || defined(APPLY_RGB_VERTEX) || defined(APPLY_RGB_ONE_MINUS_VERTEX) || defined(APPLY_RGB_GEN_DIFFUSELIGHT)
    #define APPLY_ENV_MODULATE_COLOR
  #else

    #if defined(APPLY_ALPHA_DISTANCERAMP) || defined(APPLY_ALPHA_CONST) || defined(APPLY_ALPHA_VERTEX) || defined(APPLY_ALPHA_ONE_MINUS_VERTEX)
    #define APPLY_ENV_MODULATE_COLOR
    #endif
  #endif
#endif

#define DESCRIPTOR_GLOBAL_SET 0
#define DESCRIPTOR_FRAME_SET 1
#define DESCRIPTOR_PASS_SET 2 
#define DESCRIPTOR_OBJECT_SET 3 

#include "resource.glsl"
#include "math_utils.glsl"
#include "colorspace_utils.glsl"

// default set of attributes
#include "attributes.glsl"



layout(set = DESCRIPTOR_OBJECT_SET, binding = 0) uniform ObjectCB {
   vec4 fogEyePlane;
   vec4 fogPlane;
   vec3 entityOrigin;
   float isAlphaBlending;
   mat4 mvp;
   mat4 mv;
   vec4 rgbGenFuncArgs;
   vec4 alphaGenFuncArgs;
   vec4 colorConst;
   vec4 lightAmbient;
   vec4 lightDiffuse;
   vec3 lightDir;
   vec3 entityDist;
} obj;

#ifdef APPLY_INSTANCED_TRANSFORMS
	layout(set = DESCRIPTOR_OBJECT_SET, binding = 1) uniform vec4 instancePoints[MAX_UNIFORM_INSTANCES * 2]; 
	#define a_InstanceQuat instancePoints[gl_InstanceID*2]
	#define a_InstancePosAndScale instancePoints[gl_InstanceID*2+1]
#endif

layout(set = DESCRIPTOR_FRAME_SET, binding = 0) uniform FrameCB {
  vec3 viewOrigin;
  float shaderTime;
  vec2 blendMix;
  float zNear;
  float zFar;
  float fogScale;
  float eyeDist;
  vec3 fogColor;
  float mirrorSide;
  mat3 viewAxis;
} frame; 
