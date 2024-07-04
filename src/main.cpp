#include "main.hpp"



// Create the grid here so it stays in the heap

int main() {

    std::cout << "\n" << "\n" << "*****START*****" << std::endl;

    //-----VIEW SETUP/SFML-----

    // Only window view for the game
    sf::RenderWindow window; 

    // Container for the height and width of the window
    sf::Vector2i resPixels;
    resPixels = windowSetup(window, 500, true, "CORONA",60);

    // Position for mouse in view pixels instead of screen pixels
    sf::Vector2f mousePos;

     // Used to handle layers
    Canvas canvas(resPixels.x, resPixels.y);



    //-----GLYPHS-----

    // // load font
    // sf::Font fontSmall;
    // if (!fontSmall.loadFromFile("../resources/font/small_pixel.ttf")) return 1;
    
    // // Text for the cursor position and debugging
    // sf::Text textSmall;
    // textSmall.setFont(fontSmall);
    // textSmall.setString("Hello world");
    // textSmall.setCharacterSize(8);
    // textSmall.setFillColor(c_color("white"));
    // textSmall.setStyle(sf::Text::Regular);
    // textSmall.setPosition(5, 5);

    // Rectangles to draw the grids
    sf::RectangleShape rect;
    rect.setSize(sf::Vector2f(3,3));
    rect.setFillColor(c_color("blue"));
    rect.setOutlineColor(sf::Color::Transparent);
    rect.setOutlineThickness(2);
    rect.setOrigin(1, 1);



    //----DRAW STATIC----
    canvas.reset(0,false);
    canvas.clear(0, c_color("black"));




    //----WINDOW EVENTS---- 

    std::cout << "*****LOOP START*****" << std::endl;

    while (window.isOpen()) 
    {
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

                // If the escape key is pressed exit
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape) { window.close(); }
                    break;
            }
        }

        //----MAIN LOOP----




        //----UPDATE----




        //----UPDATE MAKE CALCULATIONS-----     

        // Create a vector with the pixel coord's in the actual window not the display
        mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));




        //----DRAW UPDATE-----
        


        // Move the rectangle to the correct position before drawing
        rect.setPosition(0, 0);
        canvas.draw(1, rect);

        rect.setPosition(0, resPixels.y-1);
        canvas.draw(1, rect);

        rect.setPosition(resPixels.x-1, resPixels.y-1);
        canvas.draw(1, rect);

        rect.setPosition(resPixels.x-1, 0);
        canvas.draw(1, rect);


        rect.setPosition(mousePos.x, mousePos.y);
        canvas.draw(2, rect);



        //----DISPLAY THE STUFF----

        // Display canvas layers to screen
        canvas.display(window);
        window.display();
    }




    //----DEBUGGING OUTPUTS----

    std::cout << "*****END***** "<< "\n" << std::endl;


    return 0;
}