#ifndef EG_RENDERER3_RENDERPASS_H
#define EG_RENDERER3_RENDERPASS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

VkRenderPass eg_renderpass_main(void);
VkRenderPass eg_renderpass_gbuffer(void);

void eg_renderpass_create_main(void);
void eg_renderpass_create_gbuffer(void);

void eg_renderpass_destroy_main(void);
void eg_renderpass_destroy_gbuffer(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // EG_RENDERER3_RENDERPASS_H
