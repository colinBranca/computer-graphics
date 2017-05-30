
class BezierCurve {
private:
    vector<glm::vec3> control_points;
    vector<glm::vec3> path;
    size_t resolution;

    int factorial(int n) {
        if (n == 0 || n == 1) return 1;
        int values[n + 1];
        values[0] = 1;
        for (size_t i = 1; i <= n; ++i) {
            values[i] = values[i - 1] * i;
        }
        return values[n];
    }

    int binom(int n, int k) {
        return factorial(n) / (factorial(k) * factorial(n - k));
    }

    float bernstein(float t, size_t n, size_t i) {
        return binom(n, i) * pow(t, i) * pow(1 - t, n - i);
    }

    glm::vec3 bezierPoint(float t) {
        glm::vec3 res = glm::vec3(0.0f);
        size_t n = control_points.size();
        for (size_t i = 0; i < n; ++i) {
            res += bernstein(t, n - 1, i) * control_points[i];
        }
        return res;
    }

    void computePath() {
        float step = 1.0f / ((float) resolution);
        for (size_t i = 0; i < resolution; ++i) {
            path.push_back(bezierPoint(i * step));
        }
    }

public:
    void Init(size_t points_in_curve, vector<glm::vec3> ps = vector<glm::vec3>()) {
        resolution = points_in_curve;
        control_points = ps;
        computePath();
    }

    vector<glm::vec3> getPath() const {
        return path;
    }
};
