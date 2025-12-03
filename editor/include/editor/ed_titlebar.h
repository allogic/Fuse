#ifndef ED_TITLEBAR_H
#define ED_TITLEBAR_H

class ed_titlebar_t {
public:
  ed_titlebar_t();
  virtual ~ed_titlebar_t();

public:
  virtual void refresh();
  virtual void draw();

private:
  void reset_drag_state();

  void draw_icon();
  void draw_main_menu();
  void draw_window_controls();
  void draw_scene_controls();
};

#endif // ED_TITLEBAR_H
