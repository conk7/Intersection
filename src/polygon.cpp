#include <SFML/Graphics.hpp>
#include "../include/polygon.hpp"
#include "../include/line.hpp"
#include <cmath>

#define EPS 10e-3

Polygon::Polygon()
{
    vertCount = 0;
    // this->edges.resize(vertCount);
    // this->verts.resize(vertCount);
    dynamicEdge = false;
    finished = true;
}

void Polygon::addVert(sf::Vector2i coords, Grid grid)
{
    float gridSizeF = grid.getGridSizeF();
    int gridSize = grid.getGridSizeU();
    sf::Vector2f coordsF = sf::Vector2f(static_cast<float>(coords.x), static_cast<float>(coords.y));
    coords = sf::Vector2i(round(coordsF.x / gridSizeF) * gridSize, round(coordsF.y / gridSizeF) * gridSize);
    coordsF = sf::Vector2f(static_cast<float>(coords.x), static_cast<float>(coords.y));

    if(verts.size() == 0)
    {
        float radius = 5;
        sf::CircleShape circle(radius);
        circle.setPosition(sf::Vector2f(static_cast<float>(coords.x) - radius, static_cast<float>(coords.y) - radius));
        circle.setFillColor(sf::Color::Yellow);
        verts.push_back(circle);

        tLine edge(sf::Vector2f(coords.x, coords.y), sf::Vector2f(coords.x + radius, coords.y + radius), sf::Color(105,105,105,255), radius);
        edges.push_back(edge);
        finished = false;
        dynamicEdge = true;


        return;
    }
    else if(verts.size() > 0)
    {
        float radius = 5;
        if(abs(coordsF.x - verts[0].getPosition().x - radius) < EPS &&
            abs(coordsF.y - verts[0].getPosition().y - radius) < EPS)
        {
            edges[edges.size() - 1].updatePointB(coordsF);
            dynamicEdge = false;
            finished = true;

            for (auto &vert : verts)
                vert.setFillColor(sf::Color::Red);
            for (auto &egde : edges)
                egde.setColor(sf::Color::Black);

            return;
        }
        else
        {
            sf::CircleShape circle(radius);
            circle.setPosition(sf::Vector2f(static_cast<float>(coords.x) - radius, static_cast<float>(coords.y) - radius));
            circle.setFillColor(sf::Color::Red);
            verts.push_back(circle);

            edges[edges.size() - 1].updatePointB(coordsF);

            tLine edge(sf::Vector2f(coords.x, coords.y), sf::Vector2f(coords.x+5, coords.y+5), sf::Color(105,105,105,255), radius);
            edges.push_back(edge);

            
            return;
        }
    }
}

void Polygon::updateDynamicEdge(Grid grid, sf::Vector2i coords)
{
    if(!dynamicEdge)
        return;
    float gridSizeF = grid.getGridSizeF();
    int gridSize = grid.getGridSizeU();

    sf::Vector2f coordsF = sf::Vector2f(static_cast<float>(coords.x), static_cast<float>(coords.y));
    coordsF = sf::Vector2f(round(coordsF.x / gridSizeF) * gridSize, round(coordsF.y / gridSizeF) * gridSize);
    edges[edges.size() - 1].updatePointB(coordsF);
}

std::vector<tLine> Polygon::getEdges() const
{
    return this->edges;
}

std::vector<sf::CircleShape> Polygon::getVerts() const
{
    return this->verts;
}

bool Polygon::isFinished()
{
    return finished;
}

std::vector<Point> Polygon::getVertsCoords()
{
    unsigned vertCount = verts.size();
    std::vector<Point> coords(vertCount);
    for (int i = 0; i < vertCount; i++)
    {
        coords[i].setX(verts[i].getPosition().x);
        coords[i].setY(verts[i].getPosition().y);
    }
    return coords;
}

void Polygon::setVerts(std::vector<sf::CircleShape> verts)
{
    this->verts = verts;
}

void Polygon::setEdges(std::vector<tLine> edges)
{
    this->edges = edges;
}

// void Polygon::draw(sf::RenderWindow &window)
// {
//     for (auto &shape : shapes)
//     {
//         std::vector<tLine> edges = shape.getEdges();
//         for(auto &edge : edges)
//         {
//             window.draw(edge);
//         }
//         std::vector<sf::CircleShape> verts = shape.getVerts();
//         for(auto &vert : verts)
//         {
//             window.draw(vert);
//         }
//     }
// }
