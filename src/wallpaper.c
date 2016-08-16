 /*
Copyright (C) 2016 Mark Reed

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "pebble.h"
#include <ctype.h>	  


enum {
  BLUETOOTHVIBE_KEY = 0x0,
  HOURLYVIBE_KEY = 0x1,
  BACKGROUND_KEY = 0x2
};


static AppSync sync;
static uint8_t sync_buffer[128];

static int bluetoothvibe;
static int hourlyvibe;

static int background;
static int hourlyvibe;

static bool appStarted = false;

Window *window;
static Layer *window_layer;

GBitmap *background_image = NULL;
static BitmapLayer *background_layer;

TextLayer *layer_date_text;
TextLayer *layer_time_text;
TextLayer *layer_secs_text;

int cur_day = -1;

int charge_percent = 0;

static int s_random = 13;
static int temp_random;

TextLayer *battery_text_layer;

BitmapLayer *layer_conn_img;
GBitmap *img_bt_connect;
GBitmap *img_bt_disconnect;

static GFont time_font;
static GFont date_font;
static GFont battery_font;





void uppercase ( char *sPtr ) {
    while ( *sPtr != '\0' )
    {
    *sPtr = toupper ( ( unsigned char ) *sPtr );
    ++sPtr;
    }
}

	
void wallpaper_choice() {

	switch(background) {
		
		case 0: // blank + time


		

	    break;
		
		case 1: //grid + time
		
	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
    }

			background_image = gbitmap_create_with_resource(RESOURCE_ID_LOADING);		
					
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
    }
	
	    break;

		case 2:  // random 

		if(s_random == 13){
			s_random = 0;
		} else {

			temp_random = rand() % 13;

			while(temp_random == s_random){
			    temp_random = rand() % 13;
		    }

		    s_random = temp_random;

	    if (background_image) {
		gbitmap_destroy(background_image);

		background_image = NULL;
			
    }

		   if(s_random == 0){
			   background_image = gbitmap_create_with_resource(RESOURCE_ID_COLOURS);
         } else if(s_random == 1){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_EARTH);
         } else if(s_random == 2){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_WAVE1);
         } else if(s_random == 3){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_RAVE);
         } else if(s_random == 4){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_MONARCH4);
         } else if(s_random == 5){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_NIGHTMTN);
         } else if(s_random == 6){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_LIGHTHOUSE1);
		 } else if(s_random == 7){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_MONARCH5);
         } else if(s_random == 8){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_GREENHILLS);
         } else if(s_random == 9){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_SKATER);
         } else if(s_random == 10){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_MONARCH3);
         } else if(s_random == 11){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_PALM1);
         } else if(s_random == 12){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_BERRY);
         } else if(s_random == 13){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_EARTH2);
         } else if(s_random == 14){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_MUSIC2);
         } else if(s_random == 15){
 				background_image = gbitmap_create_with_resource(RESOURCE_ID_TREESUNSET);
         }// else if(s_random == 16){
 			//	background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG);
		// } else if(s_random == 17){
 		//		background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG);
         //}
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	}				
	    break;
		
		case 3:  // EARTH2

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_EARTH2);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
		case 4:  // MUSIC2

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_MUSIC2);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;		
		
		case 5:  // BERRY

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_BERRY);
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
		case 6:  // RAVE

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
        }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_RAVE);	
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
		case 7:  // NIGHTMTN

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
 	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_NIGHTMTN);
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
		case 8:  // GREENHILLS

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_GREENHILLS);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
		case 9:  // MONARCH3

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_MONARCH3);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
		case 10:  // MONARCH4

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_MONARCH4);
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
		case 11:  // MONARCH5

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_MONARCH5);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
	
		case 12:  // COLOURS

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_COLOURS);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
		case 13:  // PALM1

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_PALM1);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;
			
	case 14:  // SKATER

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_SKATER);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;

	case 15:  // TREESUNSET

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_TREESUNSET);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;	
		
	case 16:  // EARTH

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_EARTH);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
	case 17:  // LIGHTHOUSE1

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_LIGHTHOUSE1);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
		
	case 18:  // WAVE1

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
			background_image = gbitmap_create_with_resource(RESOURCE_ID_WAVE1);
		 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
			
	    break;
	}
}
	
 
void update_time(struct tm *tick_time) {

	static char time_text[] = "00:00";
    static char date_text[] = "xxx xxx 00xx xx xxxxxxx";

    char *time_format;

    int new_cur_day = tick_time->tm_year*1000 + tick_time->tm_yday;
    if (new_cur_day != cur_day) {
        cur_day = new_cur_day;

      strftime(date_text, sizeof(date_text), "%a, %e %b", tick_time);
	uppercase(date_text);
	  text_layer_set_text(layer_date_text, date_text);
				
  }
	
	if (clock_is_24h_style()) {
        time_format = "%R";
		
    } else {
        time_format = "%l:%M";
    }

    strftime(time_text, sizeof(time_text), time_format, tick_time);

    if (!clock_is_24h_style() && (time_text[0] == '0')) {
        memmove(time_text, &time_text[1], sizeof(time_text) - 1);
    }
	
    text_layer_set_text(layer_time_text, time_text);
}


void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
    update_time(tick_time);
	
  if (units_changed & MINUTE_UNIT) {
wallpaper_choice();
}
}

void force_update(void) {
    time_t now = time(NULL);
    update_time(localtime(&now));
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  switch (key) {

	case BACKGROUND_KEY:
      background = new_tuple->value->uint8;
	  persist_write_bool(BACKGROUND_KEY, background);	  
	  wallpaper_choice(); 
	break; 	  
	  
	case BLUETOOTHVIBE_KEY:
      bluetoothvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(BLUETOOTHVIBE_KEY, bluetoothvibe);
      break;      
	  
    case HOURLYVIBE_KEY:
      hourlyvibe = new_tuple->value->uint8 != 0;
	  persist_write_bool(HOURLYVIBE_KEY, hourlyvibe);	  
      break;	
	 
  }
  // Refresh display

  // Get current time
  time_t now = time( NULL );
  struct tm *tick_time = localtime( &now );

  // Force update to Refresh display
  handle_minute_tick(tick_time, DAY_UNIT + HOUR_UNIT + MINUTE_UNIT );
	
}

void update_battery_state(BatteryChargeState charge_state) {
	static char battery_text[] = "+100";

  if ( charge_state.is_charging ) {
	snprintf(battery_text, sizeof(battery_text), "+%d", charge_state.charge_percent);

  } else {
	snprintf(battery_text, sizeof(battery_text), "%d", charge_state.charge_percent);
  }
	charge_percent = charge_state.charge_percent;   
	text_layer_set_text(battery_text_layer, battery_text);
} 

void handle_bluetooth(bool connected) {
    if (connected) {
        bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
    } else {
        bitmap_layer_set_bitmap(layer_conn_img, img_bt_disconnect);
    }

    if (appStarted && bluetoothvibe) {
      
        vibes_long_pulse();
	}
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);

	// font resources

	time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_REGULAR_28));
    date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_14));	
    battery_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CUSTOM_12));	

	//add background layer
  background_image = gbitmap_create_with_resource( RESOURCE_ID_LOADING );
  background_layer = bitmap_layer_create( layer_get_frame( window_layer ) );
  bitmap_layer_set_bitmap( background_layer, background_image );
  layer_add_child( window_layer, bitmap_layer_get_layer( background_layer ) );

  img_bt_connect     = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTH);
  img_bt_disconnect  = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_BLUETOOTH);	

#ifdef PBL_PLATFORM_CHALK
  layer_conn_img  = bitmap_layer_create(GRect(86, 119, 7, 11));
#else
  layer_conn_img  = bitmap_layer_create(GRect(128, 153, 7, 11));
#endif
  bitmap_layer_set_bitmap(layer_conn_img, img_bt_connect);
  layer_add_child(window_layer, bitmap_layer_get_layer(layer_conn_img)); 
		
#ifdef PBL_PLATFORM_CHALK
    layer_time_text = text_layer_create(GRect(-1, 122, 180, 70));
    layer_date_text = text_layer_create(GRect(0, 149, 180, 20));
#else
	layer_time_text = text_layer_create(GRect(-1, 122, 144, 70));
    layer_date_text = text_layer_create(GRect(0, 148, 144, 20));
#endif
	
    text_layer_set_background_color(layer_date_text, GColorClear);
    text_layer_set_font(layer_date_text, date_font);
    text_layer_set_background_color(layer_time_text, GColorClear);
    text_layer_set_font(layer_time_text, time_font);

    text_layer_set_text_alignment(layer_date_text, GTextAlignmentCenter);
    text_layer_set_text_alignment(layer_time_text, GTextAlignmentCenter);

	// set-up layer colours
    text_layer_set_text_color(layer_time_text, GColorBlack);
    text_layer_set_text_color(layer_date_text, GColorBlack);

	// composing layers
    layer_add_child(window_layer, text_layer_get_layer(layer_date_text));
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text));
	

	
	  // set up battery text layer
	
#ifdef PBL_PLATFORM_CHALK
  battery_text_layer = text_layer_create(GRect(0, 163, 180, 20));	
  text_layer_set_background_color(battery_text_layer, GColorClear);
  text_layer_set_text_color(battery_text_layer, GColorBlack);
  text_layer_set_font(battery_text_layer, battery_font);
  text_layer_set_text_alignment(battery_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));		
#else
  battery_text_layer = text_layer_create(GRect(4, 150, 50, 20));	
  text_layer_set_background_color(battery_text_layer, GColorClear);
  text_layer_set_text_color(battery_text_layer, GColorBlack);
  text_layer_set_font(battery_text_layer, battery_font);
  text_layer_set_text_alignment(battery_text_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));	
#endif
	
	
  // draw first frame
  force_update();
	
}

static void window_unload(Window *window) {
	
  text_layer_destroy(layer_time_text);
  text_layer_destroy(layer_date_text);
  text_layer_destroy(battery_text_layer);
  bitmap_layer_destroy(background_layer);
  bitmap_layer_destroy(layer_conn_img);
	
  gbitmap_destroy(background_image);
  gbitmap_destroy(img_bt_connect);
  gbitmap_destroy(img_bt_disconnect);
	
  // Destroy font objects
  fonts_unload_custom_font( time_font );
  fonts_unload_custom_font( date_font );
  fonts_unload_custom_font( battery_font );

	
  layer_remove_from_parent(window_layer);
  layer_destroy(window_layer);
	
}

void main_window_push() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}

static void init() {
	main_window_push();

  const int inbound_size = 128;
  const int outbound_size = 128;
  app_message_open(inbound_size, outbound_size);  
	

  Tuplet initial_values[] = {
    TupletInteger(BLUETOOTHVIBE_KEY, persist_read_bool(BLUETOOTHVIBE_KEY)),
    TupletInteger(HOURLYVIBE_KEY, persist_read_bool(HOURLYVIBE_KEY)),
    TupletInteger(BACKGROUND_KEY, persist_read_bool(BACKGROUND_KEY)),
  };
  
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, NULL, NULL);
   
  appStarted = true;
  
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  battery_state_service_subscribe(&update_battery_state);
  bluetooth_connection_service_subscribe(&handle_bluetooth);

// update the battery on launch
    update_battery_state(battery_state_service_peek());
		handle_bluetooth(bluetooth_connection_service_peek());

}

void handle_deinit(void) {
	
  app_sync_deinit(&sync);

  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
	
  window_destroy(window);

}

int main(void) {
    init();
    app_event_loop();
    handle_deinit();
}
