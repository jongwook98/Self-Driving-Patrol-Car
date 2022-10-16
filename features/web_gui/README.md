# Web Server using the FastAPI


## Prerequisite

  * We use Ubuntu 18.04, so we need to install Python 3.8 or higher

  ```bash
  $ sudo apt install python3.8 python3.8-dev python3.8-pip python3.8-setuptools
  ```

  * Upgrade the pip

  ```bash
  $ sudo python3.8 -m pip install -U pip
  ```

  * Setup the virtual environment

  ```bash
  $ python3.8 -m venv .env
  $ source .env/bin/activate
  (.env) $ pip install -U pip
  (.env) $ pip install -r requirements.txt
  ```


## How to start the web server

  * If not excute the virtual envrionment,

  ```bash
  $ source .env/bin/activate
  ```

  ```bash
  (.env) $ sudo -E python3 main.py
  INFO:     Started server process [6166]
  INFO:     Waiting for application startup.
  INFO:     Application startup complete.
  INFO:     Uvicorn running on http://0.0.0.0:8000 (Press CTRL+C to quit)
  ```

  * After that, you can access the ***localhost:8000*** on the browser

  * Also, you can access the ***localhost:8000/docs, localhost:8000/redocs***

  * If need to exit the web server, input the ***ctrl + c***
