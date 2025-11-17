#ifndef ED_NOISE_NODE_H
#define ED_NOISE_NODE_H

ed_noise_node_t *ed_noise_node_create(eg_context_t *context, uint32_t node_id);
void ed_noise_node_refresh(ed_noise_node_t *node);
void ed_noise_node_draw(ed_noise_node_t *node);
void ed_noise_node_destroy(ed_noise_node_t *node);

#endif // ED_NOISE_NODE_H
