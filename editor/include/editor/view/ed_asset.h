#ifndef ED_ASSET_VIEW_H
#define ED_ASSET_VIEW_H

extern char const *g_asset_type_names[EG_ASSET_TYPE_COUNT];

class ed_asset_t : public ed_view_t {
public:
  ed_asset_t(char const *name);
  virtual ~ed_asset_t();

public:
  virtual void refresh() override;
  virtual void draw() override;

private:
  void draw_swapchain(uint64_t asset_index);
  void draw_renderer(uint64_t asset_index);
  void draw_pipeline(uint64_t asset_index);
  void draw_model(uint64_t asset_index);
  void draw_scene(uint64_t asset_index);
  void draw_graph(uint64_t asset_index);

private:
  void create_assets();
  void destroy_assets();

private:
  int64_t m_selected_asset = -1;

  eg_vector_t *m_assets = 0;

  eg_asset_type_t m_asset_type = EG_ASSET_TYPE_PIPELINE;
};

#endif // ED_ASSET_VIEW_H
