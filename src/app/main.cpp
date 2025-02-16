
//////////////////////////////////////////////////////////////////
// Headers
//////////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <ostream>
#include <string>
#include <iostream>
#include "display.hpp"
#include "utils/math.hpp"
#include "utils/data.hpp"
#include "3d/obj3d.hpp"

#include <chrono>
#include <cmath>



int main() {

    defineGlobal();

    //////////////////////////////////////////////////////////////////
    // Setup for SFML window and resolution
    //////////////////////////////////////////////////////////////////

    sf::Vector2f mousePos;
    sf::Vector2i resWindow;

    sf::RenderWindow window;
    resWindow = windowSetup(window, 1600, true, "CORONA",60);

    sf::RenderTexture rendWindow;
    rendWindow.create(resWindow.x,resWindow.y);


    camera cam;

    object3d object;
    object.load("../resources/objects/cow.obj");
    object.z = 10;
    object.x = 0;

    object3d object2;
    object2.load("../resources/objects/cow.obj");
    object2.z = 5;
    object2.x = -10;

    object3d object3;
    object3.load("../resources/objects/cow.obj");
    object3.z = 15;
    object3.x = 20;



    //////////////////////////////////////////////////////////////////
    // GLYPHS
    //////////////////////////////////////////////////////////////////

    // load font
    // sf::Font fontSmall;
    // if (!fontSmall.loadFromFile("../resources/font/small_pixel.ttf")) return 1;


    sf::Color col1 = rgb(40,150,140);
    // Rectangles to draw the grids
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(3,3));
    rect.setFillColor(col1);
    rect.setOutlineColor(sf::Color::Transparent);
    rect.setOutlineThickness(2);
    rect.setOrigin(1, 1);



    //////////////////////////////////////////////////////////////////
    // DRAW STATIC
    //////////////////////////////////////////////////////////////////




    //////////////////////////////////////////////////////////////////
    // WINDOW EVENT HANDLER
    //////////////////////////////////////////////////////////////////

    std::cout << "*****LOOP START*****" << std::endl;

    while (window.isOpen()) 
    {
        bool up = 0, down = 0, right = 0, left = 0, space = 0, keyA = 0, keyD = 0, user_input = 0;

        // Event handler
        sf::Event event; 
        while (window.pollEvent(event)) 
        {
            switch (event.type)
            {
                // If the X window button is pressed exit
                case sf::Event::Closed:
                    window.close();
                    break;

                // Keyboard input
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape) { window.close(); }
                    else if (event.key.code == sf::Keyboard::Up) { up = 1; user_input = 1;}
                    else if (event.key.code == sf::Keyboard::Down) { down = 1; user_input = 1;}
                    else if (event.key.code == sf::Keyboard::Right) { right = 1; user_input = 1;}
                    else if (event.key.code == sf::Keyboard::Left) { left = 1; user_input = 1;}
                    else if (event.key.code == sf::Keyboard::Space) { space = 1; user_input = 1;}
                    else if (event.key.code == sf::Keyboard::A) { keyA = 1; user_input = 1;}
                    else if (event.key.code == sf::Keyboard::D) { keyD = 1; user_input = 1;}
                    break;

                default:
                    // Do nothing
                    break;
            }
        }

        //////////////////////////////////////////////////////////////////
        // MAIN LOOP
        //////////////////////////////////////////////////////////////////





        //////////////////////////////////////////////////////////////////
        // UPDATE
        //////////////////////////////////////////////////////////////////    

        // Create a vector with the pixel coord's in the actual window not the display
        mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        float move_x = 0;
        float move_z = 0;

        move_z += (up - down)*.1;
        move_x += (left - right)*.1;
        cam.v += (keyA - keyD)*.1;

        // Update if a key is pressed
        cam.move(move_x,0,move_z);
        cam.update();



        //////////////////////////////////////////////////////////////////
        // DRAW
        ////////////////////////////////////////////////////////////////// 

        // Move the rectangle to the correct position before drawing
        rendWindow.clear(black);
        rect.setPosition(mousePos.x, mousePos.y);
        rendWindow.draw(rect);

        rect.setPosition(1,1);
        rendWindow.draw(rect);

        rect.setPosition(resWindow.x-2,resWindow.y-2);
        rendWindow.draw(rect);


        object.draw (rendWindow,resWindow,cam,red);
        object2.draw (rendWindow,resWindow,cam,yellow);
        object3.draw (rendWindow,resWindow,cam,green);

        //////////////////////////////////////////////////////////////////
        // DISPLAY TO SCREEN
        //////////////////////////////////////////////////////////////////

        // Display canvas layers to screen
        rendWindow.display();
        window.draw(sf::Sprite(rendWindow.getTexture()));
        window.display();
    }

    //////////////////////////////////////////////////////////////////
    // DEBUG OUTPUTS
    /////////////////////////////////////////////////////////////////

    // for (float i = 0;i < 8.0; i += .45) {
    //     std::cout << math_sin(i) << std::endl;
    //     std::cout << sin(i) << std::endl;
    // }


    // auto start = std::chrono::high_resolution_clock::now();
    // for (float i = 0;i < 1000000; i ++) {
    //     math_sin(i);
    // }
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << duration.count() << std::endl;


    // std::cout << iRand(1,200,26123489) << std::endl;
    // std::cout << iRand(1,200,73246922) << std::endl;
    // std::cout << iRand(1,200,19232143) << std::endl;
    // std::cout << iRand(1,200,48572734) << std::endl;

    std::cout << "*******END******* "<< std::endl;

    return 0;
}
