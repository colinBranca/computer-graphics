# Introduction to Computer Graphics: Final Report

## Implemented features

Here is a brief recap of the features implemented for each step:

### Part 1

 - Perlin noise and fractal brownian motion for terrain rendering
 - Phong shading
 - BONUS: infinite terrain.

### Part 2

 - Blend of four textures according to height and surface normals
 - Skybox
 - BONUS: multiple skyboxes
 - BONUS: skybox and terrain reflection in water
 - BONUS: water shape and movement

### Part 3

 - Controlling the camera using mouse and keyboard
 - Camera acceleration along its path
 - First person exploration mode
 - Bezier curve exploration path
 - BONUS: realtime terrain modification using keyboard

## Implementation description

We will briefly explain how the bonuses and the new features (part 3) were implemented.

### Texture blending

We provide a short explanation of our implementation of this feature as it was not presented in the second part report.

As indicated in the handout, we used four textures: sand, grass, snow and rock (ground). The textures are blended according to
multiple factors. We take into account the height, the water height and surface normals.

We choose these parameters as we did not want to have snow too close to the water and grass or snow under water. Also
having snow on the flat zones did not make much. Finally using surface normals allowed us not use grass or snow on the
very steep portions of our terrains and to replace them with rocks in this situations.

The Terrain class and its shaders were modified to accomplish this.

### Infinite terrain.

Source: `InfiniteTerrain.h`

The terrain was divided into squared chunks each of which is composed of a `Terrain` and a `Water`
object. This tuple is stored in map hashed by the grid coordinates of the chunk (e.g (0,0) for chunk 1, (1, 0) for the one
on its right).

Upon starting the application, 9 chunks are created. When the camera reaches a new chunk, we check whether the three chunk
adjacent to the current one were already created and we create them if that is not the case.

Having many large chunks existing at the same time and being created on the fly required some performance optimizations:

 1. For the previous parts, the creation of a terrain or water object resulted in the compilation of the associated shaders,
 which quickly resulted in latency issues. Therefore we compile the shaders only once, upon the creation of the first
 chunk.
 2. Rendering all the existing chunks was very expensive. We set out to only render the visible chunks: the current one and
 the 8 surrounding ones.

### Multiple skyboxes

To test the following bonus, we provide multible skyboxes. The skybox to use can be selected using the number keys 0,1,2,3,4.

### Water shape and movement

We decided that water should not look like a flat blue square and we set out to bring it to life.

In order to accomplish this we played with our Perlin noise implementation and generated a dedicated Perlin noise height
map which we applied to the water. This resulted in "bumpy" water, the bumps looking like waves but weirdly standing still.

We used the time constant to animate the water and obtain a flowing look.

Modifcations to the water class and its shaders were necessary.

### Skybox and Terrain reflection in water

Having implemented waves, we wanted our water to reflect. To do so, in each water vertex shader, we compute the surface
normal and use it together with the `reflect` and `mix` function (and the skybox texture) to reflect the sky.

To reflect the terrain, we had to use an other method.
We first drew each chunk in a Framebuffer with a `ReversedViewMatrix` computed in class `Camera`. This argument is then passed to the `Init` function of class Water.


Modifcations to the water class and its shaders were necessary.

### Camera controls

The camera can be moved using the usual W,A,S,D keys and the mouse can be used to orient the view. Camera acceleration
was implemented. The F3 key allows to cycle through our three different camera modes: free exploration, first person mode
and Bezier curve path.

Modifications to the Camera class and the main program were required.

### First person exploration mode

The height map of the terrain is retrieved using `glGetTexImage` and stored in an array.
The current height of the camera is then retrieved by computing its position in the world
and then by translating this into an index for the array. We also implemented a small
filter function to smooth the values of the array and result in a less "bumpy" exploration.

Modifications to the perlin noise and camera classes were necessary.

### Bezier curve

Source: `bezier.h`

Our `BezierCurve` class is initialized using the list of control points and the number (resolution) of points to generate
along the curve. It internally uses Bernstein polynomials to compute interpolate the curve starting from the control points.

We then chose the control points by exploring our terrain.

A simple velocity control along the path was also implemented.

### Realtime terrain modifications

The parameters of our procedural terrain can be tweaked during the program execution using several keys:

 - F1/F2 increase/reduce the height of the water.
 - F4/F5 increase/reduce the amplitude parameter of the perlin noise.
 - F6/F7 increase/reduce the frequency parameter of the perlin noise.
 - F8/F9 increase/reduce the number of octaves used to compute the fbm

## General comments

The implementation of the infinite terrain required us to modify and sometimes rewrite from scratch some parts of our codebase
which cost us the time to implement additional bonus features. The implementation of the water reflection and refraction (which we
worked on for a long time, unfortunately not being able to make it work) took us a very long time.

We also had some difficulties in predicting the amount of time necessary to implement some features which, combined with our busy
schedules, midterms and various projects deadlines, resulted in tensions inside the group and a bit of frustration.

## Team work

 - Vincenzo Bazzucchi (33%) worked on infinite terrain, water reflection and on Bezier curve implementation.
 - Colin Branca (33%) worked on texture blending, water reflection and refraction, water shaping and animation, camera
 acceleration and on keyboard controls for realtime terrain modifications.
 - Nicolas Phan Van (33%) worked on reflection and refraction, multiple skyboxes and on first person exploration.
