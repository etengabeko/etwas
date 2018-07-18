# Build steps

* **Linux**

  Tested on Ubuntu 18.04 x64 (gcc 7.3.0 + Qt 5.9.5) and AstraLinux 1.5 x64 (gcc 4.7.2, Qt 5.5.1)
  
  ```
  cd <path-to-project-src>
  cmake -Bbuild -H. -DCMAKE_INSTALL_PREFIX=<path-to-install> -DCMAKE_BUILD_TYPE=<build-type>
  cmake --build build
  cmake --build build --target-install
  ```
  
* **Windows**

  Tested on Windows 7 x64 (Visual Studio 12 and MinGW 6.3.0, Qt 5.10.0)

  * *Visual Studio*:
  ```
  cd <path-to-project-src>
  cmake -G"Visual Studio 12 [arch]" -Bbuild -H. -DCMAKE_INSTALL_PREFIX=<path-to-install>
  cmake --build build --config <build-type>
  cmake --build build --config <build-type> --target install
  ```
  
  * *MinGW*:
  ```
  cd <path-to-project-src>
  cmake -G"MinGW Makefiles" -Bbuild -H. -DCMAKE_INSTALL_PREFIX=<path-to-install> -DCMAKE_BUILD_TYPE=<build-type>
  cmake --build build
  cmake --build build --target install
  ```
  
**Options**:

`<build-type>`: `Release` or `Debug` (`Release` by default)

`[arch]` may be `Win64` for build on x64 platform

More of CMake generators run `cmake --help`

