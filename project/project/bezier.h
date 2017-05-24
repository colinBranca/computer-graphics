#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Bezier {
public:

  void Init() {
    bezierPoints.clear();
  }

  glm::vec3 createPoint(float t) {
    glm::vec3 point = computeBezier(bezierPoints.size(), t);
    bezierPoints.push_back(point);
    return point;
  }

private:

  vector<glm::vec3> bezierPoints;

  glm::vec3 computeBezier(int n, float t) {
    glm::vec3 bn = glm::vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < n; i++) {
      bn += bezierPoints[i] * bernstein(t, n, i);
    }
    return bn;
  }

  float bernstein(float t, int n, int i) {
    if(i < 0 || i > n) return 0;
    else if(i <= 1) return 1;
    else return (factorial(n) / (factorial(n-i) * factorial(i))) * pow(t, i) * pow(1-t, n-1);
  }

  float factorial(int x) {
    if(x <= 1) return x;
    else return x * factorial(x-1);
  }
}
