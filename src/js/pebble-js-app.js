var mConfig = {};

Pebble.addEventListener("ready", function(e) {
  loadLocalData();
  returnConfigToPebble();
});

Pebble.addEventListener("showConfiguration", function(e) {
	Pebble.openURL(mConfig.configureUrl);
});

Pebble.addEventListener("webviewclosed",
  function(e) {
    if (e.response) {
      var config = JSON.parse(e.response);
      saveLocalData(config);
      returnConfigToPebble();
    }
  }
);

function saveLocalData(config) {

  localStorage.setItem("bluetoothvibe", parseInt(config.bluetoothvibe)); 
  localStorage.setItem("hourlyvibe", parseInt(config.hourlyvibe)); 
  localStorage.setItem("background", parseInt(config.background)); 
 
  loadLocalData();

}
function loadLocalData() {
  
	mConfig.bluetoothvibe = parseInt(localStorage.getItem("bluetoothvibe"));
	mConfig.hourlyvibe = parseInt(localStorage.getItem("hourlyvibe"));
	mConfig.background = parseInt(localStorage.getItem("background"));
	mConfig.configureUrl = "http://www.themapman.com/pebblewatch/wallpaperx.html";

	
	if(isNaN(mConfig.bluetoothvibe)) {
		mConfig.bluetoothvibe = 1;
	}
	if(isNaN(mConfig.hourlyvibe)) {
		mConfig.hourlyvibe = 0;
	} 
	if(isNaN(mConfig.background)) {
		mConfig.background = 0;
	} 
}
function returnConfigToPebble() {
  Pebble.sendAppMessage({

    "bluetoothvibe":parseInt(mConfig.bluetoothvibe), 
    "hourlyvibe":parseInt(mConfig.hourlyvibe),
    "background":parseInt(mConfig.background)
  });    
}