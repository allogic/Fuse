#ifndef EG_FWD_H
#define EG_FWD_H

typedef enum eg_key_state_t {
  EG_KEY_STATE_UP,
  EG_KEY_STATE_PRESSED,
  EG_KEY_STATE_DOWN,
  EG_KEY_STATE_RELEASED,
} eg_key_state_t;

typedef enum eg_keyboard_key_t {
  EG_KEYBOARD_KEY_BACK = 0x08,
  EG_KEYBOARD_KEY_TAB = 0x09,
  EG_KEYBOARD_KEY_RETURN = 0x0D,
  EG_KEYBOARD_KEY_SHIFT = 0x10,
  EG_KEYBOARD_KEY_CONTROL = 0x11,
  EG_KEYBOARD_KEY_MENU = 0x12,
  EG_KEYBOARD_KEY_PAUSE = 0x13,
  EG_KEYBOARD_KEY_CAPITAL = 0x14,
  EG_KEYBOARD_KEY_ESCAPE = 0x1B,
  EG_KEYBOARD_KEY_SPACE = 0x20,
  EG_KEYBOARD_KEY_PRIOR = 0x21,
  EG_KEYBOARD_KEY_NEXT = 0x22,
  EG_KEYBOARD_KEY_END = 0x23,
  EG_KEYBOARD_KEY_HOME = 0x24,
  EG_KEYBOARD_KEY_LEFT = 0x25,
  EG_KEYBOARD_KEY_UP = 0x26,
  EG_KEYBOARD_KEY_RIGHT = 0x27,
  EG_KEYBOARD_KEY_DOWN = 0x28,
  EG_KEYBOARD_KEY_SELECT = 0x29,
  EG_KEYBOARD_KEY_PRINT = 0x2A,
  EG_KEYBOARD_KEY_EXE = 0x2B,
  EG_KEYBOARD_KEY_SNAPSHOT = 0x2C,
  EG_KEYBOARD_KEY_INSERT = 0x2D,
  EG_KEYBOARD_KEY_DELETE = 0x2E,
  EG_KEYBOARD_KEY_HELP = 0x2F,
  EG_KEYBOARD_KEY_0 = '0',
  EG_KEYBOARD_KEY_1 = '1',
  EG_KEYBOARD_KEY_2 = '2',
  EG_KEYBOARD_KEY_3 = '3',
  EG_KEYBOARD_KEY_4 = '4',
  EG_KEYBOARD_KEY_5 = '5',
  EG_KEYBOARD_KEY_6 = '6',
  EG_KEYBOARD_KEY_7 = '7',
  EG_KEYBOARD_KEY_8 = '8',
  EG_KEYBOARD_KEY_9 = '9',
  EG_KEYBOARD_KEY_A = 'A',
  EG_KEYBOARD_KEY_B = 'B',
  EG_KEYBOARD_KEY_C = 'C',
  EG_KEYBOARD_KEY_D = 'D',
  EG_KEYBOARD_KEY_E = 'E',
  EG_KEYBOARD_KEY_F = 'F',
  EG_KEYBOARD_KEY_G = 'G',
  EG_KEYBOARD_KEY_H = 'H',
  EG_KEYBOARD_KEY_I = 'I',
  EG_KEYBOARD_KEY_J = 'J',
  EG_KEYBOARD_KEY_K = 'K',
  EG_KEYBOARD_KEY_L = 'L',
  EG_KEYBOARD_KEY_M = 'M',
  EG_KEYBOARD_KEY_N = 'N',
  EG_KEYBOARD_KEY_O = 'O',
  EG_KEYBOARD_KEY_P = 'P',
  EG_KEYBOARD_KEY_Q = 'Q',
  EG_KEYBOARD_KEY_R = 'R',
  EG_KEYBOARD_KEY_S = 'S',
  EG_KEYBOARD_KEY_T = 'T',
  EG_KEYBOARD_KEY_U = 'U',
  EG_KEYBOARD_KEY_V = 'V',
  EG_KEYBOARD_KEY_W = 'W',
  EG_KEYBOARD_KEY_X = 'X',
  EG_KEYBOARD_KEY_Y = 'Y',
  EG_KEYBOARD_KEY_Z = 'Z',
  EG_KEYBOARD_KEY_NUMPAD0 = 0x60,
  EG_KEYBOARD_KEY_NUMPAD1 = 0x61,
  EG_KEYBOARD_KEY_NUMPAD2 = 0x62,
  EG_KEYBOARD_KEY_NUMPAD3 = 0x63,
  EG_KEYBOARD_KEY_NUMPAD4 = 0x64,
  EG_KEYBOARD_KEY_NUMPAD5 = 0x65,
  EG_KEYBOARD_KEY_NUMPAD6 = 0x66,
  EG_KEYBOARD_KEY_NUMPAD7 = 0x67,
  EG_KEYBOARD_KEY_NUMPAD8 = 0x68,
  EG_KEYBOARD_KEY_NUMPAD9 = 0x69,
  EG_KEYBOARD_KEY_MULTIPLY = 0x6A,
  EG_KEYBOARD_KEY_ADD = 0x6B,
  EG_KEYBOARD_KEY_SEPARATOR = 0x6C,
  EG_KEYBOARD_KEY_SUBTRACT = 0x6D,
  EG_KEYBOARD_KEY_DECIMAL = 0x6E,
  EG_KEYBOARD_KEY_DIVIDE = 0x6F,
  EG_KEYBOARD_KEY_F1 = 0x70,
  EG_KEYBOARD_KEY_F2 = 0x71,
  EG_KEYBOARD_KEY_F3 = 0x72,
  EG_KEYBOARD_KEY_F4 = 0x73,
  EG_KEYBOARD_KEY_F5 = 0x74,
  EG_KEYBOARD_KEY_F6 = 0x75,
  EG_KEYBOARD_KEY_F7 = 0x76,
  EG_KEYBOARD_KEY_F8 = 0x77,
  EG_KEYBOARD_KEY_F9 = 0x78,
  EG_KEYBOARD_KEY_F10 = 0x79,
  EG_KEYBOARD_KEY_F11 = 0x7A,
  EG_KEYBOARD_KEY_F12 = 0x7B,
  EG_KEYBOARD_KEY_F13 = 0x7C,
  EG_KEYBOARD_KEY_F14 = 0x7D,
  EG_KEYBOARD_KEY_F15 = 0x7E,
  EG_KEYBOARD_KEY_F16 = 0x7F,
  EG_KEYBOARD_KEY_F17 = 0x80,
  EG_KEYBOARD_KEY_F18 = 0x81,
  EG_KEYBOARD_KEY_F19 = 0x82,
  EG_KEYBOARD_KEY_F20 = 0x83,
  EG_KEYBOARD_KEY_F21 = 0x84,
  EG_KEYBOARD_KEY_F22 = 0x85,
  EG_KEYBOARD_KEY_F23 = 0x86,
  EG_KEYBOARD_KEY_F24 = 0x87,
  EG_KEYBOARD_KEY_LEFT_SHIFT = 0xA0,
  EG_KEYBOARD_KEY_RIGHT_SHIFT = 0xA1,
  EG_KEYBOARD_KEY_LEFT_CONTROL = 0xA2,
  EG_KEYBOARD_KEY_RIGHT_CONTROL = 0xA3,
  EG_KEYBOARD_KEY_LEFT_MENU = 0xA4,
  EG_KEYBOARD_KEY_RIGHT_MENU = 0xA5,
} eg_keyboard_key_t;

typedef enum eg_mouse_key_t {
  EG_MOUSE_KEY_LEFT,
  EG_MOUSE_KEY_MIDDLE,
  EG_MOUSE_KEY_RIGHT,
} eg_mouse_key_t;

typedef void (*eg_editor_create_proc_t)(struct eg_context_t *context);
typedef void (*eg_editor_refresh_proc_t)(struct eg_context_t *context);
typedef void (*eg_editor_draw_proc_t)(struct eg_context_t *context);
typedef void (*eg_editor_destroy_proc_t)(struct eg_context_t *context);
typedef LRESULT (*eg_editor_message_proc_t)(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

typedef enum eg_profiler_sample_lane_t {
  EG_PROFILER_SAMPLE_LANE_CONTEXT,
  EG_PROFILER_SAMPLE_LANE_SCENE,
  EG_PROFILER_SAMPLE_LANE_RENDERER,
  EG_PROFILER_SAMPLE_LANE_COUNT,
} eg_profiler_sample_lane_t;

typedef struct eg_cpu_frame_sample_t {
  float time;
  float delta;
} eg_cpu_frame_sample_t;

typedef struct eg_world_settings_t {
  lb_scene_asset_id_t default_scene_asset_id;
  lb_swapchain_asset_id_t default_swapchain_asset_id;
  lb_renderer_asset_id_t default_renderer_asset_id;
} eg_world_settings_t;

typedef struct eg_context_t eg_context_t;
typedef struct eg_swapchain_t eg_swapchain_t;
typedef struct eg_viewport_t eg_viewport_t;
typedef struct eg_renderer_t eg_renderer_t;
typedef struct eg_graphic_pipeline_t eg_graphic_pipeline_t;
typedef struct eg_compute_pipeline_t eg_compute_pipeline_t;
typedef struct eg_buffer_t eg_buffer_t;
typedef struct eg_image_t eg_image_t;
typedef struct eg_scene_t eg_scene_t;
typedef struct eg_terrain_t eg_terrain_t;

typedef struct eg_time_info_t {
  float time;
  float delta_time;
} eg_time_info_t;
typedef struct eg_screen_info_t {
  lb_vector2_t resolution;
} eg_screen_info_t;
typedef struct eg_camera_info_t {
  lb_vector3_t position;
  int32_t reserved;
  lb_matrix4_t view;
  lb_matrix4_t projection;
  lb_matrix4_t view_projection;
  lb_matrix4_t view_projection_inv;
} eg_camera_info_t;

typedef struct eg_debug_vertex_t {
  lb_vector3_t position;
  lb_vector4_t color;
} eg_debug_vertex_t;
typedef struct eg_terrain_vertex_t {
  lb_vector3_t position;
  lb_vector3_t normal;
  lb_vector4_t color;
} eg_terrain_vertex_t;
typedef struct eg_brdf_vertex_t {
  lb_vector3_t position;
  lb_vector3_t normal;
  lb_vector3_t tangent;
  lb_vector2_t texcoord_channel_0;
  lb_vector4_t color_channel_0;
} eg_brdf_vertex_t;

typedef enum eg_renderer_pipeline_link_type_t {
  EG_RENDERER_PIPELINE_LINK_TYPE_DEBUG,
  EG_RENDERER_PIPELINE_LINK_TYPE_PBR,
} eg_renderer_pipeline_link_type_t;

#endif // EG_FWD_H
