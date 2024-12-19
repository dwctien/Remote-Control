#include "app_interface.h"
#include "globals.h"

std::thread threadCheckMail;

bool isMouseOver(const sf::RectangleShape& object, const sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return object.getGlobalBounds().contains(mousePos);
}

void updateButtonTexture(vector<sf::RectangleShape*> Button, vector<sf::Texture>& texHover, vector<sf::Texture>& texNormal, sf::RenderWindow& window) {
    for (int i = 0; i < Button.size(); i++) {
        if (isMouseOver(*Button[i], window)) {
            Button[i]->setTexture(&texHover[i]);
        }
        else {
            Button[i]->setTexture(&texNormal[i]);
        }
    }
}

void draw(vector<sf::RectangleShape*> button, sf::RenderWindow& window) {
    if (isRunning) {
        window.draw(*button[button.size() - 1]);
    }
    else {
        window.draw(*button[0]);
    }
    for (int i = 1; i < button.size() - 1; i++) {
        window.draw(*button[i]);
    }
}

void setButtonPosition(vector<sf::RectangleShape*> button) {
    float x = 275, y = 250;
    button[button.size() - 1]->setPosition(x, y);
    for (int i = 0; i < button.size() - 1; i++) {
        button[i]->setPosition(x, y + i * 100);
    }
}

void setButtonSize(vector<sf::RectangleShape*> button) {
    sf::Vector2f size(250.f, 70.f);
    for (int i = 0; i < button.size(); i++) {
        button[i]->setSize(size);
    }
}

bool loadButtonImages(vector<sf::Texture>& texNormal, vector<sf::Texture>& texHover, vector<string> buttonName) {
    for (int i = 0; i < texNormal.size(); i++) {
        if (!texNormal[i].loadFromFile("graphic/" + buttonName[i] + "_button.png") ||
            !texHover[i].loadFromFile("graphic/" + buttonName[i] + "_button_hover.png")) {
            std::cout << "Failed to load button images!" << std::endl;
            return false;
        }
    }
    return true;
}

void handleRunButton(sf::RectangleShape& runButton) {
    if (!isRunning) {
        isRunning = true;
        threadCheckMail = std::thread(checkMailsContinuously);
    }
}

void handleStopButton(sf::RectangleShape& stopButton, sf::RenderWindow& window) {
    if (isRunning) {
        isRunning = false;
        if (threadCheckMail.joinable()) {
            threadCheckMail.join();
        }
    }
}

void handleExitButton(sf::RenderWindow& window) {
    if (isRunning) {
        isRunning = false;
        if (threadCheckMail.joinable()) {
            threadCheckMail.join();
        }
    }
    window.close();
}

void handleAboutUsButton(sf::RenderWindow& window) {
    sf::Texture aboutUsTexture, backTextureNormal, backTextureHover;

    if (!aboutUsTexture.loadFromFile("graphic/about_us_image.png") ||
        !backTextureNormal.loadFromFile("graphic/back_button.png") ||
        !backTextureHover.loadFromFile("graphic/back_button_hover.png")) {
        std::cerr << "Failed to load image!" << std::endl;
        return;
    }

    sf::Sprite background(aboutUsTexture);
    sf::RectangleShape backButton(sf::Vector2f(250.f, 70.0f));
    backButton.setPosition(500, 500);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                if (threadCheckMail.joinable()) {
                    threadCheckMail.join();
                }
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (backButton.getGlobalBounds().contains(mousePos)) {
                    return;
                }
            }
        }
        if (isMouseOver(backButton, window)) {
            backButton.setTexture(&backTextureHover);
        }
        else {
            backButton.setTexture(&backTextureNormal);
        }
        window.clear();
        window.draw(background);
        window.draw(backButton);
        window.display();
    }
}

void createText(sf::Text& inputText, const sf::Font& font, int sz, float x, float y) {
    inputText.setFont(font);
    inputText.setCharacterSize(sz);
    inputText.setFillColor(sf::Color::Black);
    inputText.setPosition(x, y);
}

bool isValidEmail(const std::string& email) {
    const std::regex emailPattern(
        R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
    );
    return std::regex_match(email, emailPattern);
}

void handleEmailScrolling(sf::Event event, sf::RectangleShape& emailBox, int& scrollOffset, const std::vector<std::string>& emails, size_t maxEmailsToDisplay) {
    if (emailBox.getGlobalBounds().contains(event.mouseWheelScroll.x, event.mouseWheelScroll.y)) {
        if (event.mouseWheelScroll.delta > 0) {
            if (scrollOffset > 0) {
                scrollOffset--; // Scroll up
            }
        }
        else {
            if (scrollOffset < emails.size() - maxEmailsToDisplay) {
                scrollOffset++; // Scroll down
            }
        }
    }
}

void handleTextInput(sf::Event event, std::string& userInput, sf::Text& inputText, std::vector<std::string>& emails, bool& isTyping) {
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode < 128) {     // Check for valid character
            if (event.text.unicode == 8) {  // Handle Backspace key
                if (!userInput.empty()) {
                    userInput.pop_back();   // Remove the last character
                }
            }
            else if (userInput.length() < 30) { // Limit the length of the text
                userInput += static_cast<char>(event.text.unicode);
            }
            inputText.setString(userInput); // Update the text in the textBox
        }

        // If Enter is pressed, add email to the list (do not include Enter character)
        if (event.text.unicode == 13) { // Enter key
            if (!userInput.empty()) {
                // Remove invalid characters
                userInput.erase(std::remove(userInput.begin(), userInput.end(), '\n'), userInput.end());
                userInput.erase(std::remove(userInput.begin(), userInput.end(), '\r'), userInput.end());
                if (isValidEmail(userInput)) {
                    emails.push_back(userInput);    // Add the string to the vector
                }
                userInput.clear();                  // Clear the input string after adding
                inputText.setString(userInput);     // Update the textBox
            }
            isTyping = false;
        }
    }
}


void handleMouseClickEvents(sf::Event event, bool& isTyping, string& userInput, sf::Text& inputText,
    vector<std::string>& emails, sf::RectangleShape& textBox, sf::RectangleShape& plusButton, sf::RectangleShape& minusButton) {

    if (textBox.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
        isTyping = true;
    }
    else {
        isTyping = false;
    }

    if (plusButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
        if (!userInput.empty()) {
            if (isValidEmail(userInput)) {
                emails.push_back(userInput);    // Add the string to the vector
            }
            inputText.setString("");            // Clear the input text after adding (not clearing userInput)
            userInput = "";
            isTyping = false;
        }
    }

    if (minusButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
        if (!userInput.empty()) {
            auto it = std::find(emails.begin(), emails.end(), userInput);
            if (it != emails.end()) {
                emails.erase(it);               // Remove the string from the vector if found
            }
            inputText.setString("");            // Clear the text in the text box after removal
            userInput = "";
            isTyping = false;
        }
    }
}


void updateCursor(bool isTyping, sf::Clock& clock, bool& cursorVisible, sf::RectangleShape& cursor, const sf::Text& inputText) {
    // Show or hide the blinking cursor every 0.5 seconds, only while typing
    if (isTyping && clock.getElapsedTime().asSeconds() >= 0.5f) {
        cursorVisible = !cursorVisible; // Toggle cursor visibility state
        clock.restart();
    }

    // Update the cursor position (place the cursor after the text)
    cursor.setPosition(190 + inputText.getGlobalBounds().width, 223);
}


void renderEmailList(sf::RenderWindow& window, const sf::Font& font, const std::vector<std::string>& emails, int maxEmailsToDisplay, int scrollOffset) {
    sf::Text email;
    if (emails.empty()) {
        createText(email, font, 20, 205, 327);
        email.setString("Empty");
        window.draw(email);
    }
    else {
        for (size_t i = 0; i < maxEmailsToDisplay; ++i) {
            if (i + scrollOffset < emails.size()) {
                createText(email, font, 20, 205, 327 + i * 30);
                email.setString(emails[i + scrollOffset]);
                window.draw(email);
            }
        }
    }
}

vector<string> loadAdmin() {
    vector<string> admin;
    ifstream file("admin.txt");
    if (!file.is_open()) {
        cout << "Cannot open admin file" << endl;
    }
    else {
        string line;
        while (getline(file, line)) {
            if (!line.empty())
                admin.push_back(line);
        }
        file.close();
    }
    return admin;
}

bool updateAdmin(vector<string> gmails) {
    ofstream file("admin.txt");
    if (!file.is_open()) {
        cout << "Cannot open admin file" << endl;
        return false;
    }
    else {
        for (int i = 0; i < gmails.size(); i++) {
            file << gmails[i] << endl;
        }
        file.close();
        return true;
    }
}

void handleConfigButton(sf::RenderWindow& window) {

    sf::Font font;
    sf::Clock clock;
    sf::Text inputText;
    std::string userInput = "";
    std::vector<std::string> emails;
    bool isTyping = false, cursorVisible = true;
    int scrollOffset = 0, maxEmailsToDisplay = 9;
    sf::Texture textBoxTexture, backgroundTexture, emailBoxTexture, backTextureNormal, backTextureHover;
    sf::Texture plusTextureNormal, plusTextureHover, minusTextureNormal, minusTextureHover;
    sf::RectangleShape textBox(sf::Vector2f(375, 50)), cursor(sf::Vector2f(2, 22)), plusButton(sf::Vector2f(40, 40));
    sf::RectangleShape emailBox(sf::Vector2f(470, 300)), backButton(sf::Vector2f(250, 70)), minusButton(sf::Vector2f(40, 40));

    vector<sf::RectangleShape*> buttons = { &plusButton, &minusButton, &backButton };
    vector<string> buttonNames = { "plus", "minus", "back" };
    vector<sf::Texture> texNormal = { plusTextureNormal, minusTextureNormal, backTextureNormal };
    vector<sf::Texture> texHover = { plusTextureHover, minusTextureHover, backTextureHover };

    if (!loadButtonImages(texNormal, texHover, buttonNames))
        return;


    if (!backgroundTexture.loadFromFile("graphic/config_image.png") ||
        !textBoxTexture.loadFromFile("graphic/textbox_button.png") ||
        !emailBoxTexture.loadFromFile("graphic/emailbox.png") ||
        !font.loadFromFile("graphic/arial.ttf"))
        return;

    sf::Sprite background(backgroundTexture);

    textBox.setTexture(&textBoxTexture);
    emailBox.setTexture(&emailBoxTexture);
    plusButton.setTexture(&texNormal[0]);
    minusButton.setTexture(&texNormal[1]);
    backButton.setTexture(&texNormal[2]);

    textBox.setPosition(180, 210);
    plusButton.setPosition(565, 213);
    minusButton.setPosition(615, 213);
    emailBox.setPosition(180, 310);
    backButton.setPosition(275, 650);

    cursor.setFillColor(sf::Color::Black);

    createText(inputText, font, 20, 190, 220);

    emails = loadAdmin();

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Check if the mouse clicked on the textBox
            if (event.type == sf::Event::MouseButtonPressed) {
                if (backButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                    updateAdmin(emails);
                    return;
                }
                else {
                    handleMouseClickEvents(event, isTyping, userInput, inputText, emails, textBox, plusButton, minusButton);
                }
            }

            // Handle mouse wheel scrolling
            if (event.type == sf::Event::MouseWheelScrolled) {
                handleEmailScrolling(event, emailBox, scrollOffset, emails, maxEmailsToDisplay);
            }

            // If the user is typing text
            if (isTyping) {
                handleTextInput(event, userInput, inputText, emails, isTyping);
            }

            if (isMouseOver(backButton, window)) {
                backButton.setTexture(&backTextureHover);
            }
            else {
                backButton.setTexture(&backTextureNormal);
            }

        }

        updateButtonTexture(buttons, texHover, texNormal, window);
        updateCursor(isTyping, clock, cursorVisible, cursor, inputText);

        window.clear();

        window.draw(background);
        window.draw(textBox);
        window.draw(inputText);
        window.draw(emailBox);
        window.draw(plusButton);
        window.draw(minusButton);
        window.draw(backButton);

        if (cursorVisible && isTyping) {
            window.draw(cursor);
        }

        renderEmailList(window, font, emails, maxEmailsToDisplay, scrollOffset);
        window.display();
    }
}

void handle(sf::Event event, sf::RenderWindow& window, vector<sf::RectangleShape*> button) {

    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (button[0]->getGlobalBounds().contains(mousePos) && !isRunning) {
            handleRunButton(*button[0]);
        }
        else if (button[1]->getGlobalBounds().contains(mousePos)) {
            handleExitButton(window);
        }
        else if (button[2]->getGlobalBounds().contains(mousePos)) {
            handleAboutUsButton(window);
        }
        else if (button[3]->getGlobalBounds().contains(mousePos)) {
            //handleAddAdmin("blabla@gmail.com", "127.0.0.1");
        }
        else if (button[4]->getGlobalBounds().contains(mousePos)) {
            handleConfigButton(window);
        }
        else if (button[button.size() - 1]->getGlobalBounds().contains(mousePos) && isRunning) {
            handleStopButton(*button[button.size() - 1], window);
        }
    }
    else if (event.type == sf::Event::Closed) {
        handleExitButton(window);
    }
}

void runApp() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "REMOTE CONTROL WITH GMAIL", sf::Style::Close | sf::Style::Titlebar);
    sf::RectangleShape runButton, exitButton, aboutUsButton, instrucButton, configButton, stopButton;
    sf::Texture backgroundTexture;
    sf::Texture runTextureNormal, runTextureHover;
    sf::Texture stopTextureNormal, stopTextureHover;
    sf::Texture aboutUsTextureNormal, aboutUsTextureHover;
    sf::Texture exitTextureNormal, exitTextureHover;
    sf::Texture instrucTextureNormal, instrucTextureHover;
    sf::Texture configTextureNormal, configTextureHover;

    vector<sf::RectangleShape*> button = { &runButton, &exitButton, &aboutUsButton, &instrucButton, &configButton , &stopButton };
    vector<sf::Texture> texNormal = { runTextureNormal , exitTextureNormal, aboutUsTextureNormal, instrucTextureNormal, configTextureNormal, stopTextureNormal };
    vector<sf::Texture> texHover = { runTextureHover , exitTextureHover, aboutUsTextureHover, instrucTextureHover, configTextureHover, stopTextureHover };
    vector<std::string> buttonNames = { "run", "exit", "about_us", "instruc", "config" , "stop" };

    if (!backgroundTexture.loadFromFile("graphic/background.png"))
        return;

    sf::Sprite background(backgroundTexture);

    if (!loadButtonImages(texNormal, texHover, buttonNames))
        return;

    setButtonPosition(button);
    setButtonSize(button);

    bool isAppRunning = false;

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            handle(event, window, button);
        }

        updateButtonTexture(button, texHover, texNormal, window);

        window.clear(sf::Color::Black);
        window.draw(background);
        draw(button, window);
        window.display();
    }
}