#pragma once
#include "icg_helper.h"


using namespace glm;

class Trackball {
public:
    Trackball() : radius_(1.0f) {}

    // this function is called when the user presses the left mouse button down.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    void BeingDrag(float x, float y) {
        anchor_pos_ = vec3(x, y, 0.0f);
        ProjectOntoSurface(anchor_pos_);
    }

    // this function is called while the user moves the curser around while the
    // left mouse button is still pressed.
    // x, and y are in [-1, 1]. (-1, -1) is the bottom left corner while (1, 1)
    // is the top right corner.
    // returns the rotation of the trackball in matrix form.
    mat4 Drag(float x, float y) {
        vec3 current_pos = vec3(x, y, 0.0f);
        ProjectOntoSurface(current_pos);

        // Explication in readme.txt
        mat4 rotation = IDENTITY_MATRIX;
        vec3 n = cross(anchor_pos_, current_pos);
        float normN = sqrt(dot(n, n));
        // arcsin is only defined in [-1, 1] therfore clipping wrong values
        // so that the board does not disappear
        float theta = asin(normN > 1 ? 1 : normN < -1 ? -1 : normN);
        mat3 nN = mat3(n[0]*n[0], n[0]*n[1], n[0]*n[2],
                       n[0]*n[1], n[1]*n[1], n[1]*n[2],
                       n[0]*n[2], n[1]*n[2], n[2]*n[2]);
        // Use Rodrigues Formula, from class slides.
        mat3 nMat = mat3(0.0f, -n[2], n[1],
                         n[2], 0.0f, -n[0],
                         -n[1], n[0], 0.0f);
        mat3 rotation3 = cos(theta)*mat3(1.0f) + (1-cos(theta)) * (nN) + sin(theta)*nMat;
        rotation = mat4(vec4(rotation3[0], 0.0f),
                        vec4(rotation3[1], 0.0f),
                        vec4(rotation3[2], 0.0f),
                        vec4(0.0f, 0.0f, 0.0f, 1.0f)
                       );
        return rotation;
    }

private:
    // projects the point p (whose z coordiante is still empty/zero) onto the
    // trackball surface. If the position at the mouse cursor is outside the
    // trackball, use a hyberbolic sheet as explained in:
    // https://www.opengl.org/wiki/Object_Mouse_Trackball.
    // The trackball radius is given by 'radius_'.
    void ProjectOntoSurface(vec3& p) const {
        float xSq = p[0]*p[0];
        float ySq = p[1]*p[1];
        float rSq = radius_*radius_;
        // Use sphere equation if pointer is inside the sphere
        if (xSq + ySq > rSq / 2.0f) {
            p[2] = (rSq / 2.0f) / (sqrt(xSq + ySq));
        } else { // or the hyperplan equation otherwise
            p[2] = sqrt(rSq - (xSq + ySq));
        }
    }

    float radius_;
    vec3 anchor_pos_;
    mat4 rotation_;
};
