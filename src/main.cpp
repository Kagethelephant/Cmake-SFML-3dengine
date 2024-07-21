#include "main.hpp"



int main() {

    //////////////////////////////////////////////////////////////////
    // Setup for SFML window and resolution
    //////////////////////////////////////////////////////////////////

    sf::Vector2f mousePos;
    sf::Vector2i resWindow;
    
    sf::RenderWindow window;
    resWindow = windowSetup(window, 500, true, "CORONA",60);

    sf::RenderTexture rendWindow;
    rendWindow.create(resWindow.x,resWindow.y);




    object3d cube;

    float phi = 3.14159 * std::sqrt(5)-1;
    int n = 400;

    for(int i=0; i<n; i++)
    {
        float x, y, z;
        y = 1 - (i / float(n - 1)) * 2;
        float rad = std::sqrt(1 - y * y);

        float theta = phi * i;

        x = mat_cos(theta) * rad;
        z = mat_sin(theta) * rad;

        object3d::point pnew;
        pnew.x = x; pnew.y = y; pnew.z = z;

        cube.pointCloud.push_back(pnew);
    }


    float lo = -1.0f;
    float hi = 1.0f;
    cube.mesh = {

        // Front
        {hi,hi,lo,   lo,hi,lo,   hi,lo,lo}, 
        {lo,lo,lo,   hi,lo,lo,   lo,hi,lo},

        // Back
        {lo,hi,hi,   hi,hi,hi,   lo,lo,hi}, 
        {hi,lo,hi,   lo,lo,hi,   hi,hi,hi},

        // Righ
        {hi,hi,hi,   hi,hi,lo,   hi,lo,hi}, 
        {hi,lo,lo,   hi,lo,hi,   hi,hi,lo},

        // Left
        {lo,hi,lo,   lo,hi,hi,   lo,lo,lo}, 
        {lo,lo,hi,   lo,lo,lo,   lo,hi,hi},

        // Top
        {lo,lo,lo,   lo,lo,hi,   hi,lo,lo}, 
        {hi,lo,hi,   hi,lo,lo,   lo,lo,hi},

        // Bottom
        {hi,hi,lo,   hi,hi,hi,   lo,hi,lo}, 
        {lo,hi,hi,   lo,hi,lo,   hi,hi,hi},
    };

    



    //////////////////////////////////////////////////////////////////
    // GLYPHS
    //////////////////////////////////////////////////////////////////

    // // load font
    // sf::Font fontSmall;
    // if (!fontSmall.loadFromFile("../resources/font/small_pixel.ttf")) return 1;
    
    // // Text for the cursor position and debugging
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

    float U = 0;
    float V = 0;
    float W = 0;
    float Z = 3;


    //////////////////////////////////////////////////////////////////
    // DRAW STATIC
    //////////////////////////////////////////////////////////////////




    //////////////////////////////////////////////////////////////////
    // WINDOW EVENT HANDLER
    //////////////////////////////////////////////////////////////////

    std::cout << "*****LOOP START*****" << std::endl;

    while (window.isOpen()) 
    {
        bool up = 0, down = 0, right = 0, left = 0, space = 0, keyA = 0, keyD = 0;
        
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
                    else if (event.key.code == sf::Keyboard::Up) { up = 1; }
                    else if (event.key.code == sf::Keyboard::Down) { down = 1; }
                    else if (event.key.code == sf::Keyboard::Right) { right = 1; }
                    else if (event.key.code == sf::Keyboard::Left) { left = 1; }
                    else if (event.key.code == sf::Keyboard::Space) { space = 1; }
                    else if (event.key.code == sf::Keyboard::A) { keyA = 1; }
                    else if (event.key.code == sf::Keyboard::D) { keyD = 1; }
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
        
        U += (down - up)*.1;
        V += (left - right)*.1;
        Z += (keyD - keyA)*.1;

        if(space)
        {
            U = 0;
            V = 0;
            Z = 0;
        }

        

        //////////////////////////////////////////////////////////////////
        // DRAW
        ////////////////////////////////////////////////////////////////// 

        // Move the rectangle to the correct position before drawing
        rendWindow.clear(c_color(Black));
        rect.setPosition(mousePos.x, mousePos.y);
        rendWindow.draw(rect);
        cube.drawPointCloud(rendWindow,U,V,W,Z);


        

        //////////////////////////////////////////////////////////////////
        // DISPLAY TO SCREEN
        //////////////////////////////////////////////////////////////////

        // Display canvas layers to screen
        //canvas.display(window);
        rendWindow.display();
        window.draw(sf::Sprite(rendWindow.getTexture()));
        window.display();
    }

    //////////////////////////////////////////////////////////////////
    // DEBUG OUTPUTS
    /////////////////////////////////////////////////////////////////

    float rad = .66;

    std::cout << "*****END***** "<< "\n" << std::endl;
    std::cout << mat_sin(rad*3.14159) << std::endl;
    std::cout << mat_cos(rad*3.14159) << std::endl;
    std::cout << std::sinf(rad*3.14159) << std::endl;
    std::cout << std::cosf(rad*3.14159) << std::endl;


    return 0;
}