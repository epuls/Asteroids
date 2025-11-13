# Report

## Overview
1. Uses SDL3's callbacks to handle the application loop
2. Application.h/.cpp are the main drivers of the application
3. Player/Asteroid contain the core logic for those respective objects
4. State_Game.h/.cpp contain the core asteroid logic (with the State_MainMenu handling prompting the player to press space to begin)
5. Particles are rendered to an offscreen framebuffer object and then rendered to a fullscreen quad to allow for the torus tiling in egocentric camera mode


## Limitations
1. Resource management is pretty haphazard
2. Lambdas were used for several math functions that could have been separated (ie forward/right vector calcs)
3. Hardcoded shaders to reduce file io management
4. The particle system itself does use raw pointers (nothing else does) but tried to follow RAII principles
5. A lot of room for improvement on design overall
6. Code documentation is way more sparse than intended, tried to do too much and time fell short
7. collision checking when objects are at the "edges"
8. everything in ndc

## Fun stuff
1. Particle system uses a struct of arrays and calculates position/attributes CPU side, then copies into a buffer and drawn instanced
2. All models made in blender and textures made in Substance Painter
3. Sounds all CC0 from FreeSound.org
4. Font from google fonts
5. CMakeLists.txt -- used official SDL3 template for most of this

### Tweaking player values
1. ship/asteroidsettings/bulletdata
2. 