module.exports = [
  {
    "type": "heading",
    "defaultValue": "Wallpaper Configuration"
  },
     {
        "type": "text",
        "defaultValue": "<h6>A white toggle = OFF, an orange toggle = ON</h6>",
      },
  {
    "type": "section",
    "items": [
 	     {
        "type": "heading",
        "defaultValue": "<h5>Display</h5>"
      },
	  {
  "type": "radiogroup",
  "messageKey": "random",
  "label": "Random behaviour <br>(ONLY when RANDOM is selected below)",
  "options": [
    { 
      "label": "Change each Minute", 
      "value": 0
    },
    { 
      "label": "Change each Hour", 
      "value": 1
    },
	  ],
	  },
//      {
//        "type": "toggle",
//        "messageKey": "random",
//        "label": "Random frequency",
//        "defaultValue": false
//      },
		{
  "type": "select",
  "messageKey": "picture",
  "defaultValue": 0,
  "label": "Random or select an individual image",
  "options": [

    { 
      "label": "Random",
      "value": 0
    },
	{ 
      "label": "Traditional",
      "value": 1
    },
    { 
      "label": "Lazy day",
      "value": 2
    },
    { 
      "label": "GT Racer",
      "value": 3
    },
    { 
      "label": "Old map",
      "value": 4
    },
    { 
      "label": "Clownfish",
      "value": 5
    },
	{ 
      "label": "Lamborghini Aventador",
      "value": 6
    },
    { 
      "label": "Dance party",
      "value": 7
    },
    { 
      "label": "Turntable",
      "value": 8
    },
    { 
      "label": "Lady Bird",
      "value": 9
    },
	{ 
      "label": "Tree Frog",
      "value": 10
    },
    { 
      "label": "Daisy",
      "value": 11
    },
	{ 
      "label": "Flowers",
      "value": 12
    },
    { 
      "label": "Apple",
      "value": 13
    },
    { 
      "label": "Orange Peel",
      "value": 14
    },
	{ 
      "label": "Dragon eye",
      "value": 15
    },
    { 
      "label": "Sunset",
      "value": 16
	},
	{ 
      "label": "Green bubbles",
      "value": 17
	},
	{ 
      "label": "Tropical island",
      "value": 18
	},
	{ 
      "label": "Mountain evening",
      "value": 19
	},
	{ 
      "label": "Deserted beach",
      "value": 20
	},
	{ 
      "label": "Redbull F1 car",
      "value": 21
	},
	{ 
      "label": "Pioneer CDJ",
      "value": 22
	}
	  ]
		}
  ]
},
  	
{
    "type": "section",
    "items": [	
 	  {
        "type": "heading",
        "defaultValue": "<h5>Vibration</h5>"
      },
      {
        "type": "toggle",
        "messageKey": "bluetoothvibe",
        "label": "Bluetooth Vibration",
        "defaultValue": false
      },
	  {
        "type": "toggle",
        "messageKey": "hourlyvibe",
        "label": "Vibrate each hour",
        "defaultValue": false
      }
    ]
  },
  {
        "type": "text",
        "defaultValue": "<h6>This watchface will continue to be free.  If you find it useful, please consider making a <a href='https://www.paypal.me/markchopsreed'>small donation here</a>. Thankyou.</h6>",
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];