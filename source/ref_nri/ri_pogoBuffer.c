#include "ri_pogoBuffer.h"

//void RI_PogoInsertInitialBarrier(struct RIDevice_s* device, struct RI_PogoBuffer* pogo, VkCommandBuffer cmd) {
//	VkImageMemoryBarrier2 imageBarriers[2] = {};
//	imageBarriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
//	imageBarriers[0].srcStageMask = VK_PIPELINE_STAGE_2_NONE;
//	imageBarriers[0].srcAccessMask = 0;
//	imageBarriers[0].dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
//	imageBarriers[0].dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
//	imageBarriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//	imageBarriers[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//	imageBarriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	imageBarriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	imageBarriers[0].image = pogo->pogoAttachment[pogo->attachmentIndex].vk.image.handle;
//	imageBarriers[0].subresourceRange = (VkImageSubresourceRange){
//		VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS,
//	};
//
//	pogo->attachmentIndex = ((pogo->attachmentIndex + 1) % 2);
//
//	imageBarriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
//	imageBarriers[1].srcStageMask = VK_PIPELINE_STAGE_2_NONE;
//	imageBarriers[1].srcAccessMask = 0;
//	imageBarriers[1].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
//	imageBarriers[1].dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
//	imageBarriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//	imageBarriers[1].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//	imageBarriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	imageBarriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//	imageBarriers[1].image = pogo->pogoAttachment[pogo->attachmentIndex].vk.image.handle;
//	imageBarriers[1].subresourceRange = (VkImageSubresourceRange){
//		VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS,
//	};
//	VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
//	dependencyInfo.imageMemoryBarrierCount = 2;
//	dependencyInfo.pImageMemoryBarriers = imageBarriers;
//	vkCmdPipelineBarrier2( cmd, &dependencyInfo );
//}


void RI_PogoBufferToggle(struct RIDevice_s *device, struct RI_PogoBuffer *pogo, struct RICmdHandle_s *handle ) {
	VkImageMemoryBarrier2 imageBarriers[2] = {};
	imageBarriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	imageBarriers[0].srcStageMask = pogo->initial ? VK_PIPELINE_STAGE_2_NONE : VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	imageBarriers[0].srcAccessMask = pogo->initial ? 0 :VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	imageBarriers[0].dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
	imageBarriers[0].dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
	imageBarriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageBarriers[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageBarriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarriers[0].image = pogo->pogoAttachment[pogo->attachmentIndex].vk.image.handle;
	imageBarriers[0].subresourceRange = (VkImageSubresourceRange){
		VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS,
	};

	pogo->attachmentIndex = ((pogo->attachmentIndex + 1) % 2);

	imageBarriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	imageBarriers[1].srcStageMask = pogo->initial ? VK_PIPELINE_STAGE_2_NONE :VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
	imageBarriers[1].srcAccessMask = pogo->initial ? 0 :VK_ACCESS_2_SHADER_READ_BIT;
	imageBarriers[1].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
	imageBarriers[1].dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
	imageBarriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageBarriers[1].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	imageBarriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarriers[1].image = pogo->pogoAttachment[pogo->attachmentIndex].vk.image.handle;
	imageBarriers[1].subresourceRange = (VkImageSubresourceRange){
		VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS,
	};
	VkDependencyInfo dependencyInfo = { VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
	dependencyInfo.imageMemoryBarrierCount = 2;
	dependencyInfo.pImageMemoryBarriers = imageBarriers;
	vkCmdPipelineBarrier2( handle->vk.cmd, &dependencyInfo );
	pogo->initial = 1;
}

