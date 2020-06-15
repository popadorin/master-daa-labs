const mqtt = require('mqtt')
const uuidv1 = require('uuid/v1');

let options = {
  username: 'dxiymulv:dxiymulv',
  password: 'khICbXilmBfD4nB3jMhlTpD19t5BiIWl',
  clientId: 'mqttLambda_' + uuidv1(),
  port: 1883
}

let mqEndpoint = 'mqtt://roedeer.rmq.cloudamqp.com'
let topic = 'test-topic'

let client = mqtt.connect(mqEndpoint, options)

client.on('connect', function() {
  console.log("connected")
  client.subscribe(topic, function (err) {
    if(err) console.log(err)
  })
})

client.on('error', function (error) {
  console.log(error)
})

// Log messages
client.on('message', function (topic, message) {
  console.log(`message received on ${topic}: ${message.toString()}`)
})
