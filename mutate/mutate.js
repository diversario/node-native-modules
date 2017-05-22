const mutate = require('./build/Release/mutate')

mutate.mutate(function (msg) {
  console.log(msg || `hello`)
})

let o = {}
console.log(o)
mutate.mutate(o)
console.log(o)