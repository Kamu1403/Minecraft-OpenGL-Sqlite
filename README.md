# Minecraft-OpenGL-Sqlite
## Introduction
 A Minecraft-like game implemented with OpenGL and Sqlite.

## Our work
- This project is a demo of a Minecraft sandbox game. 
- The project implements the basic block and scene generation in Minecraft, 
  lighting rendering, collision detection logic, and placing and destroying cubes. 
- In addition, we use the database to realize an infinite map that can be automatically 
  generated, stored, and loaded with the help of block cache scheduling, and use 
  perspective and surface clipping to improve rendering efficiency.
- We've provided a demo video [here](./resources/demonstration.mp4).
- For more information, refer to [Project report](./resources/Project_report.pdf).

## Project contents
- The project is a demo of a Minecraft sandbox game, which will achieve the following functions:
- [x] Random generation of maps;
- [x] Dynamic save and recall of the map;
- [x] Real-time perspective and surface clipping;
- [x] Realize scenes composed of various blocks;
- [x] Characters walking, jumping, colliding;
- [x] Simple natural light.

## Involved CG techniques
- [x] Database storage: SQLite database for map loading, storage and rendering;
- [x] Cache scheduling: NRU cache replacement algorithm;
- [x] Dynamic random generation: Random generation of discrete uniform distribution;
- [x] Graphics rendering pipeline: GLFW, GLM implementation model output to GPU and rendering;
- [x] Physics Engine: Position detection.


## Dependencies
- OpenGL
- Glfw
- Glm
- Sqlite

## How to use

1) We recommand to use Visual Studio 2019 to create an environment with OpenGL, including 
   the components Glfw and Glm
2) Build and run the program under Windows x86.

## References
- https://learnopengl.com/Introduction;
- https://github.com/Hopson97/MineCraft-One-Week-Challenge/tree/master/Source;
- http://www.linuxgraphics.cn/graphics/opengl_view_frustum_culling.html;
- https://www.runoob.com/sqlite/sqlite-tutorial.html;
- https://blog.csdn.net/Programmer_Dong/article/details/115678387.
