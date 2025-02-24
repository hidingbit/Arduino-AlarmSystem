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
  const address = '0xe7f1725E7734CE288F8367e1Bb143E90bb3F0512';
  const AlarmStorage = await ethers.getContractFactory('AlarmStorage');
  const alarmStorage = AlarmStorage.attach(address);
  
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
        console.log("Intrusion detected! ");
      } else if (topic === 'fire') {
        console.log("Fire detected! ");
      } else if (topic === 'info') {
        console.log("Scheduled system status info: ");
      } else {
        console.log("Something went wrong! Message: ");
      }
      
      await alarmStorage.store(complete_message);
      const value = await alarmStorage.retrieve();
      console.log(value.toString());
    });

  });
}

main()
  .then(() => process.exit(0))
  .catch(error => {
    console.error(error);
    process.exit(1);
  });
