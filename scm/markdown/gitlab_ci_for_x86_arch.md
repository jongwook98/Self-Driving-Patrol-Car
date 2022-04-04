# How to setup the gitlab CI

## How to setup the Host server
* I use the host server as the GCP(Google Cloud Platform)
* To execute the Gitlab-Runner as the Docker needs to install the Docker in the host
```bash
$ sudo apt update
$ sudo apt install -y docker.io
$ sudo usermod -aG docker $USER
```
## How to build the docker image for the CI
```bash
$ cd $(git rev-parse --show-toplevel)
$ scm/docker/docker.sh build
```
* After that, you can check the new docker image for the CI
```bash
$ docker images
REPOSITORY                    TAG                 IMAGE ID            CREATED             SIZE
pycounter                     ci                  < ID Value >        2 days ago          578MB
ubuntu                        18.04               < ID Value >        2 weeks ago         64.2MB
```
* If you want to connect the new docker image,
```bash
$ scm/docker/docker.sh run
root@<container id>:/#
```

## How to write the .gitlab-ci.yml file
* Please refer to the link below
	- <https://docs.gitlab.com/ee/ci/yaml/README.html>

## How to setup the gitlab runner on the host
* gitlab-runner install command,
```bash
$ curl -L https://packages.gitlab.com/install/repositories/runner/gitlab-runner/script.deb.sh | sudo bash
$ sudo apt install gitlab-runner
```

* Start the *gitlab-runner* on the GCP
```bash
jslee24x@rich_flexin:~$ sudo gitlab-runner register
Runtime platform    arch=amd64 os=linux pid=xxx revision=xxx version=12.10.2
Running in system-mode.

Please enter the gitlab-ci coordinator URL (e.g. https://gitlab.com/):
https://gitlab.com/
Please enter the gitlab-ci token for this runner:
gAFGtLzuJg9Cqjh4Dq4y
Please enter the gitlab-ci description for this runner:
[rich_flexin]: gitlab-runner for the pycounter
Please enter the gitlab-ci tags for this runner (comma separated):
pycounter
Registering runner... succeeded                     runner=KeVnfRwB
Please enter the executor: custom, docker, docker-ssh, parallels, kubernetes, shell, ssh, virtualbox, docker+machine, docker-ssh+machine:
docker
Please enter the default Docker image (e.g. ruby:2.6):
pycounter:ci
Runner registered successfully. Feel free to start it, but if it's running already the config should be automatically reloaded!
```

* Please refer to find the \<My-Token\> as below,
![gitlab_runner_1](https://user-images.githubusercontent.com/54479819/78148837-44f09c80-7470-11ea-871e-851b9e7886df.png)

* After register the *gitlab-runner*, you can see below image,
![gitlab_runner_4](https://user-images.githubusercontent.com/54479819/81576249-fde8b600-93e2-11ea-8166-3aab79d77823.png)

	* In the GCP,
	```bash
	$ sudo gitlab-runner status
	Runtime platform	arch=amd64 os=linux pid=xxx revision=xxx version=12.10.2
	gitlab-runner: Service is running!
	```
	* Also, can check the gitlab-runner configuration file in the GCP,
	```bash
	$ sudo cat /etc/gitlab-runner/config.toml
	concurrent = 1
	check_interval = 0

	[session_server]
	  session_timeout = 1800

	[[runners]]
	  name = "gitlab-runner for the pycounter"
	  url = "https://gitlab.com/"
	  token = "Des1Mvzxnz-S8qLRJLNW"
	  executor = "docker"
	  [runners.custom_build_dir]
	  [runners.cache]
		[runners.cache.s3]
		[runners.cache.gcs]
	  [runners.docker]
		tls_verify = false
		image = "pycounter:ci"
		privileged = false
		disable_entrypoint_overwrite = false
		oom_kill_disable = false
		disable_cache = true
		pull_policy = "if-not-present"
		shm_size = 0
	```

* Also, you can check the gitlab-runner list below command,
```bash
$ sudo gitlab-runner list
Runtime platform                                    arch=amd64 os=linux pid=28108 revision=c5874a4b version=12.10.2
Listing configured runners                          ConfigFile=/etc/gitlab-runner/config.toml
gitlab-runner for the pycounter                     Executor=docker Token=gAFGtLzuJg9Cqjh4Dq4y URL=https://gitlab.com/
```
