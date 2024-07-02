#include "main.hpp"



// Create the grid here so it stays in the heap

int main() {


    //----VIEW SETUP/SFML----
    sf::Vector2f mousePos;

    // Only window view for the game
    sf::RenderWindow window; 

    // Container for the height and width of the window
    sf::Vector2i resPixels;
    resPixels = windowSetup(window, 500, true,30);


    // Create the buffers and sprites used to draw the map and gui
    sf::RenderTexture bufferMap;
    sf::RenderTexture bufferGUI;

    bufferMap.create(resPixels.x, resPixels.y);
    bufferGUI.create(resPixels.x, resPixels.y);

     
    Canvas canvas(resPixels.x, resPixels.y);


    //----GLYPHS----

    //load font
    sf::Font fontSmall;
    if (!fontSmall.loadFromFile("../resources/font/small_pixel.ttf")) return 1;
    
    //Text for the cursor position and debugging
    sf::Text textSmall;
    textSmall.setFont(fontSmall);
    textSmall.setString("Hello world");
    textSmall.setCharacterSize(8);
    textSmall.setFillColor(sf::Color(G_white_x, G_white_y, G_white_z));
    textSmall.setStyle(sf::Text::Regular);
    textSmall.setPosition(5, 5);

    // Rectangles to draw the grids
    sf::RectangleShape rectCursor;
    rectCursor.setSize(sf::Vector2f(3,3));
    rectCursor.setFillColor(sf::Color(G_white_x, G_white_y, G_white_z));
    rectCursor.setOutlineColor(sf::Color::Transparent);
    rectCursor.setOutlineThickness(2);
    rectCursor.setOrigin(1, 1);



    //----DRAW STATIC----


    //----MAIN LOOP----

    while (window.isOpen()) {

        //----WINDOW EVENTS----

        // Event handler
        sf::Event event; 
        while (window.pollEvent(event)) {

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




        //----UPDATE----



      


        //----UPDATE MAKE CALCULATIONS-----     

        // Create a vector with the pixel coord's in the actual window not the display
        mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));





        //----DRAW UPDATE-----
      
        // Clear the map view
        bufferMap.clear(sf::Color(G_black_x, G_black_y, G_black_z));
        // Clear the GUI clear so it doesnt cover up the map view
        bufferGUI.clear(sf::Color::Transparent);

        //// Draw some debugging text
        //textSmall.setPosition(5, 5);
        //textSmall.setString("Some String:" + some_var);
        //bufferGUI.draw(textSmall);

        //textSmall.setPosition(5, 15);
        //textSmall.setString("Some String:" + some_var);
        //bufferGUI.draw(textSmall);


        //Move the rectangle to the correct position before drawing
        rectCursor.setPosition(0, 0);
        bufferGUI.draw(rectCursor);

        rectCursor.setPosition(0, resPixels.y-1);
        bufferGUI.draw(rectCursor);

        rectCursor.setPosition(resPixels.x-1, resPixels.y-1);
        bufferGUI.draw(rectCursor);

        rectCursor.setPosition(resPixels.x-1, 0);
        bufferGUI.draw(rectCursor);


        rectCursor.setPosition(mousePos.x, mousePos.y);
        bufferGUI.draw(rectCursor);


        
        //// Draw stuff on to a surface so we can save it
        //canvas.draw(rectCursor, 0);
        //canvas.draw(selectionS, 1);



        //----DISPLAY THE STUFF----
        
        // Display the buffer and draw the buffer
        bufferMap.display(); 
        window.draw(sf::Sprite(bufferMap.getTexture()));


       
        // Same thing for the GUI buffer
        bufferGUI.display(); 
        window.draw(sf::Sprite(bufferGUI.getTexture()));


        //// Display the window to the screen
        //window.display();
        canvas.display(window);
    }




    //----DEBUGGING OUTPUTS----

    std::cout << "*****GAME TERMINATED***** " << std::endl;


    return 0;
}