#include "main.hpp"



int main() {

    // Test on the other side

    float test = 2;

    //////////////////////////////////////////////////////////////////
    // Setup for SFML window and resolution
    //////////////////////////////////////////////////////////////////

    sf::Vector2f mousePos;
    sf::Vector2i resWindow;
    
    sf::RenderWindow window;
    resWindow = windowSetup(window, 400, true, "CORONA",60);

    sf::RenderTexture rendWindow;
    rendWindow.create(resWindow.x,resWindow.y);

    // int N = 400;
    // float dlong = 3.151592653*(3-sqrt(5));  /* ~2.39996323 */
    // float dz = 2.0/N;
    // float lng = 0.0f;
    // float z = 1 - dz/2;
    // for (int k = 0; k < N; k++)
    // {
    //     float r  = sqrt(1-z*z);
    //     vector3 pnew;
    //     pnew.x = math_cos(lng)*r; pnew.y = math_sin(lng)*r; pnew.z = z;
    //     cube.cloud.push_back(pnew);
    //     z  = z - dz;
    //     lng += dlong;
    // }

    
    camera cam;
    
    object3d object;
    object.load("../resources/objects/cow.obj");
    object.z = 10;

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
    
    // Text for the cursor position and debugging
    // sf::Text textSmall;
    // textSmall.setFont(fontSmall);
    // textSmall.setString("Hello world");
    // textSmall.setCharacterSize(8);
    // textSmall.setFillColor(c_color(White));
    // textSmall.setStyle(sf::Text::Regular);
    // textSmall.setPosition(5, 5);

    // Rectangles to draw the grids
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(3,3));
    rect.setFillColor(c_color(White));
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
        rendWindow.clear(c_color(Black));
        rect.setPosition(mousePos.x, mousePos.y);
        rendWindow.draw(rect);

        rect.setPosition(1,1);
        rendWindow.draw(rect);
        
        rect.setPosition(resWindow.x-2,resWindow.y-2);
        rendWindow.draw(rect);

        // textSmall.setPosition(5,5);
        // textSmall.setString("Somthing");
        // rendWindow.draw(textSmall);
        

        object.draw (rendWindow,resWindow,cam);
        object2.draw (rendWindow,resWindow,cam);
        object3.draw (rendWindow,resWindow,cam);
        
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

    std::cout << "*******END******* "<< std::endl;
    
    // std::cout << "      v                           __   _      "<< std::endl;
    // std::cout << "    > 0 <    ___          v      (  )_( )_    "<< std::endl;
    // std::cout << "      ^     (    )_    v    v   (   __   _)   "<< std::endl;
    // std::cout << "           _(   (  _)          (______) (__)  "<< std::endl;
    // std::cout << "          (___________)                       "<< std::endl;
    // std::cout << "              ____                            "<< std::endl;
    // std::cout << " ---------:--/  | \--p -----------------------"<< std::endl;
    // std::cout << " -  -  -  '-O------O-'   -  -  -  -  -  -  -  "<< std::endl;
    // std::cout << "----------------------------------------------"<< std::endl;

    return 0;
}