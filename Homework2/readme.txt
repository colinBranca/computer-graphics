
## 1 - Perspective Projection
The following matrix (presented during the lectures) was used:

![matrix](readme_pictures/per_mat.png)

## 2 - Trackball

### Drag and drop rotation
#### Projecting a screen point on the matrix
First the *z* component of the position vector of the pointer is determined using the
equation of the sphere. If the cursor is outside the sphere, the equation of a hyper
 plan is used:
 - *z = sqrt(radius² - x² - y²)* if inside the sphere.
 - *z = (1/2)r² / sqrt(x² + y² )* otherwise.

#### Finding the rotation angle
Now, having found the new position on the sphere and using the old one stored in
a variable, we can compute the vector about which the rotation was performed.
Note *a* the previous position of the pointer and *b* its new one, the vector about
which the rotation was performed *n* is *n = a cross b*
The rotation angle is then *theta = arcsin(|n|)*

#### Building the rotation matrix
Having the rotation angle and the vector about which the rotation is performed,
the Rodrigues formula was used to obtain the rotation matrix.

![rod](readme_pictures/rod.png)

#### Using the matrix
We the user drags the sphere (or hyperplan) to rotate the scene, we obtain the
new rotation matrix by using the technique described above. To avoid having the
scene going back to the original position, we obtain the new rotation matrix by
computing *new_matrix x old_matrix*.

This is equivalent to first applying the old rotation and the new one, thus
implementing a continuous movement. This is possible because matrix multiplication
is associative.

### Zooming
The position of the pointer is projected on hour *[-1,1]x[-1,1]* space.
The y component is then compared to the previous y component of the pointer,
to determine whether to zoom in or out.
Finally the view matrix is translated along the *z* axis.

## 3 - Triangle Grid and Animation

### Triangulation of the board
To triangulate the board, the coordinates of each vertex were computed and stored in the `vector`.

To do so, we iterate over the lines and columns of the grid (0-100, 0-100) and find the coordinate using the formula *c(x) = (2 / grid_dim) x - 1*.
This formula gives evenly distributed values, and
 - *c(grid_dim) = 1*
 - *c(0) = -1*

We then create and fill the indices vector as illustrated by the following picture:
![grid](./readme_pictures/grid.png)

We iterate over all vertices and for each vertex and the three adjacent to it, we add the indices of its coordinates to the vector.

## 4 - Bonus
To do the water animation, we simply changed the height equation in grid_vshader.
We added a phase (here: 2.2) in the sin equation which depends on the time.
>
> height = sin(((uv.x + uv.y) + 2.2f) * time) * 0.1f
>
With this phase that changes every time, we obtain, when iterated on all vertices, a moving wave.
