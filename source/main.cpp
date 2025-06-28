#include "Visualizer.h"

int main()
{

    // create the window (remember: it's safer to create it in the main thread due to OS limitations)
    sf::RenderWindow window(sf::VideoMode({1600, 900}), "OpenGL");

    // deactivate its OpenGL context
    window.setActive(false);

    Cell cell(GRID_SIZE_Y, GRID_SIZE_X, L);

    Vesicles vesicles;

    // Seed initial vesicles
    for (int i = 0; i < N_VESICLES; ++i) {

        vesicles.create(CIRCLE_RADIUS, N_SAMPLES, DIFFUSION_COEFF, DT, cell, 50);
    }

    // launch the rendering thread
    std::thread thread(&renderingThread, &window, &vesicles, &cell);


    while (window.isOpen())
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        vesicles.diffuse(cell);

    }

    thread.join();
};
