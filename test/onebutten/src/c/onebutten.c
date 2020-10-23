#include <pebble.h>


#define LOCKITRON_LOCK_UUID "95c22a11-4c9e-4420-adf0-11f1b36575f2"
#define LOCKITRON_ACCESS_TOKEN "99e75a775fe737bb716caf88f16140bb623d283c3561c833480f083433568b"

typedef enum {
  LOCKITRON_UNLOCKED,
  LOCKITRON_LOCKED,
  LOCKITRON_UNKNOWN
} LockitronLockState;

static LockitronLockState s_lockitron_state;

////////////////////INBOX_HANDLER///////////////////
static void prv_lockitron_toggle_state() {
  DictionaryIterator *out;
  AppMessageResult result = app_message_outbox_begin(&out);
  dict_write_cstring(out, MESSAGE_KEY_LOCK_UUID, LOCKITRON_LOCK_UUID);
  dict_write_cstring(out, MESSAGE_KEY_ACCESS_TOKEN, LOCKITRON_ACCESS_TOKEN);
  result = app_message_outbox_send();
}
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_APP_READY);
  if(ready_tuple) {
    //PebbleKit JS is ready, toggle the Lockitron!
    prv_lockitron_toggle_state();
    return;
  }
  //.....

  Tuple *lock_state_tuple = dict_find(iter, MESSAGE_KEY_LOCK_STATE);
  if(lock_state_tuple) {
    s_lockitron_state = (LockitronLockState)lock_state_tuple->value->int32;
    app_exit_reason_set(APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY);
    window_stack_remove(s_window, false);
  }
}
static void prv_update_app_glance(AppGlanceReloadSession *session, size_t limit, void *context) {
  if(limit < 1) return;
  LockitronLockState *lockitron_state = context;
  char *str = prv_lockitron_status_message(lockitron_state);
  APP_LOG(APP_LOG_LEVEL_INFO, "STATE: %s", str);
  const AppGlanceSlice entry = (AppGlanceSlice) {
    .layout = {
      .template_string = str    
    },
    .expiration_time = time(NULL)+3600
  };
  const AppGlacneResult result = app_glance_add_slice(session, entry);
  if(result != APP_GLANCE_RESULT_SUCCESS) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Appglance Error: %d", result);
  }
}
//////////////////////////////////////////////////LOCKITRON SETTING////////////////////

static Window *s_window;
static TextLayer *s_text_layer;

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Select");
}

static void prv_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Up");
}

static void prv_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_text_layer, "Down");
}

static void prv_click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, prv_up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, prv_down_click_handler);
}

static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(s_text_layer, "Press a button");
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

static void prv_init(void) {

  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(256, 256);


  s_window = window_create();
  window_set_click_config_provider(s_window, prv_click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
}

static void prv_deinit(void) {
  window_destroy(s_window);

  app_glance_reload(prv_update_app_glance, &s_lockitron_state);
}



int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
