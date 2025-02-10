const mqtt = require("mqtt");
const fs = require('fs');
const { json } = require("stream/consumers");

const protocol = 'mqtts';
const host = '192.168.4.3';
const port = '8883';
const connectUrl = `${protocol}://${host}:${port};`
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`
  
const client = mqtt.connect(
  connectUrl, {  
  clientId,
    rejectUnauthorized: true,
    ca: fs.readFileSync('./certs/ca.crt'),
    key: fs.readFileSync('./certs/client.key'),
    cert: fs.readFileSync('./certs/client.crt')
});

async function main () {
  const address = '0x5FbDB2315678afecb367f032d93F642f64180aa3';
  const Box = await ethers.getContractFactory('Box');
  const box = Box.attach(address);
  
  const topics = ['intrusion', 'fire', 'info'];
  client.subscribe(topics, (err) => {
    if (err) {
        console.error('Subscription error: ', err);
    } else {
        console.log('Subscribed to: ', topics);
    }
    return err;
  });
  
  return new Promise((resolve, reject) => {
    client.on('message', async (topic, message) => {
      // Adding the timestamp on the received message
      time = new Date();
      json_message = JSON.parse(message);
      json_message.datetime = time;
      complete_message = JSON.stringify(json_message);

      if (topic === 'intrusion') {
        console.log("Intrusion detected! Message: " + complete_message);
      } else if (topic === 'fire') {
        console.log("Fire detected! Message: " + complete_message);
      } else if (topic === 'info') {
        console.log("Scheduled system status info: " + complete_message);
      } else {
        console.log("Something went wrong: " + complete_message);
      }
      
      await box.store(complete_message);
      const value = await box.retrieve();
      console.log("Box value is:\n" + value.toString());
    });

  });
}

main()
  .then(() => process.exit(0))
  .catch(error => {
    console.error(error);
    process.exit(1);
  });
