# Self Driving Patrol Car

* The project is based on the ***Ubuntu 18.04*** LTS 64bit on the localhost

* Using the default gcc/g++(Ubuntu 7.5.0-3ubuntu1~18.04), arm-none-eabi-gcc(15:6.3.1+svn253039-1build1)

* Need to CMake version 3.11 or higher


## Main configuration of the HW platform

* Core Board: ***LattePanda Alpha 864s*** (based on the Ubuntu 18.04) [Click the Link](https://www.lattepanda.com/products/lattepanda-alpha-864s.html)

* MCU: ***myCortex-STM32F4*** (based on the STM32F407VE, ARM Cortex-M4) [Click the Link](http://withrobot.com/embeddedboard/mycortex-stm32f4/)

* Camera: USB PC Camera ***SC-FD110W***

* Graphic Card: GIGABYTE GeForce ***GTX1080*** Ti Gaming D5X 11GB


## Prerequisite

* Before the build the project, you have to install the packages below

```bash
$ bash scm/scripts/common/install_package.sh
```


## How to build the project

* Please refer to the ***build.sh*** in the each directories

* After execute the build.sh, you can check the results in the ***out*** folder


## How to flash the images and execute the project on the develop board

* TODO


## Introduce the directories

* ***core***

  * The control block for all systems

  * Using the skills: C, bash, CMake

  * Contact: ***Kim, Jinseong(Project Leader, zokj0691@gmail.com)***


* ***features***

  * ***lane_detection***

    * For lane detection using the camera

    * Using the skills: C++17, OpenCV, bash, CMake

    * Contact: ***Park, Sangjae(qkrtkdwo76@gmail.com) / Bae, Youwon(dbdnjs9804@gmail.com)***

  * ***mcu_control***

    * For control the sensor and motor

    * Using the skills: C, Matlab(for sensor fusion), bash, make

    * Contact: ***Roh, Hojin(noj7413@naver.com)***

  * ***web_gui***

    * For GUI on the Web

    * Using the skills: Python3, Flask

    * Contact: ***Kim, Jongwook(kind1547@gmail.com)***


* ***scm***

  * For source configuration management

  * Using the skills: Git, Gitlab(CI/CD using the Runner), Markdown, Docker, bash

  * Contact: ***Lee, Jaeseong(jslee24x@gmail.com)***
