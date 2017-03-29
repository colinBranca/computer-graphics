# Homework 4

## 1 - Fast Gaussian Blur
We implemented the two different passes using two distinct framebuffers, both
initialized in the main.

In `Display` we used the first framebuffer to store the rendered image containing
the cube and the board so that we could post-process it.
So we `Bind` the first framebuffer, and `Draw` both the cube and the board.

We then unbind the first framebuffer and bind the second one. This way when we tell
the screenquad to draw itself, the result image will be stored in the second framebuffer.
Note the we tell the screenquad, through the `Draw` parameter whether we want it to
perform the first or the second pass of the blurring.
After having performed the first pass, we unbind the framebuffer.
We can now tell the screenquad to draw itself after performing the second pass.
As no framebuffer is bound, the default one will be used, which will result in the
image being displayed in the OpenGL Window.

The Screenquad class was also modified in order to make it aware of the presence of two framebuffers:
it now stores their id and is responsible for sending the uniform variables needed for the
blurring to the fragment shader: the sigma for the blurring formula, and a simple
integer to indicate the framebuffer whether it should perform the horizontal or vertical
blurring.

Finally the screenquad fragment shader computes the new color for the fragment
using the weighted sum learned during the lab session but only in one dimension.
The direction is chosen using the uniform variable indicating whether it is dealing
with the first or the second pass.

The key callback listening for W/Q simply calls a Screenquad varying the value
of the sigma that this class sends to the framebuffer through the uniform variable.

## 2 - Screen Space Reflection
First we had to do some work in the main.
We had to initialize the framebuffer and the shinyfloor with the framebuffer texture.

To compute the mirrored view point we flipped the z coordinate of the camera position.
> mirror_cam_pos = cam_pos

> mirror_cam_pos.z = -mirror_cam_pos.z

And then we used this new camera prositon to find our view projection matrix (VP):
> mirror_view = lookAt(mirror_cam_pos, cam_look, cam_up)

> mirror_view_projection = projection_matrix * mirror_view

To compute the reflection we had to to draw the cube's reflection in the framebuffer we initialized using the new VP as argument.

The next step was to modify the `floor_fshader`.
Here we had to compute color_from_mirror we had to find the vector uv that is needed in argument of texture.
> color_from_mirror = texture(tex_mirror, vec2(u, v)).rgb

To find u and v we followed he itnstructions given in comments.

## 3 - Motion Blur
First we had to compute the motionVector in quad_fshader. It is simply done by the floowing equation:
> motionVector = currentPosition.xy/currentPosition.w - previousPosition.xy/previousPosition.w

Next we computed the right color in screenquad_fshader.

We first searched the velocity vector stored in velocityTex and scaled it.
> velocity = scale * texture(velocityTex, uv).xy

Then we computed the line integral.
