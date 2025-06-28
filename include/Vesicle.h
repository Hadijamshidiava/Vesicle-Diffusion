#ifndef VESICLE_H
#define VESICLE_H
#include <vector>
#include <Eigen/Dense>


struct Vesicle {
    public:
        /**
         * Represents a single vesicle with a center, samples, and diffusion properties.
         */
        Eigen::Vector2d center;             // 2D coordinate of the vesicle center.
        Eigen::Vector2d position;             // 2D coordinate of the vesicle center.
        Eigen::MatrixXd samples;             // Coordinates of sampled points on the vesicle perimeter.
        double diffusion_coeff;              // Diffusion coefficient (D).
        int nSample;                         // Number of perimeter samples.
        double dt;                           // Time step used for diffusion.
        double r;                            // Vesicle radius.
        std::vector<int> overlapped;         // List of overlapping triangles.

        Vesicle(const Eigen::Vector2d& center, const Eigen::Vector2d& position, const Eigen::MatrixXd& samples,
                double diffusion_coeff, int nSample,
                double dt, double r)
            : center(center), position(position), samples(samples),
            diffusion_coeff(diffusion_coeff), nSample(nSample),
            dt(dt), r(r), overlapped() {}
};

class Vesicles {
public:
    /**
     * A container class for managing and diffusing multiple vesicles in a 2D cell mesh.
     */
    std::vector<Vesicle> vesicles;

    Vesicles() {}

    std::pair<bool, std::vector<int>> overlap_check(double r,
                                                    const std::vector<int>& oldOverlapped, const Eigen::Vector2d& center,
                                                    const Eigen::MatrixXd& samples, Cell& cell) {

        std::vector<int> newOverlapped; // Store indices of overlapped triangles
        Eigen::MatrixXd diff = cell.centers.rowwise() - center.transpose();
        Eigen::VectorXd distances = diff.rowwise().norm(); // Calculate the norm for each triangle center

        // Get indices of triangles within a distance of 3*r
        std::vector<int> triangleIndexes;
        Eigen::VectorXd moveDist(2); // Create a vector of size 1
        moveDist.setZero(); // Initialize moveDist to zero

        for (int i = 0; i < distances.size(); i++) {
            if (distances(i) <= 3 * r) {
                triangleIndexes.push_back(i);
            }
            if (r <= distances(i) && distances(i) <= 2 * r) {
                moveDist += diff.row(i);  // Accumulate vectors from the vesicle center to nearby triangle centers.
                    // If the resulting vector (moveDist) has a small magnitude,
                    // it suggests the triangles are evenly distributed around the vesicle,
                    // implying the vesicle is well-contained within the cell boundary.
            }
        }

        if(is_outside(moveDist)){
            return { false, oldOverlapped };
        }


        if (triangleIndexes.empty()) {
            return { false, oldOverlapped }; // No triangles within range
        }

        for (int index : triangleIndexes) {
            bool isOc = cell.is_occupied(center, r, distances, index);
            if (cell.triangles[index].occupied && isOc &&
                std::find(oldOverlapped.begin(), oldOverlapped.end(), index) == oldOverlapped.end()) {
                return { false, oldOverlapped }; // Overlapping; not valid
            }
            else if (isOc) {
                newOverlapped.push_back(index); // Add to new overlaps
            }
        }

        // Update occupied status for old overlapped triangles
        for (int index : oldOverlapped) {
            cell.triangles[index].occupied = false; // Mark as not occupied
        }
        for (int index : newOverlapped) {
            cell.triangles[index].occupied = true; // Mark as occupied
        }

        return { true, newOverlapped }; // Valid position
    }

    void create(double CIRCLE_RADIUS, int N_SAMPLES, double diffusion_coeff, double dt, Cell& cell, int iteration) {
        /**
         * Attempts to create a new vesicle at a random valid position.
         * If the generated position is invalid (due to overlap), it retries recursively.
         *
         * Args:
         *     CIRCLE_RADIUS (double): Radius of each vesicle.
         *     x_max, y_max (double): Simulation area boundaries.
         *     N_SAMPLES (int): Number of sample points on vesicle perimeter.
         *     diffusion_coeff (double): Diffusion coefficient.
         *     dt (double): Time step.
         *     cell (Cell): The simulation mesh.
         *     iteration: Maximum number of attempts to create the vesicle before giving up.
         */

        if(iteration == 0)
            return;

        // Generate a random center within specified bounds
        std::random_device rd; // Obtain a random number from hardware
        std::mt19937 eng(rd()); // Seed the generator
        std::uniform_int_distribution<> x_dis(CIRCLE_RADIUS, cell.x_max() - CIRCLE_RADIUS);
        std::uniform_int_distribution<> y_dis(CIRCLE_RADIUS, cell.y_max() - CIRCLE_RADIUS);

        Eigen::Vector2d center(x_dis(eng), y_dis(eng)); // Random center coordinates
        Eigen::Vector2d position(center.x() - CIRCLE_RADIUS, center.y() - CIRCLE_RADIUS);
        // Eigen::Vector2d position(center.x(), center.y());

        // Generate samples on the perimeter of the vesicle (I do not use them in the current overlap method so I leave the array empty).
        Eigen::MatrixXd samples(2, N_SAMPLES);

        // --[ Sample generation for future use ]--
        // These lines generate evenly spaced sample points along the vesicle perimeter.
        // Currently unused, but kept here for possible reactivation in future feature development.
        //
        // Eigen::VectorXd angles = Eigen::VectorXd::LinSpaced(N_SAMPLES, 0, 2 * M_PI);
        // for (int i = 0; i < N_SAMPLES; ++i) {
        //     samples(0, i) = center(0) + CIRCLE_RADIUS * cos(angles(i)); // x-coordinates
        //     samples(1, i) = center(1) + CIRCLE_RADIUS * sin(angles(i)); // y-coordinates
        // }

        // Create a new vesicle
        Vesicle vesicle(center, position, samples, diffusion_coeff, N_SAMPLES, dt, CIRCLE_RADIUS);

        // vesicles.push_back(vesicle);

        // Check for overlap
        auto [flag, overlap] = overlap_check(CIRCLE_RADIUS, {}, center, samples, cell);
        if (flag) {
            vesicle.overlapped = overlap; // Update overlapping triangles
            vesicles.push_back(vesicle); // Add vesicle to the list
        } else {
            // Recursive retry
            create(CIRCLE_RADIUS, N_SAMPLES, diffusion_coeff, dt, cell, iteration-1);
        }
    }

    void diffuse(Cell& cell) {
        /**
         * Performs a diffusion step on all vesicles. Each vesicle moves by a random
         * Brownian step and updates its position only if the new location is valid.
         *
         * Args:
         *     cell (Cell): The mesh to test occupancy against.
         */

        // Seed the random number generator for Brownian motion
        std::random_device rd;
        std::mt19937 eng(rd());
        std::normal_distribution<> norm(0.0, 1.0); // Normal distribution for Brownian motion

        for (size_t index = 0; index < vesicles.size(); ++index) {
            Vesicle& vesicle = vesicles[index]; // Reference to current vesicle
            Eigen::Vector2d old_center = vesicle.center;  // Save old center
            Eigen::Vector2d old_position = vesicle.position;  // Save old position
            Eigen::MatrixXd old_samples = vesicle.samples; // Save old samples
            std::vector<int> old_overlap = vesicle.overlapped; // Save old overlaps

            // Brownian motion scaling
            double scale = std::sqrt(2.0 * vesicle.diffusion_coeff * vesicle.dt);

            // Generate random displacement
            Eigen::Vector2d displacement(scale * norm(eng), scale * norm(eng));

            // Apply displacement
            Eigen::Vector2d new_center = old_center + displacement;
            Eigen::Vector2d new_position = old_position + displacement;
            Eigen::MatrixXd new_samples = old_samples.colwise() + displacement; // Update samples

            // Validate and update
            auto [flag, new_overlap] = overlap_check(vesicle.r, old_overlap, new_center, new_samples, cell);
            if (flag) {
                vesicle.center = new_center;
                vesicle.samples = new_samples;
                vesicle.overlapped = new_overlap;
                vesicle.position = new_position;
            }
        }
    }

    bool is_outside(Eigen::VectorXd& distances){
        /**
         * Checks whether the vesicle is likely outside the cell boundary
         * based on the accumulated vector from surrounding triangle centers.
         *
         * @param vector_sum The accumulated vector from vesicle to nearby triangles.
         * @return True if the vesicle appears to be near the boundary or outside.
         */
        if (distances.norm() > 50){
            return true;
        }
        return false;
    }
};
#endif // VESICLE_H
