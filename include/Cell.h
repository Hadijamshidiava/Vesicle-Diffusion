#ifndef CELL_H
#define CELL_H

#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include <cmath>

struct Triangle {
public:
    Eigen::Matrix<double, 2, 3> vertices; // 3 vertices in 2D space
    Eigen::Matrix<double, 2, 3> normals;  // Outward normals for each vertex
    bool occupied;

    Triangle(const Eigen::Matrix<double, 2, 3>& verts, const Eigen::Matrix<double, 2, 3>& norms, bool occ)
        : vertices(verts), normals(norms), occupied(occ) {}
};

class Cell {
public:
    std::vector<Eigen::Vector2d> points;
    std::vector<Triangle> triangles;
    Eigen::MatrixXd centers;

    Cell(int grid_size_y, int grid_size_x, double L) :centers(0, 2) {
        points = generate_grid(grid_size_y, grid_size_x, L);
        generate_mesh(grid_size_y, grid_size_x);
    }

    std::vector<Eigen::Vector2d> generate_grid(int rows, int cols, double L) {
        std::vector<Eigen::Vector2d> grid_points;
        for (int j = 0; j < rows; ++j) {
            for (int i = 0; i < cols; ++i) {
                double x = i * L + (j % 2) * (L / 2) + 50;
                double y = j * (std::sqrt(3) / 2) * L + 50;
                grid_points.emplace_back(x, y);
            }
        }
        return grid_points;
    }

    void generate_mesh(int rows, int cols) {
        for (int j = 0; j < rows - 1; ++j) {
            for (int i = 0; i < cols - 1; ++i) {
                int idx = j * cols + i;
                Eigen::Vector2d p0 = points[idx];
                Eigen::Vector2d p1 = points[idx + 1];
                Eigen::Vector2d p2 = points[idx + cols];
                Eigen::Vector2d p3 = points[idx + cols + 1];

                if (j % 2 == 0) {
                    create_triangle(p0, p1, p2);
                    create_triangle(p1, p3, p2);
                } else {
                    create_triangle(p0, p1, p3);
                    create_triangle(p0, p3, p2);
                }
            }
        }
    }

    void create_triangle(const Eigen::Vector2d& v0, const Eigen::Vector2d& v1, const Eigen::Vector2d& v2) {
        Eigen::Matrix<double, 2, 3> vertices;
        vertices.col(0) = v0;
        vertices.col(1) = v1;
        vertices.col(2) = v2;

        Eigen::Matrix<double, 2, 3> normals;
        Eigen::Vector2d center = Eigen::Vector2d::Zero();

        for (int i = 0; i < 3; ++i) {
            Eigen::Vector2d p_i = vertices.col(i);
            Eigen::Vector2d p_j = vertices.col((i + 1) % 3);
            Eigen::Vector2d edge = p_j - p_i;
            Eigen::Vector2d normal(-edge.y(), edge.x()); // Rotate 90 degrees to get normal
            normal.normalize();
            normals.col(i) = normal;
            center += p_i; // accumulate the centers
        }

        triangles.emplace_back(vertices, normals, false);
        int currentRows = centers.rows();
        centers.conservativeResize(currentRows + 1, Eigen::NoChange); // Resize to accommodate one more center
        centers.row(currentRows) = center/3.0; // Add the new center
    }

    bool is_occupied(const Eigen::Vector2d& center, double r,
                     const Eigen::MatrixXd& distances, int triIndex, const Eigen::MatrixXd& samples) {

        const Triangle& triangle = triangles[triIndex];

        if (distances(triIndex) <= r){
            return true;
        }

        // samples shape: (2, N_samples)
        const int N = samples.cols();
        Eigen::Array<bool, Eigen::Dynamic, 1> inside_flags(N);
        inside_flags.setConstant(true);  // assume inside, then invalidate

        // Side-test logic (check for each edge)
        for (int j = 0; j < 3; ++j) {
            Eigen::Vector2d vertex = triangle.vertices.col(j);
            Eigen::Vector2d normal = triangle.normals.col(j);
            for (int i = 0; i < N; ++i) {
                Eigen::Vector2d sample = samples.col(i);
                Eigen::Vector2d diff = sample - vertex;
                double dot = diff.dot(normal);
                if (dot < 0) {
                    inside_flags(i) = false;
                }
            }
        }

        if (inside_flags.any()) return true;

        // Check if any vertex of the triangle is inside the circle
        for (int j = 0; j < 3; ++j) {
            Eigen::Vector2d vertex = triangle.vertices.col(j);
            double dist = (vertex - center).norm();
            if (dist <= r) {
                return true; // Found a vertex inside the circle
            }
        }

        return false; // No vertices are inside the circle
    }


    double x_max() const {
        // Ensure points is not empty
        if (points.empty()) {
            throw std::runtime_error("points vector is empty.");
        }

        // Extract the maximum x-coordinate from the points vector
        return std::max_element(points.begin(), points.end(),
                                [](const Eigen::Vector2d& a, const Eigen::Vector2d& b) {
                                    return a.x() < b.x(); // Compare based on x-value
                                })->x();
    }

    double y_max() const {
        // Ensure points is not empty
        if (points.empty()) {
            throw std::runtime_error("points vector is empty.");
        }

        // Extract the maximum y-coordinate from the points vector
        return std::max_element(points.begin(), points.end(),
                                [](const Eigen::Vector2d& a, const Eigen::Vector2d& b) {
                                    return a.y() < b.y(); // Compare based on x-value
                                })->y();
    }
};

#endif // CELL_H
