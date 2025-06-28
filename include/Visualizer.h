#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <thread>
#include <chrono>
#include <random>
#include <stdexcept>
#include "Cell.h"
#include "Vesicle.h"

const int GRID_SIZE_X = 30;
const int GRID_SIZE_Y = 30;
const double L = 20.0;
const double CIRCLE_RADIUS = 50.0;
const double DIFFUSION_COEFF = 500.0;
const double DT = 0.3;
const int N_SAMPLES = 2;
const int N_VESICLES = 10;

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
#endif // VISUALIZER_H
