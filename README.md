# ![Logo](data/favicon.svg) P1-meter

This project reads "telegrams" from a P1 meter and POSTs them to a given HTTP endpoint for further processing. No fancy schmancy MQTT stuff or anything. Plain, simple, pure HTTP POST. That's it.

## Getting started

1. Build the circuit shown under [hardware](#hardware)
2. Follow the instructions in [include/example_config.h](include/example_config.h) and [example_env_secrets.ini](example_env_secrets.ini).
3. Flash the Wemos D1 Mini with this software
4. Power the Wemos D1 mini and connect it to your meter
5. Connect to the WiFi Access Point it creates (`p1-meter` or a name you provided) and configure your WiFi
6. You should now see POST requests at your configured HTTP endpoint

Where you go from here is up to you. Personally, I use [DSMRParser.Net](https://github.com/RobThree/DSMRParser.Net) hosted in a C# WebAPI project to handle the POST requests and check the checksum, parse the telegram etc. and then hand the parsed data to InfluxDB so I can visualize my data with Grafana. But you may have totally different plans. And that's OK. All this project requires is that you handle a POST request that contains the raw, unsanitized, unchecked and unvalidated telegram data.

## Hardware

This project is based on a Wemos D1 mini. The cicruit is pretty simple: 

![Circuit](/doc/schema.png)

![Circuit](/doc/circuit.png)

It has been a while since I built this project and I'm not sure if this is the best way to do this. But it works for me:

![Works On My Machine!](/doc/womm.png)

This was a 'weekend project' many moons ago. I have polished it *a little* recently, but don't expect too much.