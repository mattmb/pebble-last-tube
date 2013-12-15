var locationOptions = { "timeout": 15000, "maximumAge": 60000 };

//On ready do geolocation and get data.
Pebble.addEventListener("ready",
function(e) {
        console.log("connect!" + e.ready);
        window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
}
);

var coordinates;
var data;

function locationSuccess(pos) {
  coordinates = pos.coords;
  console.log(coordinates.latitude, coordinates.longitude);
  getLastTubeData(coordinates.latitude, coordinates.longitude);
}
function locationError(err) {
  console.log(err.code + "," + err.message);
}

//on appmessage we look up the requested
//entry in our results.
Pebble.addEventListener("appmessage",
                        function(e) {
                          console.log("message!");
                          console.log(e.payload);
                          console.log(e.payload.tube);
                          var tube = e.payload.tube;
                          getLastTube(data,tube);
                        });

function getLastTubeData(lat,lon) {
  var url = 'http://darwin.meadbriggs.net:5000/get/' + lat + '/' + lon;
  var xhr = new XMLHttpRequest();
  xhr.open("GET", url, true);
  xhr.onload = function (e) {
    if (xhr.readyState === 4) {
      if (xhr.status === 200) {
        console.log(xhr.responseText);
        var response = JSON.parse(xhr.responseText);
        data = response;
        var transactionId = Pebble.sendAppMessage({'station':'Press Down','line':'To View','time':'','destination':'Next Entry'},
            function(e) {
              console.log("Successfully delivered message with transactionId="
                + e.data.transactionId);
            },    
            function(e) {
              console.log("Unable to deliver message with transactionId="
                + e.data.transactionId
                + " Error is: " + e.error.message);
            }    
          );    
      } else {
        console.error(xhr.statusText);
      }
    }
  };
  xhr.onerror = function (e) {
    console.error(xhr.statusText);
  };
  xhr.send(null);
}

//send back the result
function getLastTube(response, tube) {
  if (response == undefined){
    tube = 0;
    response = [{}];
    response[tube]['station'] = "No Connection";
    response[tube]['line'] = "To Server";
    response[tube]['destination'] = "";
    response[tube]['last'] = ["",""]; 
  }     
  if (response[tube] == undefined){
    tube = 0;
    response = [{}];
    response[tube]['station'] = "No More";
    response[tube]['line'] = "Results";
    response[tube]['destination'] = "";
    response[tube]['last'] = ["",""]; 
  }     
  var time = response[tube]['last'][0];
  var line = response[tube]['line'];
  var station = response[tube]['station'];
  var destination = response[tube]['last'][1];
  if (line !== "Results" && line !== "To Server") {
    line = line + " (" + response[tube]['direction'] + ")";
  }
  var transactionId = Pebble.sendAppMessage({'station':station,'line':line,'time':time,'destination':destination},
    function(e) {
      console.log("Successfully delivered message with transactionId="
        + e.data.transactionId);
    },      
    function(e) {
      console.log("Unable to deliver message with transactionId="
        + e.data.transactionId
        + " Error is: " + e.error.message);
    }       
  );    
  console.log("SENT")
  console.log(station)
}
