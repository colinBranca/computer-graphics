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

  glm::vec3 bezierPosition(float t) {
    int size = bezierPoints.size();
    glm::vec3 point = computeBezier(size, t);
    //bezierPoints.push_back(point);
    std::cout << "point " << point.x << "  " << point.y << "  " << point.z << '\n';
    std::cout << "time  " << t << '\n';
    return point;
  }

  void addPoint(glm::vec3 pos) {
    bezierPoints.push_back(pos);
  }

private:

  vector<glm::vec3> bezierPoints;

  glm::vec3 computeBezier(int n, float t) {
    if(n == 0 || n == 1) return glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 bn = glm::vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < n; i++) {
      float p = bernstein(t, n, i);
      bn += bezierPoints[i] * bernstein(t, n, i);
      std::cout << i << "  " << p << '\n';
    }
    return bn;
  }

  float bernstein(float t, int n, int i) {
    if(i < 0 || i > n) return 0;
    else if(i <= 1) return 1;
    else return (factorial(n) / (factorial(n-i) * factorial(i))) * pow(t, i) * pow(1-t, n-i);
  }

  float factorial(int x) {
    if(x <= 1) return x;
    else return x * factorial(x-1);
  }
};
