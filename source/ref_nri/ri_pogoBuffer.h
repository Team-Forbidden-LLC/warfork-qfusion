#ifndef RI_POGO_BUFFER_H
#define RI_POGO_BUFFER_H

#include "ri_types.h"

struct RI_PogoBuffer {
	struct RIDescriptor_s pogoAttachment[2];
	uint16_t attachmentIndex: 15;
	uint16_t initial: 1;
};


void RI_PogoBufferToggle( struct RIDevice_s *device, struct RI_PogoBuffer *pogo, struct RICmdHandle_s *handle );

static inline struct RIDescriptor_s *RI_PogoBufferAttachment( struct RI_PogoBuffer *pogo )
{
	return pogo->pogoAttachment + pogo->attachmentIndex;
}
static inline struct RIDescriptor_s* RI_PogoBufferShaderResource(struct RI_PogoBuffer* pogo) {
  return pogo->pogoAttachment + ((pogo->attachmentIndex + 1) % 2);
}

#endif

