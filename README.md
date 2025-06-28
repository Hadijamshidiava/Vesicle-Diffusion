## Vesicle Diffusion Prototype

This project is a fast and modular prototype for simulating vesicle diffusion within a 2D triangular mesh — a simplified abstraction of vesicle transport along biological membranes.

The simulation was developed in response to a scientific paper shared as part of an interview preparation, aiming to demonstrate understanding of spatial diffusion, geometric sampling, and efficient real-time computation.

## Dependencies & Installation
To build and run the project, make sure the following libraries are installed:

### Required Libraries

SFML (Simple and Fast Multimedia Library) – for real-time visualization

Eigen 3 – for linear algebra and matrix computations

### Install on Linux (Example)

```bash

# Install Eigen (if not already installed)
sudo apt-get install libeigen3-dev

# Install SFML (use package or manually)
sudo apt-get install libsfml-dev

```

Alternatively, you can build SFML manually and point CMake to the custom install path.

## Key Features

- Vesicles modeled as circular particles with membrane sample points
- Triangle occupancy detection using a fully vectorized 2D side-test
- Brownian motion (diffusion) with rejection sampling for spatial exclusion
- Per-vesicle diffusion coefficients
- Real-time interactive visualization using
- Modular design, easily extendable to tetrahedral 3D meshes and reaction models

## Reference

This prototype was inspired by:

> **Iain Hepburn et al., "Vesicle and reaction-diffusion hybrid modeling with STEPS"**  
> *Communications Biology, 2024*  
> [https://doi.org/10.1038/s42003-024-06276-5](https://doi.org/10.1038/s42003-024-06276-5)

The current code implements a simplified 2D equivalent of the tetrahedral diffusion model described in the paper, using triangle meshes and point-based surface sampling for efficiency.


