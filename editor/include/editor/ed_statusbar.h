#ifndef ED_STATUSBAR_H
#define ED_STATUSBAR_H

class ed_statusbar_t {
public:
  ed_statusbar_t();
  virtual ~ed_statusbar_t();

public:
  virtual void refresh();
  virtual void draw();
};

#endif // ED_STATUSBAR_H
