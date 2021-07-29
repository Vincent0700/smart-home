const ora = require('ora');
const mqtt = require('mqtt');
const chalk = require('chalk');
const { InfluxDB, Point } = require('@influxdata/influxdb-client');

const config = require('../../config.json');

const SERVER_URL = `mqtt://${config.mqtt.host}:${config.mqtt.port}`;
const TEXT_CONNECT = `connect to ${chalk.cyan.underline(SERVER_URL)}`;

const spinner = ora({
  text: TEXT_CONNECT,
  spinner: 'star'
}).start();

// connect mqtt
const client = mqtt.connect(SERVER_URL);

// connect influxdb
const influxdb = new InfluxDB({
  url: config.influxdb.url,
  token: config.influxdb.token
});

const writeApi = influxdb.getWriteApi(config.influxdb.org, config.influxdb.bucket);
writeApi.useDefaultTags({ host: 'mqtt-server' });

client.on('message', function (topic, message) {
  if (topic === 'device/bme280') {
    const data = JSON.parse(message);
    console.log(data);

    const point = new Point('bme280/' + data.id);
    point.floatField('temp', data.temp);
    point.floatField('humi', data.humi);
    point.floatField('pres', data.pres);
    writeApi.writePoint(point);
  }
});

client.on('connect', function () {
  spinner.stop();
  console.log(chalk.greenBright('✔'), TEXT_CONNECT);
  console.log();
  client.subscribe('device/+');
});

process.on('exit', () => {
  // TODO: handle exit
  writeApi.close();
});

process.on('SIGINT', function () {
  process.exit();
});
