#ifndef R_FRAME_CMD_BUFFER_H
#define R_FRAME_CMD_BUFFER_H

#include "r_nri.h"
#include "r_vattribs.h"
#include "r_block_buffer_pool.h"


typedef struct mesh_vbo_s mesh_vbo_t;

struct frame_cmd_vertex_input_s {
	NriBuffer *buffer;
	uint64_t offset;
};

// the serialized state of the pipeline
struct frame_cmd_state_s {
	uint32_t viewport[4];
	struct {
		uint16_t x;
		uint16_t y;
		uint16_t w;
		uint16_t h;
	} scissor;

  
  NriBuffer* vertexBuffers[MAX_VERTEX_BINDINGS];
  uint64_t offsets[MAX_VERTEX_BINDINGS];
	uint32_t dirtyVertexBuffers;

	// binding
	struct NriDescriptor *bindings[DESCRIPTOR_SET_MAX][DESCRIPTOR_MAX_BINDINGS];
};

struct frame_backbuffer_s {
	NriDescriptor *colorAttachment;
	NriTexture *texture;
	NriAccessLayoutStage currentLayout;
};

// hack to store some additional managed state 
struct frame_additional_data {
	struct {
		unsigned int firstVert;
		unsigned int numVerts;
		unsigned int firstElem;
		unsigned int numElems;
		unsigned int numInstances;
	} drawElements;
	struct {
		unsigned int firstVert;
		unsigned int numVerts;
		unsigned int firstElem;
		unsigned int numElems;
		unsigned int numInstances;
	} drawShadowElements;
};

struct frame_cmd_buffer_s {
	uint8_t frameCount; // this value is bound by NUMBER_FRAMES_FLIGHT
	struct block_buffer_pool_s uboBlockBuffer; 
	struct frame_cmd_state_s cmdState;
	struct pipeline_layout_config_s layoutDef;
	struct frame_backbuffer_s backBuffer;
	NriCommandAllocator *allocator;
	NriCommandBuffer *cmd;
	struct frame_additional_data additional;
};

// resets the pipeline definition
void FR_FrameResetDefaultPipelineLayoutDef(struct pipeline_layout_config_s* def);

// cmd buffer 
void FR_CmdSetOpqaueState( struct frame_cmd_buffer_s *cmd );
void FR_CmdSetDefaultState( struct frame_cmd_buffer_s *cmd );
void FR_CmdSetTexture( struct frame_cmd_buffer_s *cmd, uint32_t set, uint32_t binding, NriDescriptor *texture );
void FR_CmdSetVertexInput( struct frame_cmd_buffer_s *cmd, uint32_t slot, NriBuffer *buffer, uint64_t offset );
void FR_CmdSetScissor(struct frame_cmd_buffer_s* cmd, int x, int y, int w, int h );

void FR_CmdDrawElements( struct frame_cmd_buffer_s *cmd, uint32_t indexNum, uint32_t instanceNum, uint32_t baseIndex, uint32_t baseVertex, uint32_t baseInstance );

void FR_HashDescriptorSet(struct pipeline_layout_s* layout);
#endif
