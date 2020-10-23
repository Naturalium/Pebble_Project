

/*
Pebble.addEventListener('ready',
  function(e) {
    console.log('PebbleKit JS ready!');
  }
);

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    getWeather();
  }
);
*/


/*
var xhrRequest = function(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};
*/


function xhrRequest(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function() {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};


function locationSuccess(pos) {
  console.log('trying to access with weather map')

    var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
    pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + '0a79e7655794528bfd97f15fac75f5e9';

  //send request to OpenWeatherMap

  xhrRequest(url, 'GET',
    function(responseText) {
      console.log(responseText);
      //responseText contains a JSON object with weather info
      var json = JSON.parse(responseText);

      //Temperature in Kelvin requires adjustment
      var temperature = Math.round(json.main.temp - 273.15);
      console.log('Temperature is ' + temperature);

      //Conditions
      var conditions = json.weather[0].main;
      console.log('Conditions are ' + conditions);
     
      
      var dictionary = {
        'TEMPERATURE': temperature,
        'CONDITIONS': conditions
      };

      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log('weather info sent to Pebble successfully!');
        },
        function(e) {
          console.log('Error sending weather info to Pebble!');
        }
      );
    }
  );

//  getweather();

//  var temperature = Math.round(json.main.temp - 273.15);
//  console.log('Temperature is ' + temperature);

//  var conditions = json.weather[0].main;
//  console.log('Conditions are ' + conditions);
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

Pebble.addEventListener('ready',
  function(e) {
    console.log('PebbleKit JS ready!');

    //get initial weather
    getWeather();
  }
)

Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    getWeather();
  }
);
/*
var dictionary = {
  'TEMPERATURE': temperature,
  'CONDITIONS': conditions
};


// Send to Pebble
Pebble.sendAppMessage(dictionary,
  function(e) {
    console.log('weather info sent to Pebble successfully!');
  },
  function(e) {
    console.log('Error sending weather info to Pebble!');
  }
);
*/
