Simple implementation of the boid algorithm by Craig Reynolds, written in C using raylib.

A (probably outdated) version of this project can be found on my [itch.io page](https://blastfm.itch.io/boids)

Building the project:

0. Compile and install raylib

1. Compile the project 
  * Desktop - Either use the Makefile (make sure to configure your paths correctly), or if you're on linux, use the provided shell script build.sh for an easy compilation

  * Web (WASM) - Make sure you have set up the emscripten toolchain and compiled raylib for web (A written guide can be found on the [raylib wiki](https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5))
    * Either hardcode the correct paths to raylib, emsdk and python into the Makefile and use ```make PLATFORM=PLATFORM_WEB -B``` , or pass them to the make command
    * The resulting game can then be hosted locally with ```emrun boids.html```
