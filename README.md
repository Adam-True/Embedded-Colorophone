# Embedded Colorophone 

This repository is a shared repository for the common developpement of the audio and video parts. After each part has been developped with success, it is time to merge the two parts into one project. In this way further developpement of the audio and video parts are possible. The current project is written in C but the longterm goal is to port the code to C++ in order to benefit from the Object Oriented concepts. In this way the project can be decoupled as much as possible from the underlying firmware and RTOS. The current UML and goal UML class diagrams are shown below. 

## Documentation 
Documentation about the code that has been written to implement the audio generation and video processing parts can be found [here](https://xbz667.bitbucket.io)

### Hardware 

* [STM32F769NI]() - MCU
* [STM32F769I-Discovery]() - Board
* [Logitech C920]() - Webcam

### Software

* [SW4STM32](https://www.st.com/en/development-tools/sw4stm32.html) - IDE
* [FreeRTOS](https://www.freertos.org/) - Real Time OS
* [STM32CubeF7](https://www.st.com/en/embedded-software/stm32cubef7.html) - Firmware for the MCU
* [Percepio Tracealyzer 4](https://percepio.com/2017/11/02/welcome-first-look-tracealyzer-4/) - Tracing Software

