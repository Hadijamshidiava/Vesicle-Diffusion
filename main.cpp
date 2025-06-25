#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>
#include <random>
#include "Cell.h"
#include "Vesicle.h"

const int GRID_SIZE_X = 10;
const int GRID_SIZE_Y = 10;
const double L = 10.0;
const double CIRCLE_RADIUS = 50.0;
const double DIFFUSION_COEFF = 8.0;
const double DT = 0.1;
const int N_SAMPLES = 8;
const int N_VESICLES = 4;

void renderingThread(sf::RenderWindow* window, Vesicles* vesicles, Cell* cell)
{
    // activate the window's context
    window->setActive(true);
    sf::VertexArray triangle(sf::PrimitiveType::Triangles, 3);

    // the rendering loop
    while (window->isOpen())
    {

        while (const std::optional event = window->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window->close();
        }

        window->clear();

        for (int index = 0; index < cell->triangles.size(); index++) {
            triangle[0].position = sf::Vector2f(cell->triangles[index].vertices(0, 0), cell->triangles[index].vertices(1, 0));
            triangle[1].position = sf::Vector2f(cell->triangles[index].vertices(0, 1), cell->triangles[index].vertices(1, 1));
            triangle[2].position = sf::Vector2f(cell->triangles[index].vertices(0, 2), cell->triangles[index].vertices(1, 2));

            if(cell->triangles[index].occupied){
                triangle[0].color = sf::Color::Green;
                triangle[1].color = sf::Color::Green;
                triangle[2].color = sf::Color::Green;
            }else{
                triangle[0].color = sf::Color::Blue;
                triangle[1].color = sf::Color::Blue;
                triangle[2].color = sf::Color::Blue;
            }

            window->draw(triangle);

            // window->display();
        }

        for (int index = 0; index < vesicles->vesicles.size(); index++){
            sf::CircleShape vesicle(vesicles->vesicles[index].r);
            Eigen::Vector2d position = vesicles->vesicles[index].position;
            sf::Vector2f sfmlPosition(position.x(), position.y());
            vesicle.move(sfmlPosition);
            window->draw(vesicle);
        }

        window->display();

    }
}

int main()
{
    // create a triangle strip
    int grid_size_y = 12;
    int grid_size_x = 15;
    double L = 80.0;

    // create the window (remember: it's safer to create it in the main thread due to OS limitations)
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "OpenGL");

    // deactivate its OpenGL context
    window.setActive(false);

    Cell cell(grid_size_y, grid_size_x, L);

    Vesicles vesicles;

    // Seed initial vesicles
    for (int i = 0; i < N_VESICLES; ++i) {
        vesicles.create(CIRCLE_RADIUS, N_SAMPLES, DIFFUSION_COEFF, DT, cell);
    }

    // launch the rendering thread
    std::thread thread(&renderingThread, &window, &vesicles, &cell);

    // the event/logic/whatever loop
    while (window.isOpen())
    {
        // std::this_thread::sleep_for(std::chrono::seconds(2));
        vesicles.diffuse(cell);

    }

    thread.join();
}
