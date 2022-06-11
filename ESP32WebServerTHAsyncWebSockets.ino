#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Ticker.h>
#include <ESPAsyncWebServer.h>

#define DHTPIN 4
#define LED 12

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void sendSensor();
Ticker timer;

char homePage[] PROGMEM = R"=====(

<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8" />
		<meta http-equiv="X-UA-Compatible" content="IE=edge" />
		<meta name="viewport" content="width=device-width, initial-scale=1.0" />
		<!-- Font Awesome -->
		<link
			href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css"
			rel="stylesheet"
		/>

		<link
			rel="stylesheet"
			href="https://unicons.iconscout.com/release/v4.0.0/css/line.css"
		/>
		<!-- Google Fonts -->
		<link
			href="https://fonts.googleapis.com/css?family=Roboto:300,400,500,700&display=swap"
			rel="stylesheet"
		/>
		<!-- MDB -->
		<link
			href="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/4.0.0/mdb.min.css"
			rel="stylesheet"
		/>
		<!-- MDB -->
		<script
			type="text/javascript"
			src="https://cdnjs.cloudflare.com/ajax/libs/mdb-ui-kit/4.0.0/mdb.min.js"
		></script>
		<!-- <script src="main.js"></script> -->
		<title>IoT Weather App</title>
	</head>
	<body>
		<section
			class="vh-100"
			style="
				background-image: url(https://images.unsplash.com/photo-1496450681664-3df85efbd29f?crop=entropy&cs=tinysrgb&fm=jpg&ixlib=rb-1.2.1&q=80&raw_url=true&ixid=MnwxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8&auto=format&fit=crop&w=1600);
				background-repeat: no-repeat;
			"
		>
			<div class="container py-5 h-100">
				<div
					class="row d-flex justify-content-center align-items-center h-100"
				>
					<div class="col-md-8 col-lg-6 col-xl-4">
						<div
							class="card"
							style="color: #4b515d; border-radius: 35px"
						>
							<div class="card-body p-4">
								<div class="d-flex">
									<h6 class="flex-grow-1">Nagpur, IN</h6>
									<h6 id="Time">15:07</h6>
									&emsp;
									<h6 id="Date">Wed, 20</h6>
								</div>

								<div
									class="d-flex flex-column text-center mt-5 mb-4"
								>
									<h6
										class="display-4 mb-0 font-weight-bold"
										id="temperature"
										style="color: #1c2331"
									>
										30°C
									</h6>
									<span
										class="small"
										style="color: #868b94"
										id="desc"
										>Stormy</span
									>
								</div>

								<div class="d-flex align-items-center">
									<div
										class="flex-grow-1"
										style="font-size: 1rem"
									>
										<div>
											<i
												class="fas fa-wind fa-fw"
												style="color: #868b94"
											></i>
											<span class="ms-1" id="wind">
											</span>
										</div>
										<div>
											<i
												class="fas fa-tint fa-fw"
												style="color: #868b94"
											></i>
											<span class="ms-1" id="humidity">
												46%
											</span>
										</div>
										<div>
											<i
												class="uil uil-sunset"
												style="color: #868b94"
											></i>
											<span class="ms-1" id="sunset">
											</span>
										</div>
									</div>
									<div>
										<img
											id="weatherImg"
											src=""
											width="100px"
										/>
									</div>
								</div>
							</div>
						</div>
					</div>
				</div>
			</div>
		</section>

		<script>
			var connection = new WebSocket(
				'ws://' + location.hostname + ':81/'
			);
			connection.onmessage = function (event) {
				var fullData = event.data;
				console.log(fullData);

				var data = JSON.parse(fullData);

				document.getElementById('temperature').innerHTML = data.temp + '°C';
				document.getElementById('humidity').innerHTML = data.hum + '%';
			};

			const currTime = document.getElementById('Time');
			const currDate = document.getElementById('Date');
			const windSpeed = document.getElementById('wind');
			const weatherDesc = document.getElementById('desc');
			const sunsetTime = document.getElementById('sunset');
			const weatherImg = document.getElementById('weatherImg');

			const weekday = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];

			var today = new Date();

			hh = today.getHours();
			mm = today.getMinutes();
			hh = hh < 10 ? '0' + hh : hh;
			mm = mm < 10 ? '0' + mm : mm;

			currTime.innerHTML = `${hh}:${mm}`;
			currDate.innerHTML = `${
				weekday[today.getDay()]
			}, ${today.getDate()}`;

			const url =
				'https://api.openweathermap.org/data/2.5/weather?q=Nagpur,india&appid=4f3ccf7eff9976e2fedac54dcf90129a&units=metric';

			async function getWeatherData(url) {
				const response = await fetch(url);
				var data = await response.json();

				console.log(response);

				var sunset = new Date(data.sys.sunset * 1000);
				sunsetTime.innerHTML = `${sunset.getHours()}:${sunset.getMinutes()}`;

				windSpeed.innerHTML = `${data.wind['speed']} km/hr`;
				weatherDesc.innerHTML = data.weather[0].main;

				icon = data.weather[0].icon;

				weatherImg.src = `http://openweathermap.org/img/wn/${icon}@2x.png`;
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
	}
}

const char *ssid = "JIOFIBER-MANE LOW";
const char *password = "RNDM@20RADIOWAVE20";

#define ACPin 12

void setup()
{
	pinMode(ACPin, OUTPUT);
	Serial.begin(115200);
	dht.begin();

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
	float h = dht.readHumidity();
	// Read temperature as Celsius (the default)
	float t = dht.readTemperature();

	int ac_status = 0;

	if (isnan(h) || isnan(t))
	{
		Serial.println(F("Failed to read from DHT sensor!"));
		return;
	}
	// JSON_Data = {"temp":t,"hum":h, "ac": ac_status}
	String JSON_Data = "{\"temp\":";
	JSON_Data += t;
	JSON_Data += ",\"hum\":";
	JSON_Data += h;
	JSON_Data += ",\"ac\":";
	JSON_Data += ac_status;
	JSON_Data += "}";
	Serial.println(JSON_Data);
	websockets.broadcastTXT(JSON_Data);
}

void controlAC(int status, int temp)
{
	if (status == 0 && temp >= 32)
	{
		digitalWrite(ACPin, HIGH);
	}
	else if (status == 1 && temp <= 32)
	{
		digitWrite(ACPin, LOW);
	}
}
