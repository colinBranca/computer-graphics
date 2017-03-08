# Introduction to Computer Graphics

## Homework 1 - Vincenzo Bazzucchi, Colin Branca, Nicolas Phan Van

### Exercice 1:  Triangle Spirals

To draw a normal spiral, the constant FERMAT_SPIRAL shall be set to 0. To draw a Fermat spira, set it to a non-zero value.

#### Classical Spiral

To generate the first spiral we first noticed that the one in the handout consisted of around 60 spirals, and that in each "half-spiral" there were around 10 triangles.

We worked in polar coordinates using radius r and angle theta.
We therefore used a `for` loop from one to sitxy, incrementing the angle by Pi/10.0 for each iteration. Also, at each iteration, we compute the new radius by multiplying the angle by a positive constant.

We then computed the x and y coordinates using the conversion formulas:

 - x = r * cos(theta)
 - y = r * sin(theta)

This gave us the position of each triangle. We set the translation matrix of the triangle to use these values T[3][0] = x and T[3][1] = y;

To compute the rotation matrix of each triangle we took the angle -theta in the rotation matrix we learnt in class.

To compute the scale factor of each triangle, we start with a default value and increase it (thus reducing the size) at each iteration.

#### Fermat Spiral

We used the same rotation angles from the previous part.

The formula from lab slides was used to upgrade the angle theta.

### Exercice 2: Checkerboard

### Exercice 3: Solar System

For each planet we had to perform three operations:

 - Scale: to determine the size of each body, the value does not change during execution.
 - Rotation: to rotate each body around its axis, the value of the angle changes during execution.
 - Traslation: to determine the position of the planet (following the orbit equation). The value changes during execution.

For each planet, the correct order of transformation is therefore:
 1. scale
 2. rotate
 3. traslate

Therefore the model matrix was obtained by multiplying the matrices in this order.

To compute the position of the Moon and of Earth we used the same techniques used in the first exercise: polar coordinates, ellipse equation (and therefore circle equations).

### Exercice Bonus: Arkanoid

Quads were with different scale factors to generate the walls, the bar and the ball.

The norm of the velocity is constant.

The velocity vector after a collision is computed using the following formula:

     v2 = v1 - 2 * dot(v1, n) n

Where v2 is the velocity vector after the collision, v1 the velocity vector before the collision and n the vector normal to the surface of the collision.

To have an angle when the ball bounces against the bar, n was chosen using the following strategy:

 - The horizontal component nx is nx = bcx - bx where bcx is the horizontal component of the position of the center of the bar and bx is the horizontal component of the position of the ball.
 - The vertical component ny is ny = sqrt(1 - nx^2)

Various utility functions where added.

