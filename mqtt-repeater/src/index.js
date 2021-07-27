const ora = require('ora');
const mqtt = require('mqtt');
const chalk = require('chalk');
const config = require('../../config.json');

const SERVER_URL = `mqtt://${config.mqtt.host}:${config.mqtt.port}`;
const TEXT_CONNECT = `connect to ${chalk.cyan.underline(SERVER_URL)}`;

const spinner = ora({
  text: TEXT_CONNECT,
  spinner: 'star'
}).start();

const client = mqtt.connect(SERVER_URL);

client.on('message', function (topic, message) {
  console.log(`>> ${topic}: ${message}`);
});

client.on('connect', function () {
  spinner.stop();
  console.log(chalk.greenBright('✔'), TEXT_CONNECT);
  console.log();
  client.subscribe('sensor/+');
});

process.on('exit', () => {
  // TODO: handle exit
});

process.on('SIGINT', function () {
  process.exit();
});
