Pebble.addEventListener('ready',
  function(e) {
    console.log('PebbleKit JS ready!');
}

);

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
}
);





var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
}


function locationSuccess(pos) {

var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
    pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + myAPIKey;

    xhrRequest(url, 'GET',
      function(responseText) {
	var json = JSON.parse(responseText);
	var temperature = Math.round(json.main.temp - 273.15);
	console.log('Temperature is ' + temperature);
	var conditions = json.weather[0].main;
	console.log('Conditions are ' + conditions);
}
);
}

function locationError(err) {
  console.log('Error requesting location!');
}

function getWeather() {
  navigator.geolocation.getCurrentPosition(
    locationSuccess,
    locationError,
    {timeout: 15000, maximumAge: 60000}
  );
}

pebble.addEventListener('ready',
  function(e) {
    console.log('PebbleKit JSs ready!');

    getWeather();
  }
);
