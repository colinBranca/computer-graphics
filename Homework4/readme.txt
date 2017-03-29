# Homework 4

## 1 - Fast Gaussian Blur
TODO

## 2 - Screen Space Reflection
First we had to do some work in the main.
We had to initialize the framebuffer and the shinyfloor with the framebuffer texture.

To compute the mirrored view point we flipped the z coordinate of the camera position.
> mirror_cam_pos = cam_pos

> mirror_cam_pos.z = -mirror_cam_pos.z

And then we used this new camera prositon to find our VP.
> mirror_view = lookAt(mirror_cam_pos, cam_look, cam_up)

> mirror_view_projection = projection_matrix * mirror_view

To compute the reflexion we had to to draw the cube's reflexion in the framebuffer we initialized and with the new VP as argument.

The next step was to modify the floor_fshader.
Here we had ton compute color_from_mirror we had to find the vector uv that is needed in argument of texture.
> color_from_mirror = texture(tex_mirror, vec2(u, v)).rgb

To find u and v we followed he itnstructions given in comments.

## 3 - Motion Blur
First we had to compute the motionVector in quad_fshader. It is simply done by the floowing equation:
> motionVector = currentPosition.xy/currentPosition.w - previousPosition.xy/previousPosition.w

Next we computed the right color in screenquad_fshader.

We first searched the velocity vector stored in velocityTex and scaled it.
> velocity = scale * texture(velocityTex, uv).xy

Then we computed the line integral.
