const pass_stuff = require('./build/Release/pass_stuff')

console.log(pass_stuff.number(3))
// console.log(pass_stuff.number("4"))
// console.log(pass_stuff.number('five'))
// console.log(pass_stuff.number(null))

console.log(pass_stuff.int(3.5))
// console.log(pass_stuff.int("4.5"))
// console.log(pass_stuff.int('five'))
// console.log(pass_stuff.int(null))

console.log(pass_stuff.bool(3.5))
console.log(pass_stuff.bool())
console.log(pass_stuff.bool(true))
console.log(pass_stuff.bool(false))

console.log(pass_stuff.string('abcdef'))