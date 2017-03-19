# Homework 3

## 1 - Phong shading
Equations from the class slides were used while following the instructions in
the handout.

## 2 - Toon shading
The code from the Phong vertex shader was used.

We followed the instructions in the handout to compute the values to use with
the texture and applied what we learned in previous homework and labs about textures.

## 3 - Artistic shading
The code from the Phong vertex shader was used.

We followed the instructions in the handout to compute the values to use with
the texture and applied what we learned in previous homework and labs about textures.

## 4 - Flat shading
The code from the Phong vertex shader was used.

`dFdx` and `dFdy` give the vectors parallel to the surface of the triangle. To
find a normal to the surface of the triangle common to all the pixels we took
their cross products.
## 5 - Spotlight shading
The code from the Phong vertex shader was used.

The spotlight direction vector is received in the vector shader. We pass it to
the fragment shader in order to be able to use the provided formulas.
