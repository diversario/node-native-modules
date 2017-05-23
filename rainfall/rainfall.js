let addon = require('./cpp/build/Release/rainfall.node')

var makeup = function (max) {
  return Math.round(max * Math.random() * 100) / 100;
}

var locations = []
for (var i = 0; i < 10; i++) {
  var loc = {
    latitude: makeup(180),
    longitude: makeup(180),
    samples: [
      { date: "2015-07-20", rainfall: makeup(3) },
      { date: "2015-07-21", rainfall: makeup(3) },
      { date: "2015-07-22", rainfall: makeup(3) },
      { date: "2015-07-23", rainfall: makeup(3) }
    ]
  }
  locations.push(loc);
}

let average = addon.avg_rainfall(locations[0])
console.log(average)

console.log(addon.rainfall_data(locations[0]))