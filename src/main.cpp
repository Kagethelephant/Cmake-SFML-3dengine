#include "main.hpp"



int main() {

    //////////////////////////////////////////////////////////////////
    // Setup for SFML window and resolution
    //////////////////////////////////////////////////////////////////

    sf::Vector2f mousePos;
    sf::Vector2i resWindow;
    
    sf::RenderWindow window;
    resWindow = windowSetup(window, 400, true, "CORONA",60);

    sf::RenderTexture rendWindow;
    rendWindow.create(resWindow.x,resWindow.y);


    rend3d::object3d cube;

    // int N = 400;
    // float dlong = 3.151592653*(3-sqrt(5));  /* ~2.39996323 */
    // float dz = 2.0/N;
    // float lng = 0.0f;
    // float z = 1 - dz/2;
    // for (int k = 0; k < N; k++)
    // {
    //     float r  = sqrt(1-z*z);
    //     vector3 pnew;
    //     pnew.x = mat_cos(lng)*r; pnew.y = mat_sin(lng)*r; pnew.z = z;
    //     cube.cloud.push_back(pnew);
    //     z  = z - dz;
    //     lng += dlong;
    // }


    cube.loadObj("../resources/objects/cow.obj");

    // float l = -1.0f;
    // float h = 1.0f;
    // cube.mesh = {
    //     // Front
    //     rend3d::triangle(vector3(h,h,l),   vector3(l,h,l),   vector3(l,l,l)),  
    //     rend3d::triangle(vector3(l,l,l),   vector3(h,l,l),   vector3(h,h,l)),
    //     // Back
    //     rend3d::triangle(vector3(h,l,h),   vector3(l,l,h),   vector3(l,h,h)), 
    //     rend3d::triangle(vector3(l,h,h),   vector3(h,h,h),   vector3(h,l,h)),
    //     // Right
    //     rend3d::triangle(vector3(h,h,h),   vector3(h,h,l),   vector3(h,l,l)), 
    //     rend3d::triangle(vector3(h,l,l),   vector3(h,l,h),   vector3(h,h,h)),
    //     // Left
    //     rend3d::triangle(vector3(l,h,l),   vector3(l,h,h),   vector3(l,l,h)), 
    //     rend3d::triangle(vector3(l,l,h),   vector3(l,l,l),   vector3(l,h,l)),
    //     // Top
    //     rend3d::triangle(vector3(l,l,l),   vector3(l,l,h),   vector3(h,l,h)), 
    //     rend3d::triangle(vector3(h,l,h),   vector3(h,l,l),   vector3(l,l,l)),
    //     // Bottom
    //     rend3d::triangle(vector3(l,h,h),   vector3(l,h,l),   vector3(h,h,l)), 
    //     rend3d::triangle(vector3(h,h,l),   vector3(h,h,h),   vector3(l,h,h)),
    // };



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

    float U = 0;
    float V = 0;
    float W = 0;
    float Z = 3;

    float theta = 0;
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
                    else if (event.key.code == sf::Keyboard::Up) { up = 1; cube.rotateObject(U,V,W);}
                    else if (event.key.code == sf::Keyboard::Down) { down = 1; cube.rotateObject(U,V,W);}
                    else if (event.key.code == sf::Keyboard::Right) { right = 1; cube.rotateObject(U,V,W);}
                    else if (event.key.code == sf::Keyboard::Left) { left = 1; cube.rotateObject(U,V,W);}
                    else if (event.key.code == sf::Keyboard::Space) { space = 1; }
                    else if (event.key.code == sf::Keyboard::A) { keyA = 1; cube.rotateObject(U,V,W);}
                    else if (event.key.code == sf::Keyboard::D) { keyD = 1; cube.rotateObject(U,V,W);}
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
        
        //theta += (down - up)*1;
        U += (down - up)*.1;
        V += (left - right)*.1;
        Z += (keyD - keyA)*.1;

        // Reset view
        if(space){U = 0; V = 0; Z = 100;}
        

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

        cube.drawMesh(rendWindow,resWindow,Z);

        

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

    std::cout << "*****END***** "<< "\n" << std::endl;

    return 0;
}