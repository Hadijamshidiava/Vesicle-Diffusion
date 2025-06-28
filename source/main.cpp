#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>
#include <random>
#include <stdexcept>
#include "Cell.h"
#include "Vesicle.h"

const int GRID_SIZE_X = 20;
const int GRID_SIZE_Y = 30;
const double L = 2.0;
const double CIRCLE_RADIUS = 50.0;
const double DIFFUSION_COEFF = 250.0;
const double DT = 0.1;
const int N_SAMPLES = 8;
const int N_VESICLES = 4;

void renderingThread(sf::RenderWindow* window, Vesicles* vesicles, Cell* cell)
{
    // activate the window's context
    window->setActive(true);
    sf::VertexArray triangle(sf::PrimitiveType::Triangles, 3);
    sf::Event event;
    // the rendering loop
    while (window->isOpen())
    {
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
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
            vesicle.setFillColor(sf::Color(255, 0, 0, 0));
            vesicle.setOutlineColor(sf::Color::Black);
            vesicle.setOutlineThickness(2); // Thickness of the outline
            window->draw(vesicle);
        }

        window->display();

    }
}

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

        vesicles.create(CIRCLE_RADIUS, N_SAMPLES, DIFFUSION_COEFF, DT, cell);
    }

    // launch the rendering thread
    std::thread thread(&renderingThread, &window, &vesicles, &cell);

    // the event/logic/whatever loop
    while (window.isOpen())
    {
        // std::this_thread::sleep_for(std::chrono::milliseconds(500));
        vesicles.diffuse(cell);

    }

    thread.join();
};
