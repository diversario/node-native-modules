const cmath = require('./build/Release/cmath.node')

const five = cmath.add(2, 3)
console.log(`five ${five}`)

try {
  cmath.add()
} catch (error) {
  console.log(error)
}


try {
  cmath.add(3)
} catch (error) {
  console.log(error)
}


try {
  cmath.add(2, 'sdf')
} catch (error) {
  console.log(error)
}