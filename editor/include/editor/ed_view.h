#ifndef ED_VIEW_H
#define ED_VIEW_H

class ed_view_t {
public:
  ed_view_t(char const *name);
  virtual ~ed_view_t();

public:
  inline auto name() const { return m_name; }

  inline auto is_dirty() const { return m_is_dirty; }
  inline auto is_open() const { return m_is_open; }
  inline auto is_docked() const { return m_is_docked; }

public:
  inline void set_dirty(uint8_t is_dirty) { m_is_dirty = is_dirty; }
  inline void set_open(uint8_t is_open) { m_is_dirty = is_open; }
  inline void set_docked(uint8_t is_docked) { m_is_dirty = is_docked; }

public:
  virtual void refresh() = 0;
  virtual void draw() = 0;

protected:
  char const *m_name = 0;

  uint8_t m_is_dirty = 0;
  uint8_t m_is_open = 1;
  uint8_t m_is_docked = 0;
};

#endif // ED_VIEW_H
