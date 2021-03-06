# Homework 3

## 1 - Phong shading
Equations from the class slides were used while following the instructions in
the handout.

## 2 - Toon shading
The code from the Phong vertex shader was used.

We followed the instructions in the handout to compute the values to use with
the texture and applied what we learned in previous homework and labs about textures.

We saw in the `Init` method of `mesh.h` that `glTexImage1D` generates a red
texture therefore we only took the red component of the vector returned by `texture`
in the fragment shader.
> texture(tex1D, dot(normal_mv, light_dir)).r

> texture(tex1D, pow(dot(r, view_dir), alpha)).r

We also had to bind the texture in `mesh.h`.
To do so we had to activate the texture corresponding to the program id received
as an argument of the `BindShader` function.

## 3 - Artistic shading
The code from the Phong vertex shader was used.

We followed the instructions in the handout to compute the values to use with
the texture and applied what we learned in previous homework and labs about textures.

We also had to bind the texture in `mesh.h`.
To do so we had to activate the texture corresponding to the program id received
as an argument of the `BindShader` function.

## 4 - Flat shading
The code from the Phong vertex shader was used.

`dFdx` and `dFdy` give the vectors parallel to the surface of the triangle. To
find a normal to the surface of the triangle common to all the pixels we took
their cross products.
## 5 - Spotlight shading
The code from the Phong vertex shader was used together with the provided formulas.
