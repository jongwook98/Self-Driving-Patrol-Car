# How to set up the WSL2 and docker on the Windows

## What is WSL?
* Windows Subsystem for Linux(WSL) is a compatibility layer for running Linux binary executables (in ELF format) natively on Windows 10 and Windows Server 2019.
* Please refer to the link below
	- <https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux>

## The architecture difference between WSL and WSL2
* ***WSL Architecture***
![LXSS-diagram-1024x472](https://user-images.githubusercontent.com/54479819/90337376-13dac800-e01d-11ea-8ccf-304e10999d3f.jpg)

* ***WSL2 Archtecture***
![WSL-2-Architecture](https://user-images.githubusercontent.com/54479819/90337397-2f45d300-e01d-11ea-9c6a-b9bb6726fc8b.jpg)

## How to install the WSL2
1. Check the Windows Version
	* The first thing you need to do is make sure you’re running at least ***Windows 10 20H1 Build 18917***
	* Type **Winver** into the *search box(window key + r)* and hit enter
	![1_window_version](https://user-images.githubusercontent.com/54479819/90379900-4e447380-e0b6-11ea-9b77-39e4ee04e136.png)

2. Install the ***Windows Terminal***
	* In the Microsoft Store, search the ***Windows Terminal*** and install that application

3. Commands for enabling the WSL2
	* Execute the Windows Terminal as the **administrator**
	* Enable the ***Windows Subsystem for Linux*** options feature
	```bash
	c: > dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
	```

	* Enable the ***Virtual Machine Platform*** options feature
	```bash
	c: > dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
	```
4. Install the WSL(Ubuntu)
	* In the Microsoft Store, search the ***Ubuntu 18.04*** and install that application
	* On the *Windows Terminal*, input the ***wsl*** command
	```bash
	c: > wsl -l
	```
	![2_wsl_l](https://user-images.githubusercontent.com/54479819/90379958-6a481500-e0b6-11ea-8d83-3d2d509866ae.png)

5. Update from the WSL to the WSL2
	* Check the current version the WSL
	```bash
	c: > wsl -l -v
	```

	* Update to the WSL2
	```bash
	c: > wsl --set-version Ubuntu-18.04 2
	```

	* Setup the default WSL version
	```bash
	c: > wsl --set-default-version 2
	```

	* Restart the WSL and check the version
	```bash
	c: > wsl -t Ubuntu-18.04
	c: > wsl -l -v
	```
	![3_wsl_v](https://user-images.githubusercontent.com/54479819/90380003-7a5ff480-e0b6-11ea-8a4a-a4430daa221c.png)

## How to install the Docker on the Windows
1. Install the ***Docker Desktop for Windows***
	* Please refer to the link below
		- <https://hub.docker.com/editions/community/docker-ce-desktop-windows>

2. Enable the ***Use the WSL2 based engine*** on the docker
![4_docker_setting_1](https://user-images.githubusercontent.com/54479819/90381015-c2cbe200-e0b7-11ea-9305-e655afb83955.png)

3. Check the WSL2
![5_docker_setting_2](https://user-images.githubusercontent.com/54479819/90381081-d6774880-e0b7-11ea-8884-8addb1a367f1.png)

4. Check the docker command on the WSL2
![6_docker_execute](https://user-images.githubusercontent.com/54479819/90381120-e2fba100-e0b7-11ea-9f58-10674d1781aa.png)

5. Enjoy :)
