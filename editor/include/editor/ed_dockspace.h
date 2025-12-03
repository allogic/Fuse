#ifndef ED_DOCKSPACE_H
#define ED_DOCKSPACE_H

class ed_dockspace_t {
public:
  template <typename... V>
  ed_dockspace_t(V &&...views);
  virtual ~ed_dockspace_t();

public:
  virtual void refresh();
  virtual void draw();

private:
  void begin_child(ed_view_t *view, float border_size);
  void end_child(ed_view_t *view);

private:
  std::vector<ed_view_t *> m_views = {};

  uint8_t m_is_dirty = 0;
};

template <typename... V>
ed_dockspace_t::ed_dockspace_t(V &&...views) : m_views{views...} {
}

#endif // ED_DOCKSPACE_H
