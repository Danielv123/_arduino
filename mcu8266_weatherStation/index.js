var mqtt = require('mqtt')
var client  = mqtt.connect('ws://iot.eclipse.org:80/ws')

const Prometheus = require('prom-client');
const temperatureGauge = new Prometheus.Gauge({
	name: 'temparature',
	help: 'metric_help',
	labelNames: ['room']
});
const humidityGauge = new Prometheus.Gauge({
	name: 'humidity',
	help: 'metric_help',
	labelNames: ['room']
});

client.on('connect', function () {
	client.subscribe('Daniel', function (err) {
		if (!err) {
			client.publish('presence', 'Hello mqtt')
		}
	})
})
 
client.on('message', function (topic, message) {
	// message is Buffer
	message = message.toString();
	let data = message.split("|")
	console.log(data)
	temperatureGauge.labels(data[0]).set(Number(data[1]))
	humidityGauge.labels(data[0]).set(Number(data[2]))
	
	pushgateway.push({ jobName: 'weatherStation' }, function(err, resp, body) {})
})
const pushgateway = new Prometheus.Pushgateway('http://hme.danielv.no:9091');
