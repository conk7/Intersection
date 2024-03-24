#include <SFML/Graphics.hpp>
#include <sstream>
#include <vector>
#include <cmath>

#include "../include/grid.hpp"
#include "../include/polygon.hpp"
#include "../include/line.hpp"
#include "../include/movingVert.hpp"
#include "../include/polygons.hpp"
#include "../include/mouse.hpp"
#include "../include/zoom.hpp"

int main()
{   
    //default window width and height
    const uint16_t SCREENW = 1920;
    const uint16_t SCREENH = 1080;

    //creating and configuring the window
    static std::string WindowTitle = "52";
    sf::RenderWindow window(sf::VideoMode(SCREENW, SCREENH), WindowTitle, 7, sf::ContextSettings(24,8,8));  // sf::Style::Fullscreen
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    static bool isFullscreen = false;
    
    //grid
    Grid grid(100.f);

    sf::RectangleShape background;
    background.setFillColor(sf::Color::White);
    background.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), 
                        static_cast<float>(window.getSize().y)));

    //view
    sf::View view;
    view.setCenter(0,0);
    view.setSize(window.getSize().x, window.getSize().y);
    sf::View visibleArea (sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));

    //debug info
    sf::Font font;
    font.loadFromFile("../../misc/Dosis-VariableFont_wght.ttf");
    sf::Text text;
    text.setCharacterSize(40);
    text.setFillColor(sf::Color::White);
    text.setFont(font);
    text.setPosition(20.f, 20.f);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.f);

    // sf::RectangleShape tileSelector(sf::Vector2f(grid.getGridSizeF(), grid.getGridSizeF()));
    // tileSelector.setFillColor(sf::Color::Transparent);
    // tileSelector.setOutlineColor(sf::Color::Blue);
    // tileSelector.setOutlineThickness(3);
  
    std::vector<Polygon> polygons;

    Zoom zoom(1.1, 12);

    //intersectionArea
    std::vector<Point> intersectionPointsCoords;
    std::vector<sf::CircleShape> intersectionPoints;

    bool redrawIntersectionArea = false;

    bool isVertMoving = false;
    MovingVert movingVertIdx = {-1,-1};

    //main loop
    while (window.isOpen())
    {
        //update view
        visibleArea.setSize(window.getSize().x, window.getSize().y);
        visibleArea.setCenter(static_cast<float>(window.getSize().x/2), static_cast<float>(window.getSize().y/2));

        //update bg
        background.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), 
                            static_cast<float>(window.getSize().y)));

        //update mouse pos
        static sf::Vector2i prevMousePosView;
        static sf::Vector2i currMousePosView;
        static sf::Vector2i currMousePosWindow;
        static sf::Vector2i prevMousePosWindow;
        updateMousePosView(prevMousePosView, currMousePosView, window, view);
        updateMousePosWindow(prevMousePosWindow, currMousePosWindow, window, view); 
        
        static sf::Vector2i mousePosScreen;
        static sf::Vector2i mousePosWindow;
        static sf::Vector2f mousePosView;
        // static sf::Vector2i mousePosGrid;

        mousePosScreen = sf::Mouse::getPosition();
        mousePosWindow = sf::Mouse::getPosition(window);
        window.setView(view);
        mousePosView = window.mapPixelToCoords(mousePosWindow);
        // mousePosGrid.x = mousePosView.x / grid.getGridSizeU();
        // mousePosGrid.y = mousePosView.y / grid.getGridSizeU();
        window.setView(window.getDefaultView());

        //update ui
        std::stringstream ss;
        // ss << "Screen: " << mousePosScreen.x << " " << mousePosScreen.y << "\n"
        //      << "Window: " << mousePosWindow.x << " " << mousePosWindow.y << "\n"
        //      << "View: " << mousePosView.x << " " << mousePosView.y << "\n"
        //      << "Grid: " << mousePosGrid.x << " " << mousePosGrid.y << "\n"
        //      << "CurrMousePos: " << currMousePosView.x << " " << currMousePosView.y << "\n";

        // ss << "ViewMousePos: " << mousePosView.x << " " << mousePosView.y << "\n";

        static bool action = false; //flag for the recalculating of the intersection area
        static bool isMouseButtonPressed = false; //flag for camera movement

        // ss << "DeltaX: "<< (prevMousePosWindow.x - currMousePosWindow.x) << " DeltaY: " << (prevMousePosWindow.y - currMousePosWindow.y) << "\n";
        
        sf::Event event;
        //event loop
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseMoved && polygons.size() > 0)
            {
                polygons[polygons.size() - 1].updateDynamicEdge(grid,currMousePosView);
            }

            if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                isMouseButtonPressed = false;
                float factor = 1/zoom.getFactor();
                int counter = zoom.getCount();
                auto temp = sf::Mouse::getPosition(window);
                view.move((prevMousePosWindow.x - currMousePosWindow.x) * pow(factor, counter), 
                          (prevMousePosWindow.y - currMousePosWindow.y) * pow(factor, counter));
            }
            else if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                isMouseButtonPressed = true;
            }

            if(isMouseButtonPressed && event.type == sf::Event::MouseButtonReleased)
            {

                if(polygons.size() == 0 || polygons.size() != 0 && polygons[polygons.size() - 1].isFinished())
                {
                    Polygon polygon;
                    polygon.addVert(currMousePosView, grid, action);
                    polygons.push_back(polygon);
                    isMouseButtonPressed = false;
                }
                else if (polygons.size() != 0 && !polygons[polygons.size() - 1].isFinished())
                {
                    polygons[polygons.size() - 1].addVert(currMousePosView, grid, action);
                    isMouseButtonPressed = false;
                }
            }

            if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::F11) && isFullscreen)
            {
                window.create(sf::VideoMode(SCREENW, SCREENH), WindowTitle, 7, sf::ContextSettings(24,8,8));
                window.setVerticalSyncEnabled(true);
                window.setKeyRepeatEnabled(false);
                isFullscreen = false;
            }
            else if(event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::F11) && !isFullscreen)
            {
                window.create(sf::VideoMode(SCREENW, SCREENH), WindowTitle, sf::Style::Fullscreen, sf::ContextSettings(24,8,8));
                window.setVerticalSyncEnabled(true);
                window.setKeyRepeatEnabled(false);
                isFullscreen = true;
            }

            if (event.type == sf::Event::MouseWheelMoved)
            {
                if (event.mouseWheel.delta > 0)
                    zoom.zoomIn(window, view);
                else if (event.mouseWheel.delta < 0)
                    zoom.zoomOut(window, view);
            }

            if (event.type == sf::Event::Resized)
            {
                zoom.zoomSet(window, view);
            }  

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
                         !isVertMoving)
            {
                movingVertIdx = findPolygonIdxOfVert(polygons, mousePosView);
                if(movingVertIdx.polygonIdx != -1 && movingVertIdx.vertIdx != -1)
                    isVertMoving = true;
            }
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
                        isVertMoving)
            {
                moveVert(polygons, grid, movingVertIdx, mousePosView);
                // ss << polygons[movingVertIdx.polygonIdx].getVerts().size();
                action = true;
            }
            else if(!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) &&
                        isVertMoving)
            {   
                action = false;
                isVertMoving = false;
                movingVertIdx = {-1, -1};
            }

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
            {
                polygons.clear();
                intersectionPoints.clear();
                intersectionPointsCoords.clear();
                redrawIntersectionArea = true;
            }
        }

        if (action)
        {
            findIntersectionPoints(polygons, intersectionPointsCoords, redrawIntersectionArea);
            action = false;
        }

        // mousePosGrid.x = floor(mousePosView.x / grid.getGridSizeU());
        // mousePosGrid.y = floor(mousePosView.y / grid.getGridSizeU());

        // tileSelector.setPosition(mousePosGrid.x * grid.getGridSizeF(), 
        //                         mousePosGrid.y * grid.getGridSizeF());

        //render begins
        window.clear();

        //bg
        window.setView(visibleArea);
        window.draw(background);
        
        //render game elements
        window.setView(view);
       
        grid.draw_axes(window, view, zoom.getCount());
        // window.draw(tileSelector);

        getIntersectionPoints(intersectionPointsCoords, intersectionPoints, redrawIntersectionArea);

        drawIntersectionArea(window, intersectionPoints);
        drawPolygons(polygons, window);
        drawIntersectionPoints(window, intersectionPoints);

        window.setView(visibleArea);

        //render ui
        text.setString(ss.str());
        window.draw(text);

        window.display();
        //render ends
    }
    return 0;
}