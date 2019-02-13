var mqtt = require('mqtt')
var client = mqtt.connect('ws://iot.eclipse.org:80/ws')

const Prometheus = require('prom-client');
const powerConsumptionGauge = new Prometheus.Gauge({
	name: 'power_consumption',
	help: 'Current power consumption in watts',
	labelNames: ['device']
});
const powerUsed = new Prometheus.Gauge({
	name: 'power_used',
	help: 'Amount of power used in WH since monitor restarted',
	labelNames: ['device']
});

client.on('connect', function () {
	client.subscribe('Danielv123/power', function (err) {
		if (!err) {
			client.publish('presence', 'Hello mqtt')
		}
	})
})
 
client.on('message', function (topic, message) {
	// message is Buffer
	message = message.toString();
	let data = message.split("§")
	console.log(data)
	powerConsumptionGauge.labels(data[0]).set(Number(data[3].split("|")[0]))
	powerUsed.labels(data[0]).set(Number(data[4].split("|")[0]))
	
	pushgateway.push({ jobName: 'powerMonitor' }, function(err, resp, body) {})
})
const pushgateway = new Prometheus.Pushgateway('http://hme.danielv.no:9091');
