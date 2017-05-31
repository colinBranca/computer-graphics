#include <iostream>
#include <fstream>

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

    glm::vec3 parseRow(string row) {
        glm::vec3 res;
        size_t start = 0;

        size_t space = row.find(" ");

        res.x = (float) atof(row.substr(start, space).c_str());

        start = space;
        space = row.find(" ", start);
        res.y = (float) atof(row.substr(start, space).c_str());

        start = space;
        res.z = (float) atof(row.substr(start, row.length()).c_str());

        cout << "parsed point x= " << res.x << " y= " << res.y << " z= " << res.z << endl;
        return res;
    }

public:
    void Init(size_t points_in_curve, vector<glm::vec3> ps) {
        resolution = points_in_curve;
        control_points = ps;
        computePath();
    }

    vector<glm::vec3> getPath() const {
        return path;
    }

    // void Init(size_t resolution) {
    //     ifstream source("./points.txt");
    //     while (!source.eof()) {
    //         string row;
    //         getline(source, row);
    //         control_points.push_back(parseRow(row));
    //     }
    //     source.close();
    //     Init(resolution, control_points);
    // }
};
