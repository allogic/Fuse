#ifndef EG_RENDERER3_RENDERPASS_H
#define EG_RENDERER3_RENDERPASS_H

VkRenderPass eg_renderpass_main(void);
VkRenderPass eg_renderpass_gbuffer(void);

void eg_renderpass_create_main(void);
void eg_renderpass_create_gbuffer(void);

void eg_renderpass_destroy_main(void);
void eg_renderpass_destroy_gbuffer(void);

#endif // EG_RENDERER3_RENDERPASS_H
