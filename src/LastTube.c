#include <pebble.h>

static Window *window;
static TextLayer *station_layer;
static TextLayer *line_layer;
static TextLayer *destination_layer;
static TextLayer *time_layer;
enum {
  TUBE_KEY_STATION = 0,
  TUBE_KEY_LINE = 1,
  TUBE_KEY_DESTINATION = 2,
  TUBE_KEY_TIME = 3,
};
static char station[17];
static char line[16];
static char destination[15];
static char time_string[5];

//This tells us which entry in the list of results we want.
int tube = 0;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //not needed for now
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  tube = tube - 2;
  if ( tube < 0 )
    tube = 0;
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(4, tube);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  tube = tube + 1;
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletInteger(4, tube);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
  tube = tube + 1;
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }

 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
 }

 void in_received_handler(DictionaryIterator *received, void *context) {
   Tuple *station_tuple = dict_find(received, TUBE_KEY_STATION);
   Tuple *line_tuple = dict_find(received, TUBE_KEY_LINE);
   Tuple *destination_tuple = dict_find(received, TUBE_KEY_DESTINATION);
   Tuple *time_tuple = dict_find(received, TUBE_KEY_TIME);
   if (station_tuple) {
       strncpy(station, station_tuple->value->cstring, 17);
       text_layer_set_text(station_layer, station);
   }
   if (line_tuple) {
       strncpy(line, line_tuple->value->cstring, 16);
       text_layer_set_text(line_layer, line);
   }
   if (destination_tuple) {
       strncpy(destination, destination_tuple->value->cstring, 15);
       text_layer_set_text(destination_layer, destination);
   }
   if (time_tuple) {
       strncpy(time_string, time_tuple->value->cstring, 5);
       text_layer_set_text(time_layer, time_string);
   }
 }

 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  station_layer = text_layer_create((GRect) { .origin = { 0, 5 }, .size =  bounds.size });
  text_layer_set_font(station_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(station_layer, "LOADING...");
  text_layer_set_text_alignment(station_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(station_layer));
  line_layer = text_layer_create((GRect) { .origin = { 0, 38 }, .size =  bounds.size });
  text_layer_set_font(line_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(line_layer, "");
  text_layer_set_text_alignment(line_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(line_layer));
  destination_layer = text_layer_create((GRect) { .origin = { 0, 73 }, .size =  bounds.size });
  text_layer_set_font(destination_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(destination_layer, "");
  text_layer_set_text_alignment(destination_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(destination_layer));
  time_layer = text_layer_create((GRect) { .origin = { 0, 105 }, .size =  bounds.size });
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text(time_layer, "");
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(station_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  const uint32_t inbound_size = 256;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);

  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
