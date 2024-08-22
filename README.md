# ver-wer-pe-lijk
Repository for our interactive system software to enhance trainings for disabled atheletes. 

It lets you play games with up to 9 LED rings, where every ring has a LED strip, power supply and ESP8266.
You also need one ESP8266 connected to a PC as this will act as the controller through the ESPNOW wireless protocol.

# SETUP

<p>To setup the project you will need multiple ESP8266's, The project can handle up to 10.<br>
Find out all of the ESP mac addresses using the 'macaddress' sketch in the test folder of this repo.<br>
Hereafter, you can choose one to be the master and put it's macaddres in the 'light_rings' sketch as receiver address.<br>
All the other mac adresses will go into the 'controller ring' file into the receiver address array there</p>
