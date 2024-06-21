Simple implementation of the boid algorithm by Craig Reynolds, written in C using raylib.

You can check it out at https://blast1fm.github.io/boids/

Building the project:

0. Compile and install raylib

1. Compile the project 
  * Desktop - make RAYLIB_PATH=/your/path/to/raylib

  * Web (WASM) - Make sure you have set up the emscripten toolchain and compiled raylib for web (A written guide can be found on the [raylib wiki](https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5))
    * Either hardcode the correct paths to raylib and emsdk into the Makefile and use ```make PLATFORM=PLATFORM_WEB -B``` , or pass them to make yourself
    * The resulting game can then be hosted locally with ```emrun boids.html```
