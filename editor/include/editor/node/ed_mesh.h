#ifndef ED_MESH_NODE_H
#define ED_MESH_NODE_H

ed_mesh_node_t *ed_mesh_node_create(ed_canvas_t *canvas);
void ed_mesh_node_refresh(ed_mesh_node_t *node);
void ed_mesh_node_draw(ed_mesh_node_t *node);
void ed_mesh_node_destroy(ed_mesh_node_t *node);

#endif // ED_MESH_NODE_H
