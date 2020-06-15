const mqtt = require('mqtt')
const uuidv1 = require('uuid/v1')

let mqHost = 'mqtt://roedeer.rmq.cloudamqp.com'

const options = {
  username: 'dxiymulv:dxiymulv',
  password: 'khICbXilmBfD4nB3jMhlTpD19t5BiIWl',
  clientId: 'lambda_' + uuidv1(),
  port: 1883
}

const topic = "test-topic"

let client = mqtt.connect(mqHost, options)

// MQTT publish takes a buffer or string
let message = 'Grisha'

client.on('connect', function () {
  const rs = client.publish(topic, message)
  console.log(rs);
  
  client.end()
})
