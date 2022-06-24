#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Ticker.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define DHTPIN 4
#define DHTPIN2 5
#define LED1 12
#define LED2 13

#define DHTTYPE DHT11
DHT dhtRoom1(DHTPIN, DHTTYPE);
DHT dhtRoom2(DHTPIN2, DHTTYPE);

int L1, L2;

void sendSensor();
void sendLEDStatus(int L1, int L2);
Ticker timer;

char homePage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
	<meta charset="UTF-8" />
	<meta http-equiv="X-UA-Compatible" content="IE=edge" />
	<meta name="viewport" content="width=device-width, initial-scale=1.0" />
	<!-- Font Awesome -->
	<link href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css" rel="stylesheet" />

	<link rel="stylesheet" href="https://unicons.iconscout.com/release/v4.0.0/css/line.css" />
	<!-- Google Fonts -->
	<link href="https://fonts.googleapis.com/css?family=Roboto:300,400,500,700&display=swap" rel="stylesheet" />
	<!-- MDB -->
	<link href="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/4.0.0/mdb.min.css" rel="stylesheet" />
	<!-- MDB -->
	<script type="text/javascript" src="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/4.0.0/mdb.min.js"></script>
	<!-- <script src="main.js"></script> -->
	<title>IoT Weather App</title>
</head>

<body>
	<style>
		.bg {
			background-color: #ffffff;
			background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 304 304' width='304' height='304'%3E%3Cpath fill='%23868b94' fill-opacity='0.4' d='M44.1 224a5 5 0 1 1 0 2H0v-2h44.1zm160 48a5 5 0 1 1 0 2H82v-2h122.1zm57.8-46a5 5 0 1 1 0-2H304v2h-42.1zm0 16a5 5 0 1 1 0-2H304v2h-42.1zm6.2-114a5 5 0 1 1 0 2h-86.2a5 5 0 1 1 0-2h86.2zm-256-48a5 5 0 1 1 0 2H0v-2h12.1zm185.8 34a5 5 0 1 1 0-2h86.2a5 5 0 1 1 0 2h-86.2zM258 12.1a5 5 0 1 1-2 0V0h2v12.1zm-64 208a5 5 0 1 1-2 0v-54.2a5 5 0 1 1 2 0v54.2zm48-198.2V80h62v2h-64V21.9a5 5 0 1 1 2 0zm16 16V64h46v2h-48V37.9a5 5 0 1 1 2 0zm-128 96V208h16v12.1a5 5 0 1 1-2 0V210h-16v-76.1a5 5 0 1 1 2 0zm-5.9-21.9a5 5 0 1 1 0 2H114v48H85.9a5 5 0 1 1 0-2H112v-48h12.1zm-6.2 130a5 5 0 1 1 0-2H176v-74.1a5 5 0 1 1 2 0V242h-60.1zm-16-64a5 5 0 1 1 0-2H114v48h10.1a5 5 0 1 1 0 2H112v-48h-10.1zM66 284.1a5 5 0 1 1-2 0V274H50v30h-2v-32h18v12.1zM236.1 176a5 5 0 1 1 0 2H226v94h48v32h-2v-30h-48v-98h12.1zm25.8-30a5 5 0 1 1 0-2H274v44.1a5 5 0 1 1-2 0V146h-10.1zm-64 96a5 5 0 1 1 0-2H208v-80h16v-14h-42.1a5 5 0 1 1 0-2H226v18h-16v80h-12.1zm86.2-210a5 5 0 1 1 0 2H272V0h2v32h10.1zM98 101.9V146H53.9a5 5 0 1 1 0-2H96v-42.1a5 5 0 1 1 2 0zM53.9 34a5 5 0 1 1 0-2H80V0h2v34H53.9zm60.1 3.9V66H82v64H69.9a5 5 0 1 1 0-2H80V64h32V37.9a5 5 0 1 1 2 0zM101.9 82a5 5 0 1 1 0-2H128V37.9a5 5 0 1 1 2 0V82h-28.1zm16-64a5 5 0 1 1 0-2H146v44.1a5 5 0 1 1-2 0V18h-26.1zm102.2 270a5 5 0 1 1 0 2H98v14h-2v-16h124.1zM242 149.9V160h16v34h-16v62h48v48h-2v-46h-48v-66h16v-30h-16v-12.1a5 5 0 1 1 2 0zM53.9 18a5 5 0 1 1 0-2H64V2H48V0h18v18H53.9zm112 32a5 5 0 1 1 0-2H192V0h50v2h-48v48h-28.1zm-48-48a5 5 0 0 1-9.8-2h2.07a3 3 0 1 0 5.66 0H178v34h-18V21.9a5 5 0 1 1 2 0V32h14V2h-58.1zm0 96a5 5 0 1 1 0-2H137l32-32h39V21.9a5 5 0 1 1 2 0V66h-40.17l-32 32H117.9zm28.1 90.1a5 5 0 1 1-2 0v-76.51L175.59 80H224V21.9a5 5 0 1 1 2 0V82h-49.59L146 112.41v75.69zm16 32a5 5 0 1 1-2 0v-99.51L184.59 96H300.1a5 5 0 0 1 3.9-3.9v2.07a3 3 0 0 0 0 5.66v2.07a5 5 0 0 1-3.9-3.9H185.41L162 121.41v98.69zm-144-64a5 5 0 1 1-2 0v-3.51l48-48V48h32V0h2v50H66v55.41l-48 48v2.69zM50 53.9v43.51l-48 48V208h26.1a5 5 0 1 1 0 2H0v-65.41l48-48V53.9a5 5 0 1 1 2 0zm-16 16V89.41l-34 34v-2.82l32-32V69.9a5 5 0 1 1 2 0zM12.1 32a5 5 0 1 1 0 2H9.41L0 43.41V40.6L8.59 32h3.51zm265.8 18a5 5 0 1 1 0-2h18.69l7.41-7.41v2.82L297.41 50H277.9zm-16 160a5 5 0 1 1 0-2H288v-71.41l16-16v2.82l-14 14V210h-28.1zm-208 32a5 5 0 1 1 0-2H64v-22.59L40.59 194H21.9a5 5 0 1 1 0-2H41.41L66 216.59V242H53.9zm150.2 14a5 5 0 1 1 0 2H96v-56.6L56.6 162H37.9a5 5 0 1 1 0-2h19.5L98 200.6V256h106.1zm-150.2 2a5 5 0 1 1 0-2H80v-46.59L48.59 178H21.9a5 5 0 1 1 0-2H49.41L82 208.59V258H53.9zM34 39.8v1.61L9.41 66H0v-2h8.59L32 40.59V0h2v39.8zM2 300.1a5 5 0 0 1 3.9 3.9H3.83A3 3 0 0 0 0 302.17V256h18v48h-2v-46H2v42.1zM34 241v63h-2v-62H0v-2h34v1zM17 18H0v-2h16V0h2v18h-1zm273-2h14v2h-16V0h2v16zm-32 273v15h-2v-14h-14v14h-2v-16h18v1zM0 92.1A5.02 5.02 0 0 1 6 97a5 5 0 0 1-6 4.9v-2.07a3 3 0 1 0 0-5.66V92.1zM80 272h2v32h-2v-32zm37.9 32h-2.07a3 3 0 0 0-5.66 0h-2.07a5 5 0 0 1 9.8 0zM5.9 0A5.02 5.02 0 0 1 0 5.9V3.83A3 3 0 0 0 3.83 0H5.9zm294.2 0h2.07A3 3 0 0 0 304 3.83V5.9a5 5 0 0 1-3.9-5.9zm3.9 300.1v2.07a3 3 0 0 0-1.83 1.83h-2.07a5 5 0 0 1 3.9-3.9zM97 100a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0-16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-48 32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm32 48a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm32-16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0-32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm32 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0-16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16-64a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 96a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16-144a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0 32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16-32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16-16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-96 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16-32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm96 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16-64a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16-16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-32 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0-16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zM49 36a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-32 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm32 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zM33 68a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16-48a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0 240a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16-64a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16-32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm80-176a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16-16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm32 48a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16-16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0-32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm112 176a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm-16 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zM17 180a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0 16a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm0-32a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16 0a3 3 0 1 0 0-6 3 3 0 0 0 0 6zM17 84a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm32 64a3 3 0 1 0 0-6 3 3 0 0 0 0 6zm16-16a3 3 0 1 0 0-6 3 3 0 0 0 0 6z'%3E%3C/path%3E%3C/svg%3E");
			background-repeat: inherit;
		}
	</style>
	<section class="bg">
		<div class="container py-5 h-100">
			<div class="col d-flex justify-content-center align-items-center h-100">
				<div class="col-md-8 col-lg-6 col-xl-4">
					<div class="card" style="color: #4b515d; border-radius: 35px">
						<div class="card-body p-4">
							<div class="d-flex">
								<h6 class="flex-grow-1">Room 1</h6>
								<h6 id="Time">15:07</h6>
								&emsp;
								<h6 id="Date">Wed, 20</h6>
							</div>

							<div class="d-flex flex-column text-center mt-5 mb-4">
								<h6 class="display-4 mb-0 font-weight-bold" id="temperature" style="color: #1c2331">
									30°C
								</h6>
								<span class="small" style="color: #868b94" id="desc">Stormy</span>
							</div>

							<div class="d-flex align-items-center">
								<div class="flex-grow-1" style="font-size: 1rem">
									<div>
										<i class="fas fa-wind fa-fw" style="color: #868b94"></i>
										<span class="ms-1" id="wind">
										</span>
									</div>
									<div>
										<i class="fas fa-tint fa-fw" style="color: #868b94"></i>
										<span class="ms-1" id="humidity">
											46%
										</span>
									</div>
									<div>
										<i class="uil uil-sunset" style="color: #868b94"></i>
										<span class="ms-1" id="sunset">
										</span>
									</div>
								</div>
								<div>
									<img id="weatherImg" src="" width="100px" />
								</div>
							</div>
						</div>
					</div>
					<div class="card" style="color: #4b515d; border-radius: 35px; margin-top: 30px;">
						<div class="card-body p-4">
							<div class="d-flex">
								<h6 class="flex-grow-1">Room 2</h6>
								<h6 id="Time">15:07</h6>
								&emsp;
								<h6 id="Date">Wed, 20</h6>
							</div>

							<div class="d-flex flex-column text-center mt-5 mb-4">
								<h6 class="display-4 mb-0 font-weight-bold" id="temperature2" style="color: #1c2331">
									30°C
								</h6>
								<span class="small" style="color: #868b94" id="desc">Stormy</span>
							</div>

							<div class="d-flex align-items-center">
								<div class="flex-grow-1" style="font-size: 1rem">
									<div>
										<i class="fas fa-wind fa-fw" style="color: #868b94"></i>
										<span class="ms-1" id="wind">
										</span>
									</div>
									<div>
										<i class="fas fa-tint fa-fw" style="color: #868b94"></i>
										<span class="ms-1" id="humidity2">
											46%
										</span>
									</div>
									<div>
										<i class="uil uil-sunset" style="color: #868b94"></i>
										<span class="ms-1" id="sunset">
										</span>
									</div>
								</div>
								<div>
									<img id="weatherImg" src="" width="100px" />
								</div>
							</div>
						</div>
					</div>
					<div class="card" style="color: #4b515d; border-radius: 35px; margin-top: 30px;">
						<div class="card-body p-4">
							<div class="d-flex">
								<h6 class="flex-grow-1">Lights</h6>
								<h6 id="Time">15:07</h6>
								&emsp;
								<h6 id="Date">Wed, 20</h6>
							</div>
							<div class="container bg-light text-center">
								<h5>Room No. 1</h5>
								<div class="col-md-12 text-center">
									<button type="button" class="btn btn-success" onclick="btn1On()">ON</button>
									<button type="button" class="btn btn-danger" onclick="btn1Off()">OFF</button>
								</div>
								<span class="ms-1">Current Status : </span>
								<span class="ms-1" id="btnR1"> OFF </span>
							</div>
							<br>
							<div class="container bg-light text-center">
								<h5>Room No. 2</h5>
								<div class="col-md-12 text-center">
									<button type="button" class="btn btn-success" onclick="btn2On()">ON</button>
									<button type="button" class="btn btn-danger" onclick="btn2Off()">OFF</button>
								</div>
								<span class="ms-1">Current Status : </span>
								<span class="ms-1" id="btnR2"> OFF </span>
							</div>
						</div>
					</div>
					<div class="card" style="color: #4b515d; border-radius: 35px; margin-top: 30px;">
						<div class="card-body p-4">
							<div class="d-flex">
								<h6 class="flex-grow-1">Lights</h6>
								<h6 id="Time">15:07</h6>
								&emsp;
								<h6 id="Date">Wed, 20</h6>
							</div>
							<figure class="highcharts-figure">
								<div id="container"></div>

							</figure>
						</div>
					</div>
				</div>
			</div>
		</div>

	</section>

	<script src="https://code.highcharts.com/highcharts.js"></script>
	<script src="https://code.highcharts.com/modules/series-label.js"></script>
	<script src="https://code.highcharts.com/modules/exporting.js"></script>
	<script src="https://code.highcharts.com/modules/export-data.js"></script>
	<script src="https://code.highcharts.com/modules/accessibility.js"></script>
	<script>

		var chartHumTemp = Highcharts.chart('container', {
			chart: {
				type: 'spline'
			},
			title: {
				text: 'Room Temperature'
			},
			subtitle: {
				text: 'Source: ESP32, DHT11'
			},
			xAxis: {
				categories: [],
				accessibility: {
					description: 'Time'
				}
			},
			yAxis: {
				title: {
					text: 'Temperature'
				},plotBands: [{ 
                    from: 0,
                    to: 30,
                    color: 'rgba(68, 170, 213, 0.1)',
                    label: {
                        text: 'AC OFF',
                        style: {
                            color: '#606060'
                        }
                    }
                }, { 
                    from: 30,
                    to: 60,
                    color: 'rgba(0, 0, 0, 0)',
                    label: {
                        text: 'AC ON',
                        style: {
                            color: '#606060'
                        }
                    }
                },],
				labels: {
					formatter: function () {
						return this.value + '°';
					}
				}
			},
			tooltip: {
				crosshairs: true,
				shared: true
			},
			plotOptions: {
				spline: {
					marker: {
						radius: 4,
						lineColor: '#666666',
						lineWidth: 1
					}
				}
			},
			series: [{
				name: 'Room 1',
				marker: {
					symbol: 'square'
				},
				data: []

			}, {
				name: 'Room 2',
				marker: {
					symbol: 'diamond'
				},
				data: []
			}]
		});

		/* Web Socket Connection Code -- ARDUINO */
		var connection = new WebSocket(
			'ws://' + location.hostname + ':81/'
		);
		console.log(location.hostname);
		connection.onmessage = function (event) {
			var fullData = event.data;
			console.log(fullData);

			var data = JSON.parse(fullData);

			document.getElementById('temperature').innerHTML = data.tempRoom1 + '°C';
			document.getElementById('humidity').innerHTML = data.humRoom1 + '%';

			document.getElementById('temperature2').innerHTML = data.tempRoom1 + '°C';
			document.getElementById('humidity2').innerHTML = data.humRoom1 + '%';

			const btn1 = document.getElementById('btnR1');
			const btn2 = document.getElementById('btnR2');

			if (data.LED1) {
				btn1.innerHTML = "ON";
			}
			else {
				btn1.innerHTML = "OFF";
			}

			if (data.LED2) {
				btn2.innerHTML = "ON";
			}
			else {
				btn2.innerHTML = "OFF";
			}

			var today = new Date();
			var date = today.getFullYear() + '-' + (today.getMonth() + 1) + '-' + today.getDate();
			var time = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();

			chartHumTemp.series[0].addPoint([time, data.tempRoom1])
			chartHumTemp.series[1].addPoint([time, data.tempRoom1 - 2])


			//      const HTTP = new XMLHttpRequest();
			//      const URL = `https://script.google.com/macros/s/AKfycbzivi2j-gFeIyn7elFgdmJjqDNv2gZhdErSD6uVfUmHLHryGQiFh6OuhXp5UuYy1NMa/exec/exec?time=${date + ' ' + time}&temperature=${data.tempRoom1}&humidity=${data.humRoom1}`;
			//      HTTP.open("GET", URL);
			//      HTTP.send();
		};

		var btn_1_status = 0;
		var btn_2_status = 0;

		const btn1 = document.getElementById('btnR1')
		const btn2 = document.getElementById('btnR2')

		function btn1On() {
			btn_1_status = 1;
			btn1.innerHTML = 'ON'
			send_data(btn_1_status);
		}

		function btn1Off() {
			btn_1_status = 0;
			btn1.innerHTML = 'OFF'
			send_data(btn_1_status);
		}

		function btn2On() {
			btn_2_status = 1;
			btn2.innerHTML = 'ON'
			send_data(btn_2_status);
		}

		function btn2Off() {
			btn_2_status = 0;
			btn2.innerHTML = 'OFF'
			send_data(btn_2_status);
		}

		function send_data() {
			var fullData = `{LED1 : ${btn_1_status}, LED2: ${btn_2_status}}`
			// console.log(fullData);
			connection.send(fullData);
		}

		const currTime = document.querySelectorAll('#Time');
		const currDate = document.querySelectorAll('#Date');
		const windSpeed = document.querySelectorAll('#wind');
		const weatherDesc = document.querySelectorAll('#desc');
		const sunsetTime = document.querySelectorAll('#sunset');
		const weatherImg = document.querySelectorAll('#weatherImg');

		const weekday = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];

		var today = new Date();

		hh = today.getHours();
		mm = today.getMinutes();
		hh = hh < 10 ? '0' + hh : hh;
		mm = mm < 10 ? '0' + mm : mm;

		for (let i = 0; i < currTime.length; i++) {
			currTime[i].innerHTML = `${hh}:${mm}`;
			currDate[i].innerHTML = `${weekday[today.getDay()]}, ${today.getDate()}`;
		}
		const url =
			'https://api.openweathermap.org/data/2.5/weather?q=Nagpur,india&appid=4f3ccf7eff9976e2fedac54dcf90129a&units=metric';

		async function getWeatherData(url) {
			const response = await fetch(url);
			var data = await response.json();

			// console.log(response);
			for (let i = 0; i < currTime.length - 2; i++) {

				var sunset = new Date(data.sys.sunset * 1000);
				sunsetTime[i].innerHTML = `${sunset.getHours()}:${sunset.getMinutes()}`;

				windSpeed[i].innerHTML = `${data.wind['speed']} km/hr`;
				weatherDesc[i].innerHTML = data.weather[0].main;

				icon = data.weather[0].icon;

				weatherImg[i].src = `http://openweathermap.org/img/wn/${icon}@2x.png`;
			}
		}

		getWeatherData(url);
	</script>

</body>

</html>
)=====";

AsyncWebServer server(80);
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Sorry! The page you're looking for does not exist");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case WStype_DISCONNECTED:
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
  {
    IPAddress ip = websockets.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
  }
  break;
  case WStype_TEXT:
    Serial.printf("[%u] get Text: %s\n", num, payload);
    String message = String((char *)(payload));
    Serial.println(message);

    DynamicJsonDocument doc(200);
    // deserialize the data
    DeserializationError error = deserializeJson(doc, message);
    // parse the parameters we expect to receive (TO-DO: error handling)
    // Test if parsing succeeds.
    if (error)
    {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    int LED1_Status = doc["LED1"];
    int LED2_Status = doc["LED2"];

    // Serial.println(LED1_Status);
    // Serial.println(LED2_Status);

    L1 = LED1_Status;
    L2 = LED2_Status;

    //		sendSensor(LED1_Status, LED2_Status);

    digitalWrite(LED1, LED1_Status);
    digitalWrite(LED2, LED2_Status);
  }
}

const char *ssid = "JIOFIBER-MANE LOW";
const char *password = "RNDM@20RADIOWAVE20";

//#define ACPin 12

void setup()
{
  //	pinMode(ACPin, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  Serial.begin(115200);
  dhtRoom1.begin();
  dhtRoom2.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  if (MDNS.begin("rndmESP"))
  {
    Serial.println("MDNS Responder Started! Head to rndmesp.local");
  }

  server.on("/", [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", homePage); });

  server.onNotFound(notFound);

  server.begin(); // it will start webserver
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  timer.attach(5, sendSensor);
}

void loop()
{
  // put your main code here, to run repeatedly:
  websockets.loop();
}

void sendSensor()
{
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dhtRoom1.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dhtRoom1.readTemperature();

  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h2 = 65; // dhtRoom2.readHumidity();
                 // Read temperature as Celsius (the default)
  float t2 = 25; // dhtRoom2.readTemperature();

  int ac_status = 0;

  if (isnan(h) || isnan(t))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // JSON_Data = {"temp":t,"hum":h, "ac": ac_status}
  String JSON_Data = "{\"tempRoom1\":";
  JSON_Data += t;
  JSON_Data += ",\"humRoom1\":";
  JSON_Data += h;
  JSON_Data += ",\"tempRoom2\":";
  JSON_Data += t2;
  JSON_Data += ",\"humRoom2\":";
  JSON_Data += h2;
  JSON_Data += ",\"LED1\":";
  JSON_Data += L1;
  JSON_Data += ",\"LED2\":";
  JSON_Data += L2;
  JSON_Data += ",\"ac\":";
  JSON_Data += ac_status;
  JSON_Data += "}";
  Serial.println(JSON_Data + "\n");
  //	controlAC(ac_status, t);
  websockets.broadcastTXT(JSON_Data);
}

// void sendLEDStatus(int L1, int L2)
//{
//   String LED_Data = "{\"LED1\":";
//   LED_Data += L1;
//   LED_Data += ",\"LED2\":";
//   LED_Data += L2;
//   LED_Data += "}";
//
//   websockets.broadcastTXT(LED_Data);
// }

// void controlAC(int ac_status, int temp)
//{
//	//	if (status == 0 && temp >= 32)
//	//	{
//	//		digitalWrite(ACPin, HIGH);
//	//	}
//	//	else if (status == 1 && temp <= 32)
//	//	{
//	//		digitalWrite(ACPin, LOW);
//	//	}
//
//	Serial.printf("\ntemperature : %d, status : %d", temp, ac_status);
// }
