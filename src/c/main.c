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


#include <pebble.h>
#include "main.h"

static BitmapLayer *mouth_layer;
static BitmapLayer *face_layer;

static GBitmap *face;
static GBitmap *mouth;

static PropertyAnimation *mouth_animation_beg = NULL;
static PropertyAnimation *mouth_animation_end = NULL;
static PropertyAnimation *face_animation_beg = NULL;
static PropertyAnimation *face_animation_end = NULL;
static PropertyAnimation *nose_animation_beg = NULL;
static PropertyAnimation *nose_animation_end = NULL;
static PropertyAnimation *ear_animation_beg = NULL;
static PropertyAnimation *ear_animation_end = NULL;

static GRect mouth_from_rect;
static GRect mouth_to_rect;
static GRect face_from_rect;
static GRect face_to_rect;
static GRect nose_from_rect;
static GRect nose_to_rect;
static GRect ear_from_rect;
static GRect ear_to_rect;

static bool status_showing = false;

static Layer *window_layer;
static Window *window;

GBitmap *background_image = NULL;
static BitmapLayer *background_layer;

TextLayer *layer_time_text;
TextLayer *layer_time_text2;
TextLayer *layer_time_text3;
TextLayer *layer_time_text4;

static GFont time_font;
static GFont time_font_emery;
static GFont batt_font;

static GBitmap *bluetooth_image;
static BitmapLayer *bluetooth_layer;

static GBitmap *battcircle_image;
static BitmapLayer *battcircle_layer;

static GBitmap *bluetooth_image_on;
static BitmapLayer *bluetooth_layer_on;

TextLayer *battery_text_layer;

int charge_percent = 0;

#define TOTAL_DATE_0DIGITS 1	
static GBitmap *nose[TOTAL_DATE_0DIGITS];
static BitmapLayer *nose_layer[TOTAL_DATE_0DIGITS];
#define TOTAL_DATE_1DIGITS 1	
static GBitmap *ear[TOTAL_DATE_1DIGITS];
static BitmapLayer *ear_layer[TOTAL_DATE_1DIGITS];

const int DATENUM_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DATENUM_0,
  RESOURCE_ID_IMAGE_DATENUM_1,
  RESOURCE_ID_IMAGE_DATENUM_2,
  RESOURCE_ID_IMAGE_DATENUM_3,
  RESOURCE_ID_IMAGE_DATENUM_4,
  RESOURCE_ID_IMAGE_DATENUM_5,
  RESOURCE_ID_IMAGE_DATENUM_6,
  RESOURCE_ID_IMAGE_DATENUM_7,
  RESOURCE_ID_IMAGE_DATENUM_8,
  RESOURCE_ID_IMAGE_DATENUM_9
};

const int DAY_NAME_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_DAY_NAME_SUN,
  RESOURCE_ID_IMAGE_DAY_NAME_MON,
  RESOURCE_ID_IMAGE_DAY_NAME_TUE,
  RESOURCE_ID_IMAGE_DAY_NAME_WED,
  RESOURCE_ID_IMAGE_DAY_NAME_THU,
  RESOURCE_ID_IMAGE_DAY_NAME_FRI,
  RESOURCE_ID_IMAGE_DAY_NAME_SAT
};

const int MONTH_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_JAN,
  RESOURCE_ID_IMAGE_FEB,
  RESOURCE_ID_IMAGE_MAR,
  RESOURCE_ID_IMAGE_APR,
  RESOURCE_ID_IMAGE_MAY,
  RESOURCE_ID_IMAGE_JUN,
  RESOURCE_ID_IMAGE_JUL,
  RESOURCE_ID_IMAGE_AUG,
  RESOURCE_ID_IMAGE_SEP,
  RESOURCE_ID_IMAGE_OCT,
  RESOURCE_ID_IMAGE_NOV,
  RESOURCE_ID_IMAGE_DEC
};

static int s_random = 7;
static int temp_random;

// A struct for our specific settings (see main.h)
ClaySettings settings;

int stringToInt(char *str);

// Initialize the default settings
static void prv_default_settings() {
	
  settings.bluetoothvibe = false;
  settings.hourlyvibe = false;
  settings.random = 1;
  settings.picture = 0;
}

static void set_container_image(GBitmap **bmp_image, BitmapLayer *bmp_layer, const int resource_id, GPoint origin) {
  GBitmap *old_image = *bmp_image;

  *bmp_image = gbitmap_create_with_resource(resource_id);
  GRect bitmap_bounds = gbitmap_get_bounds((*bmp_image));
  GRect frame = GRect(origin.x, origin.y, bitmap_bounds.size.w, bitmap_bounds.size.h);
  bitmap_layer_set_bitmap(bmp_layer, *bmp_image);
  layer_set_frame(bitmap_layer_get_layer(bmp_layer), frame);

  if (old_image != NULL) {
  	gbitmap_destroy(old_image);
  }
}

void update_battery(BatteryChargeState charge_state) {
    static char battery_text[] = "x100";

    if (charge_state.is_charging) {

        snprintf(battery_text, sizeof(battery_text), "+%d", charge_state.charge_percent);
    } else {
        snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);      
    } 
    charge_percent = charge_state.charge_percent;   
    text_layer_set_text(battery_text_layer, battery_text);
} 

static void toggle_bluetooth_icon(bool connected) {
  if(!connected && settings.bluetoothvibe) {
    //vibe!
    vibes_long_pulse();
  }
  layer_set_hidden(bitmap_layer_get_layer(bluetooth_layer), connected);
}

void bluetooth_connection_callback(bool connected) {
  toggle_bluetooth_icon(connected);
}

// date - days

static void update_days(struct tm *tick_time) {
#ifdef PBL_PLATFORM_EMERY
  set_container_image(&mouth, mouth_layer, DAY_NAME_IMAGE_RESOURCE_IDS[tick_time->tm_wday], GPoint(106, 17));
  set_container_image(&nose[0], nose_layer[0], DATENUM_IMAGE_RESOURCE_IDS[tick_time->tm_mday/10], GPoint(106,199));
  set_container_image(&ear[0], ear_layer[0], DATENUM_IMAGE_RESOURCE_IDS[tick_time->tm_mday%10], GPoint(106,209));
#elif PBL_PLATFORM_CHALK
  set_container_image(&mouth, mouth_layer, DAY_NAME_IMAGE_RESOURCE_IDS[tick_time->tm_wday], GPoint(200, 0));
  set_container_image(&nose[0], nose_layer[0], DATENUM_IMAGE_RESOURCE_IDS[tick_time->tm_mday/10], GPoint(190,77));
  set_container_image(&ear[0], ear_layer[0], DATENUM_IMAGE_RESOURCE_IDS[tick_time->tm_mday%10], GPoint(190,87));
#else
  set_container_image(&mouth, mouth_layer, DAY_NAME_IMAGE_RESOURCE_IDS[tick_time->tm_wday], GPoint(160, 35));	
  set_container_image(&nose[0], nose_layer[0], DATENUM_IMAGE_RESOURCE_IDS[tick_time->tm_mday/10], GPoint(160,35));
  set_container_image(&ear[0], ear_layer[0], DATENUM_IMAGE_RESOURCE_IDS[tick_time->tm_mday%10], GPoint(160,46));
#endif
}



//months
	
static void update_months(struct tm *tick_time) {	
#ifdef PBL_PLATFORM_EMERY
  set_container_image(&face, face_layer, MONTH_IMAGE_RESOURCE_IDS[tick_time->tm_mon], GPoint(129, 56));
#elif PBL_PLATFORM_CHALK
  set_container_image(&face, face_layer, MONTH_IMAGE_RESOURCE_IDS[tick_time->tm_mon], GPoint(210, 0));
#else
  set_container_image(&face, face_layer, MONTH_IMAGE_RESOURCE_IDS[tick_time->tm_mon], GPoint(160, 46));
#endif
}


// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
	
    switch (settings.picture) {
			
	case 0:  // random mix
				
		if(s_random == 23){
			s_random = 0;
		} else {

			temp_random = rand() % 22;

			while(temp_random == s_random){
			    temp_random = rand() % 22;
		    }

		    s_random = temp_random;

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
    	}
// red wallpaper
		   if(s_random == 0){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG1);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// lazy   
		   } else if(s_random == 1){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG2);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// gt racer  
		   } else if(s_random == 2){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG3);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// map  
		   } else if(s_random == 3){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG4);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// clownfish
		   } else if(s_random == 4){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG5);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// lambo
		   } else if(s_random == 5){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG6);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );	   
// rave
		   } else if(s_random == 6){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG7);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// turntable
		   } else if(s_random == 7){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG8);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// ladybird
		   } else if(s_random == 8){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG9);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// tree frog
		   } else if(s_random == 9){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG10);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// blue daisy
		   } else if(s_random == 10){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG11);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// flowers
		   } else if(s_random == 11){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG12);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// apple
		   } else if(s_random == 12){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG13);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// orange
		   } else if(s_random == 13){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG14);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// dragon eye
		   } else if(s_random == 14){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG15);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// sunset
		   } else if(s_random == 15){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG16);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// green bubbles
		   } else if(s_random == 16){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG17);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// island
		   } else if(s_random == 17){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG18);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// nightmtn
		   } else if(s_random == 18){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG19);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// beach
		   } else if(s_random == 19){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG20);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
// redbull
		   } else if(s_random == 20){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG21);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
// cdj
		   } else if(s_random == 21){
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG22);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
}
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
    }			
		break;
			
	case 1:  // traditional wallpaper

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG1);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
	    		
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		
		}	
	case 2:  // lazy

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG2);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
	    	
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		
			
	case 3:  // gt racer

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG3);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );	    
				 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		
			
	case 4:  // map
	
	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG4);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		
			
	case 5:  // clownfish

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG5);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
				 	
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		
			
	case 6:  // lambo

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG6);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		
				
	case 7:  // rave

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG7);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		

	case 8:  // turntable

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG8);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		

	case 9:  // ladybird

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG9);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		

	case 10:  // tree frog

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG10);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;		
	case 11:  // blue daisy

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG11);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	

	case 12:  // Flowers

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG12);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	

	case 13:  // apple

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG13);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	

	case 14:  // orange

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG14);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	

	case 15:  // dragon eye

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG15);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	

	case 16:  // sunset

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG16);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	
	
	case 17:  // green bubbles

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG17);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;
	
	case 18:  // island

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG18);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	
		
	case 19:  // nightmtn

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG19);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;
	
	case 20:  // beach

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG20);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	
	
	case 21:  // red bull

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG21);
				text_layer_set_text_color(layer_time_text4, GColorBlack  );
				text_layer_set_text_color(layer_time_text3, GColorBlack  );
				text_layer_set_text_color(layer_time_text2, GColorBlack  );
				text_layer_set_text_color(layer_time_text, GColorBlack  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	
	
	case 22:  // cdj

	    if (background_image) {
		gbitmap_destroy(background_image);
		background_image = NULL;
	    }
				background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BG22);
				text_layer_set_text_color(layer_time_text4, GColorWhite  );
				text_layer_set_text_color(layer_time_text3, GColorWhite  );
				text_layer_set_text_color(layer_time_text2, GColorWhite  );
				text_layer_set_text_color(layer_time_text, GColorWhite  );
			
	   if (background_image != NULL) {
		bitmap_layer_set_bitmap(background_layer, background_image);
		layer_set_hidden(bitmap_layer_get_layer(background_layer), false);
		layer_mark_dirty(bitmap_layer_get_layer(background_layer));
       }
	    break;	
	} 

}

int stringToInt(char *str){
    int i=0,sum=0;
    while(str[i]!='\0'){
         if(str[i]< 48 || str[i] > 57){
            // if (DEBUG) APP_LOG(APP_LOG_LEVEL_ERROR, "Unable to convert it into integer.");
          //   return 0;
         }
         else{
             sum = sum*10 + (str[i] - 48);
             i++;
         }
    }
    return sum;
}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
	
  // Bluetoothvibe
  Tuple *animations_t = dict_find(iter, MESSAGE_KEY_bluetoothvibe);
  if (animations_t) {
    settings.bluetoothvibe = animations_t->value->int32 == 1;
  }

  // hourlyvibe
  Tuple *hourlyvibe_t = dict_find(iter, MESSAGE_KEY_hourlyvibe);
  if (hourlyvibe_t) {
    settings.hourlyvibe = hourlyvibe_t->value->int32 == 1;
  } 
  // random
//  Tuple *random_t = dict_find(iter, MESSAGE_KEY_random);
//  if (random_t) {
//    settings.random = random_t->value->int32 == 1;
//  }
  Tuple *random_t = dict_find(iter, MESSAGE_KEY_random);
  if (random_t) {
     settings.random = stringToInt( (char*) random_t->value->data );
  }
	
	
  // picture
  Tuple *picture_t = dict_find(iter, MESSAGE_KEY_picture);
  if (picture_t) {
     settings.picture = stringToInt( (char*) picture_t->value->data );
  }

  // Save the new settings to persistent storage
  prv_save_settings();
}


void destroy_property_animation(PropertyAnimation **prop_animation) {
    if (*prop_animation == NULL) {
        return;
    }

    if (animation_is_scheduled((Animation*) *prop_animation)) {
        animation_unschedule((Animation*) *prop_animation);
    }

    property_animation_destroy(*prop_animation);
    *prop_animation = NULL;
}

void animation_started(Animation *animation, void *data) {
    (void)animation;
    (void)data;
}

void animation_stopped(Animation *animation, void *data) {
    (void)animation;
    (void)data;

    destroy_property_animation(&mouth_animation_end);
    destroy_property_animation(&face_animation_end);
    destroy_property_animation(&nose_animation_end);
    destroy_property_animation(&ear_animation_end);
    mouth_animation_end = property_animation_create_layer_frame(bitmap_layer_get_layer(mouth_layer), &mouth_to_rect, &mouth_from_rect);
    face_animation_end = property_animation_create_layer_frame(bitmap_layer_get_layer(face_layer), &face_to_rect, &face_from_rect);
for (int i = 0; i < TOTAL_DATE_0DIGITS; i++) {
    nose_animation_end = property_animation_create_layer_frame(bitmap_layer_get_layer(nose_layer[i]), &nose_to_rect, &nose_from_rect);
}
for (int i = 0; i < TOTAL_DATE_1DIGITS; i++) {
	ear_animation_end = property_animation_create_layer_frame(bitmap_layer_get_layer(ear_layer[i]), &ear_to_rect, &ear_from_rect);
}
	animation_set_duration ((Animation*) mouth_animation_end, 3000 );
	animation_set_duration ((Animation*) face_animation_end, 3000 );
	animation_set_duration ((Animation*) nose_animation_end, 3000 );
	animation_set_duration ((Animation*) ear_animation_end, 3000 );
	animation_set_delay	(( Animation *) mouth_animation_end, 0 );	
	animation_set_delay	(( Animation *) face_animation_end, 0 );	
	animation_set_delay	(( Animation *) nose_animation_end, 0 );	
	animation_set_delay	(( Animation *) ear_animation_end, 0 );	
    animation_schedule((Animation*) mouth_animation_end);
    animation_schedule((Animation*) face_animation_end);
    animation_schedule((Animation*) nose_animation_end);
    animation_schedule((Animation*) ear_animation_end);	
}

void animatenow() {
    destroy_property_animation(&mouth_animation_beg);
    destroy_property_animation(&face_animation_beg);
    destroy_property_animation(&nose_animation_beg);
    destroy_property_animation(&ear_animation_beg);
	mouth_animation_beg = property_animation_create_layer_frame(bitmap_layer_get_layer(mouth_layer), &mouth_from_rect, &mouth_to_rect);
	face_animation_beg = property_animation_create_layer_frame(bitmap_layer_get_layer(face_layer), &face_from_rect, &face_to_rect);
	for (int i = 0; i < TOTAL_DATE_0DIGITS; i++) {
	nose_animation_beg = property_animation_create_layer_frame(bitmap_layer_get_layer(nose_layer[i]), &nose_from_rect, &nose_to_rect);
		  }
	for (int i = 0; i < TOTAL_DATE_1DIGITS; i++) {
	ear_animation_beg = property_animation_create_layer_frame(bitmap_layer_get_layer(ear_layer[i]), &ear_from_rect, &ear_to_rect);
		  }
	animation_set_duration ((Animation*) mouth_animation_beg, 3000 );
	animation_set_duration ((Animation*) face_animation_beg, 3000 );
	animation_set_duration ((Animation*) nose_animation_beg, 3000 );
	animation_set_duration ((Animation*) ear_animation_beg, 3000 );
animation_set_handlers ((Animation*) mouth_animation_beg, (AnimationHandlers) {
     .started = (AnimationStartedHandler) animation_started,
        .stopped = (AnimationStoppedHandler) animation_stopped
	}, 0);
animation_set_handlers ((Animation*) face_animation_beg, (AnimationHandlers) {
        .started = (AnimationStartedHandler) animation_started,
        .stopped = (AnimationStoppedHandler) animation_stopped
	}, 0);
animation_set_handlers ((Animation*) nose_animation_beg, (AnimationHandlers) {
        .started = (AnimationStartedHandler) animation_started,
        .stopped = (AnimationStoppedHandler) animation_stopped
	}, 0);
animation_set_handlers ((Animation*) ear_animation_beg, (AnimationHandlers) {
        .started = (AnimationStartedHandler) animation_started,
        .stopped = (AnimationStoppedHandler) animation_stopped
	}, 0);
	// section ends
	
    animation_schedule((Animation*) mouth_animation_beg);
    animation_schedule((Animation*) face_animation_beg);
    animation_schedule((Animation*) nose_animation_beg);
    animation_schedule((Animation*) ear_animation_beg);
}

// Shows animation
void show_status() {
	status_showing = true;
    animatenow();
}

// Shake/Tap Handler. On shake/tap... call "show_status"
void tap_handler(AccelAxisType axis, int32_t direction) {
	show_status();	
}

void update_time(struct tm *tick_time) {

	static char time_text[] = "00:00";
    char *time_format;

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
    text_layer_set_text(layer_time_text2, time_text);
    text_layer_set_text(layer_time_text3, time_text);
    text_layer_set_text(layer_time_text4, time_text);

}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	update_time(tick_time);
	
  if (units_changed & MONTH_UNIT) {
   update_months(tick_time);
  }
	if (units_changed & DAY_UNIT) {
    update_days(tick_time);
  }
  if (settings.random && (units_changed & HOUR_UNIT)) {
	  prv_update_display();

    } else {
	
if (!settings.random && (units_changed & MINUTE_UNIT)) {
	  prv_update_display(); 
}

  }
}


static void init(void) {
	
  prv_load_settings();
	
  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);

  window = window_create();
  if (window == NULL) {
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "OOM: couldn't allocate window");
      return;
  }
	
  window_set_background_color(window, GColorWhite);

  window_stack_push(window, true /* Animated */);

  window_layer = window_get_root_layer(window);
	
	time_font_emery = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_REGULAR_48));
	time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_REGULAR_38));
	batt_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_REGULAR_10));
	
//add background layer

  background_image = gbitmap_create_with_resource( RESOURCE_ID_LOADING );
  background_layer = bitmap_layer_create( layer_get_frame( window_layer ) );
  bitmap_layer_set_bitmap( background_layer, background_image );
  layer_add_child( window_layer, bitmap_layer_get_layer( background_layer ) );		
	
// time text ------------

#if PBL_PLATFORM_CHALK
    layer_time_text = text_layer_create(GRect(0, 17, 180, 40));
    text_layer_set_font(layer_time_text, time_font);	
	text_layer_set_background_color(layer_time_text, GColorClear);
    text_layer_set_text_alignment(layer_time_text, GTextAlignmentCenter);
    text_layer_set_text_color(layer_time_text, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text));	

    layer_time_text2 = text_layer_create(GRect(0, 16, 179, 40));
    text_layer_set_font(layer_time_text2, time_font);	
	text_layer_set_background_color(layer_time_text2, GColorClear);
    text_layer_set_text_alignment(layer_time_text2, GTextAlignmentCenter);
    text_layer_set_text_color(layer_time_text2, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text2));	
	
    layer_time_text3 = text_layer_create(GRect(0, 17, 179, 40));
    text_layer_set_font(layer_time_text3, time_font);	
	text_layer_set_background_color(layer_time_text3, GColorClear);
    text_layer_set_text_alignment(layer_time_text3, GTextAlignmentCenter);
    text_layer_set_text_color(layer_time_text3, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text3));	
	
    layer_time_text4 = text_layer_create(GRect(0, 16, 180, 40));
    text_layer_set_font(layer_time_text4, time_font);	
	text_layer_set_background_color(layer_time_text4, GColorClear);
    text_layer_set_text_alignment(layer_time_text4, GTextAlignmentCenter);
    text_layer_set_text_color(layer_time_text4, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text4));		
#elif PBL_PLATFORM_EMERY
    layer_time_text = text_layer_create(GRect(0, -7, 155, 50));
    text_layer_set_font(layer_time_text, time_font_emery);	
	text_layer_set_background_color(layer_time_text, GColorClear);
    text_layer_set_text_alignment(layer_time_text, GTextAlignmentRight);
    text_layer_set_text_color(layer_time_text, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text));	

    layer_time_text2 = text_layer_create(GRect(0, -8, 154, 50));
    text_layer_set_font(layer_time_text2, time_font_emery);	
	text_layer_set_background_color(layer_time_text2, GColorClear);
    text_layer_set_text_alignment(layer_time_text2, GTextAlignmentRight);
    text_layer_set_text_color(layer_time_text2, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text2));	
	
    layer_time_text3 = text_layer_create(GRect(0, -7, 154, 50));
    text_layer_set_font(layer_time_text3, time_font_emery);	
	text_layer_set_background_color(layer_time_text3, GColorClear);
    text_layer_set_text_alignment(layer_time_text3, GTextAlignmentRight);
    text_layer_set_text_color(layer_time_text3, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text3));	
	
    layer_time_text4 = text_layer_create(GRect(0, -8, 155, 50));
    text_layer_set_font(layer_time_text4, time_font_emery);	
	text_layer_set_background_color(layer_time_text4, GColorClear);
    text_layer_set_text_alignment(layer_time_text4, GTextAlignmentRight);
    text_layer_set_text_color(layer_time_text4, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text4));		
#else
    layer_time_text = text_layer_create(GRect(0, -6, 113, 40));
    text_layer_set_font(layer_time_text, time_font);	
	text_layer_set_background_color(layer_time_text, GColorClear);
    text_layer_set_text_alignment(layer_time_text, GTextAlignmentRight);
    text_layer_set_text_color(layer_time_text, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text));	

    layer_time_text2 = text_layer_create(GRect(0, -7, 112, 40));
    text_layer_set_font(layer_time_text2, time_font);	
	text_layer_set_background_color(layer_time_text2, GColorClear);
    text_layer_set_text_alignment(layer_time_text2, GTextAlignmentRight);
    text_layer_set_text_color(layer_time_text2, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text2));	
	
    layer_time_text3 = text_layer_create(GRect(0, -6, 112, 40));
    text_layer_set_font(layer_time_text3, time_font);	
	text_layer_set_background_color(layer_time_text3, GColorClear);
    text_layer_set_text_alignment(layer_time_text3, GTextAlignmentRight);
    text_layer_set_text_color(layer_time_text3, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text3));	
	
    layer_time_text4 = text_layer_create(GRect(0, -7, 113, 40));
    text_layer_set_font(layer_time_text4, time_font);	
	text_layer_set_background_color(layer_time_text4, GColorClear);
    text_layer_set_text_alignment(layer_time_text4, GTextAlignmentRight);
    text_layer_set_text_color(layer_time_text4, GColorBlack);
    layer_add_child(window_layer, text_layer_get_layer(layer_time_text4));		
#endif

	
//--- animation layers --------
	
#if PBL_PLATFORM_CHALK
	//day
mouth_from_rect = GRect(190, 0, 33, 180);
mouth_to_rect = GRect(110, 0, 33, 180);
    //month
face_from_rect = GRect(190, 0, 32, 180);
face_to_rect = GRect(138, 0, 32, 180);
	//date0
nose_from_rect = GRect(190, 77, 15, 11);
nose_to_rect = GRect(97, 77, 15, 11);
	//date1    
ear_from_rect = GRect(190, 87, 15, 11);
ear_to_rect = GRect(97, 87, 15,11);
	
#elif PBL_PLATFORM_EMERY
mouth_from_rect = GRect(220, 46, 20, 184);
mouth_to_rect = GRect(160, 46, 20, 184);
    
face_from_rect = GRect(220, 74, 20, 180);
face_to_rect = GRect(180, 74, 20, 180);

nose_from_rect = GRect(220, 46, 20, 11);
nose_to_rect = GRect(180, 46, 20, 11);
    
ear_from_rect = GRect(220, 60, 20, 11);
ear_to_rect = GRect(180, 60, 20, 11);
	
#else
mouth_from_rect = GRect(160, 36, 15, 140);
mouth_to_rect = GRect(115, 36, 15, 140);
    
face_from_rect = GRect(160, 57, 15, 120);
face_to_rect = GRect(129, 57, 15, 120);

nose_from_rect = GRect(160, 36, 15, 11);
nose_to_rect = GRect(129, 36, 15, 11);
    
ear_from_rect = GRect(160, 46, 15, 11);
ear_to_rect = GRect(129, 46, 15,11);
	
#endif	
	
	
mouth_layer = bitmap_layer_create(mouth_from_rect);
GCompOp compositing_mode_mouth = GCompOpSet;
bitmap_layer_set_compositing_mode(mouth_layer, compositing_mode_mouth);
bitmap_layer_set_bitmap(mouth_layer, mouth);
layer_add_child(window_layer, bitmap_layer_get_layer(mouth_layer));

face_layer = bitmap_layer_create(face_from_rect);
GCompOp compositing_mode_face = GCompOpSet;
bitmap_layer_set_compositing_mode(face_layer, compositing_mode_face);
bitmap_layer_set_bitmap(face_layer, face);
layer_add_child(window_layer, bitmap_layer_get_layer(face_layer));

for (int i = 0; i < TOTAL_DATE_0DIGITS; i++) {
nose_layer[i] = bitmap_layer_create(nose_from_rect);
layer_add_child(window_layer, bitmap_layer_get_layer(nose_layer[i]));
	  }	
for (int i = 0; i < TOTAL_DATE_1DIGITS; i++) {
ear_layer[i] = bitmap_layer_create(ear_from_rect);
layer_add_child(window_layer, bitmap_layer_get_layer(ear_layer[i]));
	  }
	
	
	
// other bits ------------
	
  bluetooth_image_on = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTHON);
  GRect bitmap_bounds_bt_on = gbitmap_get_bounds(bluetooth_image_on);	
#ifdef PBL_PLATFORM_EMERY
	GRect frame_bton = GRect(160, 0, bitmap_bounds_bt_on.size.w, bitmap_bounds_bt_on.size.h);
#elif PBL_PLATFORM_CHALK
	GRect frame_bton = GRect(81, 170, bitmap_bounds_bt_on.size.w, bitmap_bounds_bt_on.size.h);
#else 
	GRect frame_bton = GRect(115, 0, bitmap_bounds_bt_on.size.w, bitmap_bounds_bt_on.size.h);
#endif
  bluetooth_layer_on = bitmap_layer_create(frame_bton);
	GCompOp compositing_mode_bton = GCompOpSet;
    bitmap_layer_set_compositing_mode(bluetooth_layer_on, compositing_mode_bton);
  bitmap_layer_set_bitmap(bluetooth_layer_on, bluetooth_image_on);
  layer_add_child(window_layer, bitmap_layer_get_layer(bluetooth_layer_on));
	
	
  bluetooth_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BLUETOOTHOFF);
  GRect bitmap_bounds2 = gbitmap_get_bounds(bluetooth_image);
#ifdef PBL_PLATFORM_EMERY
    GRect frame2 = GRect(160, 0, bitmap_bounds2.size.w, bitmap_bounds2.size.h);
#elif PBL_PLATFORM_CHALK
    GRect frame2 = GRect(81, 170, bitmap_bounds2.size.w, bitmap_bounds2.size.h);
#else
	GRect frame2 = GRect(115, 0, bitmap_bounds2.size.w, bitmap_bounds2.size.h);
#endif
  bluetooth_layer = bitmap_layer_create(frame2);
	GCompOp compositing_mode_btoff = GCompOpSet;
    bitmap_layer_set_compositing_mode(bluetooth_layer, compositing_mode_btoff);
  bitmap_layer_set_bitmap(bluetooth_layer, bluetooth_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(bluetooth_layer));

#ifdef PBL_PLATFORM_CHALK
  battcircle_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTCIRCLE);
  GRect bitmap_bounds3 = gbitmap_get_bounds(battcircle_image);
  GRect frame3 = GRect(70, 0, bitmap_bounds3.size.w, bitmap_bounds3.size.h);
  battcircle_layer = bitmap_layer_create(frame3);
  GCompOp compositing_mode_batt = GCompOpSet;
  bitmap_layer_set_compositing_mode(battcircle_layer, compositing_mode_batt);
  bitmap_layer_set_bitmap(battcircle_layer, battcircle_image);
  layer_add_child(window_layer, bitmap_layer_get_layer(battcircle_layer));
#endif	
	
#ifdef PBL_PLATFORM_EMERY
	battery_text_layer = text_layer_create(GRect(160, 5, 40, 20));	
#elif PBL_PLATFORM_CHALK
	battery_text_layer = text_layer_create(GRect(0, 3, 180, 20));	
#else
	battery_text_layer = text_layer_create(GRect(116, 3, 26, 20));	
#endif
    text_layer_set_text_color(battery_text_layer, GColorWhite);
	text_layer_set_background_color(battery_text_layer, GColorClear);
    text_layer_set_font(battery_text_layer, batt_font);
    text_layer_set_text_alignment(battery_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));


  toggle_bluetooth_icon(bluetooth_connection_service_peek());
  update_battery(battery_state_service_peek());

  prv_update_display();

  // Avoids a blank screen on watch start.
  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);  
  handle_tick(tick_time, MONTH_UNIT + DAY_UNIT + HOUR_UNIT + MINUTE_UNIT );

  tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);

  bluetooth_connection_service_subscribe(bluetooth_connection_callback);
  battery_state_service_subscribe(&update_battery);
	
  accel_tap_service_subscribe(tap_handler);
  show_status();
}

static void deinit(void) {
	
  tick_timer_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  battery_state_service_unsubscribe();
  accel_tap_service_unsubscribe();
	
  layer_remove_from_parent(bitmap_layer_get_layer(background_layer));
  bitmap_layer_destroy(background_layer);

	if (background_image != NULL) {
		gbitmap_destroy(background_image);
    }
	
  fonts_unload_custom_font(time_font);
  fonts_unload_custom_font(time_font_emery);
  fonts_unload_custom_font(batt_font);

  layer_remove_from_parent(bitmap_layer_get_layer(bluetooth_layer));
  bitmap_layer_destroy(bluetooth_layer);
  gbitmap_destroy(bluetooth_image);
  
  layer_remove_from_parent(bitmap_layer_get_layer(bluetooth_layer_on));
  bitmap_layer_destroy(bluetooth_layer_on);
  gbitmap_destroy(bluetooth_image_on);
	
  layer_remove_from_parent(bitmap_layer_get_layer(battcircle_layer));
  bitmap_layer_destroy(battcircle_layer);
  gbitmap_destroy(battcircle_image);
	
	destroy_property_animation(&mouth_animation_beg);
    destroy_property_animation(&mouth_animation_end);
	
	destroy_property_animation(&face_animation_beg);
    destroy_property_animation(&face_animation_end);
	
	destroy_property_animation(&nose_animation_beg);
    destroy_property_animation(&nose_animation_end);
	
	destroy_property_animation(&ear_animation_beg);
    destroy_property_animation(&ear_animation_end);

    bitmap_layer_destroy(mouth_layer);
	bitmap_layer_destroy(face_layer);
	
    gbitmap_destroy(mouth);
    gbitmap_destroy(face);

  text_layer_destroy(layer_time_text);
  text_layer_destroy(layer_time_text2);
  text_layer_destroy(layer_time_text3);
  text_layer_destroy(layer_time_text4);
  text_layer_destroy(battery_text_layer);

  for (int i = 0; i < TOTAL_DATE_0DIGITS; i++) {
	layer_remove_from_parent(bitmap_layer_get_layer(nose_layer[i]));
    gbitmap_destroy(nose[i]);
    bitmap_layer_destroy(nose_layer[i]);
  }

  for (int i = 0; i < TOTAL_DATE_1DIGITS; i++) {
	layer_remove_from_parent(bitmap_layer_get_layer(ear_layer[i]));
    gbitmap_destroy(ear[i]);
    bitmap_layer_destroy(ear_layer[i]);
  }
	
  window_destroy(window);
  
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}