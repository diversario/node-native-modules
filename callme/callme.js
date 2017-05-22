const callme = require('./build/Release/callme')

callme.callme(function (msg) {
  console.log(msg || `hello`)
})


callme.callme_withthis(function (msg) {
  console.log(msg || `hello`)
}, 'dddddddddd')
