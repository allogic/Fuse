#ifndef ED_FWD_H
#define ED_FWD_H

typedef enum asset_type_t {
  ASSET_TYPE_NONE = 0,
  ASSET_TYPE_SWAPCHAIN,
  ASSET_TYPE_RENDERER,
  ASSET_TYPE_PIPELINE,
  ASSET_TYPE_MODEL,
} asset_type_t;

typedef enum dockspace_type_t {
  DOCKSPACE_TYPE_NONE = 0,
  DOCKSPACE_TYPE_MODEL,
  DOCKSPACE_TYPE_SCENE,
} dockspace_type_t;

typedef enum gbuffer_attachment_type_t {
  GBUFFER_ATTACHMENT_TYPE_COLOR,
  GBUFFER_ATTACHMENT_TYPE_DEPTH,
} gbuffer_attachment_type_t;

#endif // ED_FWD_H
