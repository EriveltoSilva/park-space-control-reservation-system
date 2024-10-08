<h1 align="center" style="font-weight: bold;"> Park Space Control and Reservation System 💻</h1>

[![Maintenance](https://img.shields.io/badge/Maintained%3F-no-red.svg)](https://bitbucket.org/lbesson/ansi-colors) &ensp; &nbsp;[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://lbesson.mit-license.org/)&ensp; &nbsp;[![Ask Me Anything !](https://img.shields.io/badge/Ask%20me-anything-1abc9c.svg)](http://www.linkedin.com/in/erivelto-silva-39a61a275) &ensp; &nbsp;![Arduino](https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white)


<p align="center">
 <a href="#started">Getting Started</a> • 
 <a href="#features">Features</a> • 
 <a href="#usedComponents">Used Components</a> • 
 <a href="#preRequisites">Pre-requisites</a> •  
 <a href="#electricalDiagram">Electrical Diagram</a> • 
 <a href="#simulation">Simulation</a> • 
 <a href="#collaborators">Collaborators</a> •
 <a href="#license">License</a> •
 <a href="#keyWords">Key words</a> 
</p>

<p align="left">
  <b>The goal of this project is to develop an automated parking space management system using the Arduino platform. The system aims to optimize the user experience by facilitating the location of available spaces and managing parking lot operations.</b>
</p>

<h2 id="features">⚙️ Project  Features</h2>

- <b>Occupied Space Detection</b>:  LDR (Light Dependent Resistor) sensors are used to detect the presence of vehicles in each space. Information about space availability is displayed in real-time on an LCD display.
- <b>Gate Control</b>:   Servo motors simulate entrance and exit gates for the parking lot. The opening of the entrance gate is triggered manually by buttons. The exit gate opens automatically using an ultrasonic sensor that detects the presence of vehicles, closing after 5 seconds.
- <b>Remote Monitoring</b>:  A web page allows users to view the status of spaces (occupied/free) in real-time. The page also offers the functionality to reserve spaces for a period of 120 seconds..


<h2 id="usedComponents">🗒️ Used Components</h2>

- ESP32
- Arduino Mega
- LDR sensors
- LCD display
- Servo motors
- Ultrasonic sensor
- Jumpers 


<h2 id="preRequisites">🗒️ Pre-requisites</h2>

- Arduino IDE software
- <a href="https://github.com/bblanchon/ArduinoJson/releases/tag/v5.13.3">ArduinoJson V5.13.3</a> 
- <a href="https://github.com/ArtronShop/IOXhop_FirebaseESP32">IOXhop_FirebaseESP32</a> 


<h2 id="started">🚀 Getting started</h2>


1. Clone this repository to your local directory.
2. For ESP32 code open the project in `Software\ESP32_PROGRAM\ESP32_PROGRAM.ino`  in the Arduino IDE.
3. For Arduino Mega code open the project in `Software\Arduino\Arduino.ino`  in the Arduino IDE.
4. Configure the ESP32 board and Arduino board and the correct serial port.
5. Upload the code to the ESP32 and Arduino board.
6. Connect the sensors, LEDs and buzzers (if used) to the 6. Arduino board according to the instructions in the code.
7. The system will be ready to use!



<h2 id="electricalDiagram"> 🖼️ Electrical Diagram</h2>
<img src="./Electronic/Docs/electrical_diagram.png"  alt="Electrical Diagram" />

<h2 id="simulation">Simulation</h2>

[See it simulation working on my youtube channel](#) 


<h2 id="collaborators">🤝 Collaborators</h2>

Special thank you for all people that contributed for this project.

<table>
  <tr>
    <td align="center">
      <a href="https://erivelto-silva-portfolio.vercel.app/">
        <img src="https://github.com/EriveltoSilva.png" width="100px;" alt="Erivelto Silva Profile Picture"/><br>
        <sub>
          <b>Erivelto Silva</b>
        </sub>
      </a>
    </td>
    <td align="center">
      <a href="#">
        <img src="" width="100px;" alt="Osvaldo Napoleão Manuel"/><br>
        <sub>
          <b>Osvaldo Napoleão Manuel</b>
        </sub>
      </a>
    </td>
  </tr>
</table>

<h2 id="license"> LICENSE</h2>

This project is licensed under the [MIT License](LICENSE.txt) © Erivelto Silva 

<h2 id="keyWords">Key Words</h2>

Arduino, park, control, monitoring, reservation, mark, space, vacancy, sensors, software

