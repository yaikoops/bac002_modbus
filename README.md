# BAC-002 thermostat & SMD120 energymeter Modbus to MQTT Gateway
This sketch is designed to provide an MQTT gateway to BAC-002 Modbus thermostats & SDM120 Modbus ernergymeter . 
It uses a ESP8266 (in my case Wemos D1 R1) and an RS485 to TTL converter(MAX485) to read the Holding Registers of the thermostat & energymeter and publish those values to MQTT. 
Also subscirbes to MQTT and relevant updates are written into only the Modbus thermostat 

## Libraries used
- Modbus Master library: https://github.com/4-20ma/ModbusMaster/blob/master/examples/RS485_HalfDuplex/RS485_HalfDuplex.ino
- ESP Software Serial: https://github.com/plerup/espsoftwareserial
- PubSubClient: handle MQTT communication. Install it from the Arduino Library Manager

## Wiring
```
Wemod D1 R1 (or other ESP)            TTL to RS485
D2                                      DE
D3                                      RE
D8                                      RO
D9                                      DI
3.3V                                    VCC
G(GND)                                GND
```
Red Modbus wire goes to the RS486 "A" screw terminals and black to "B".

## MQTT topic
Root topic is defined in the beginning of the sketch and the full topic gets appended to this. 
I specified "/myfarm/" as my root, so my topic are e.g. "/myfarm/read/power". 
Values read from the device have "read" in the topic, and values sent to the thermostat & energymeter have "write". 
Sending messages to the "read" topics get ignored.
There is no error handling on the payload of the "write" messages, data received gets sent to the device.
### BAC-002 thermostat Read topics (Published topics)
- root + "read/power": device status, Off (0) or On (1)
- root + "read/fanspeed": fan speed, Auto (0), High (1), Medium (2), Low (3)
- root + "read/settingmode": mode, Cooling (0), Heating (1), Ventillation (2)
- root + "read/settingtemp": 
- root + "read/settinglock": screen lock, Unlocked (0), Locked (1)
- root + "read/minute": minute and time on the device in MM format
- root + "read/hour": hour and time on the device in HH24 format
- root + "read/week": weekday on the device: Monday (1), ... Sunday (7)
- root + "read/roomtemp": 
- root + "read/heaterstatus": valve state, Closed (0), Open (1)
- root + "read/fanstatus": 
### SDM1200 energymeter Read topics (Published topics)
- root + "read/voltage": 
- root + "read/current": 
- root + "read/active_power": 
- root + "read/apparent_power": 
- root + "read/reactive_power": 
- root + "read/power_factor": 
- root + "read/phase_angle": 
- root + "read/frequency": 
- root + "read/import_active_energy": 
- root + "read/export_active_energy": 
- root + "read/import_reactive_energy": 
- root + "read/export_reactive_energy": 
- root + "read/total_active_energy": 
- root + "read/total_reactive_energy": 
### BAC-002 thermostatWrite topics (Subscribed topics)
- root + "write/bac002/setpower": set device status (same values as above)
- root + "write/bac002/setfan": set fan speed (same values as above)
- root + "write/bac002/setmode": set mode (same values as above)
- root + "write/bac002/settemp" : set screen lock  (same values as above)
- root + "write/bac002/setlock": set target temperature in degrees centigrade, e.g. 20.5
- root + "write/bac002/setminute": set target temperature in degrees centigrade, e.g. 20.5
- root + "write/bac002/sethour": set target temperature in degrees centigrade, e.g. 20.5
- root + "write/bac002/setweek": set target temperature in degrees centigrade, e.g. 20.5
### Error topic (published)
- root + "error": Modbus error message in text (e.g. "Response timed out")

## Test board shown in the video

## Node-Red
There is an example Node-Red flow (also shown in the video), which shows how to communicate with the device from Node-Red. The flow is in Gitbuh, and also here: https://flows.nodered.org/flow/286a8cea2fbb2a60544f0dd2faefda2a

## PCB

## Custom Case
