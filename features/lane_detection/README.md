# Lane Detection

* C++17, bash, CMake

* Opencv version 4.2.0


## How to build

```bash
$ mkdir -p out
$ cd out && cmake ../
$ make

or

$ ./build.sh
```


## How to execute

  * Execute the normal mode,

  ```bash
  $ cd out
  $ ./lane_detection
  ```

  * When checking the debug message,

  ```bash
  $ ./lane_detection -l 0
  ```

  * Help

  ```bash
  $ ./lane_detection --help
  ```