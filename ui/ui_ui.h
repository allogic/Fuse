#ifndef UI_UI_H
#define UI_UI_H

#ifdef BUILD_DEBUG
#  define UI_CREATE(...) ui_create(__VA_ARGS__)
#  define UI_DRAW(...) ui_draw(__VA_ARGS__)
#  define UI_DESTROY(...) ui_destroy(__VA_ARGS__)

#  define UI_FORWARD_MESSAGE(...) ui_forward_message(__VA_ARGS__)
#else
#  define UI_CREATE(...)
#  define UI_DRAW(...)
#  define UI_DESTROY(...)

#  define UI_FORWARD_MESSAGE(...)
#endif // BUILD_DEBUG

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void ui_create(void);
void ui_draw();
void ui_destroy(void);

void ui_forward_message(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // UI_UI_H
