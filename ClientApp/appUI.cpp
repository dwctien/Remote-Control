#include "AppUI.h"
#include "globals.h"

std::thread threadCheckMail;
std::mutex mtx;

void handleRunButton(bool& run, Button& button, sf::RenderWindow& window) {
    run = true;
    button.render(window);
    if (!isRunning) {
        isRunning = true;
        threadCheckMail = std::thread(checkMailsContinuously);
    }
}

void handleStopButton(bool& run, Button& button, sf::RenderWindow& window) {
    run = false;
    mtx.lock();
    button.sprite.setTexture(button.textureIdle);
    button.render(window);
    window.display();
    if (isRunning) {
        isRunning = false;
        if (threadCheckMail.joinable()) {
            threadCheckMail.join();
        }
    }
    mtx.unlock();
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

// ================== CLASS BUTTON ==================

Button::Button(const std::string& idleImagePath, const std::string& activeImagePath, float x, float y)
    : isHovered(false) {
    if (!textureIdle.loadFromFile(idleImagePath)) {
        throw std::runtime_error("Failed to load idle image: " + idleImagePath);
    }
    if (!textureActive.loadFromFile(activeImagePath)) {
        throw std::runtime_error("Failed to load active image: " + activeImagePath);
    }

    sprite.setTexture(textureIdle); // Set the default texture to Idle
    sprite.setPosition(x, y);
}

bool Button::isClicked(const sf::Vector2f& mousePos) const {
    return sprite.getGlobalBounds().contains(mousePos);
}

void Button::update(const sf::Vector2f& mousePos) {
    if (sprite.getGlobalBounds().contains(mousePos)) {
        isHovered = true;
        sprite.setTexture(textureActive); // Switch to the Active texture
    }
    else {
        isHovered = false;
        sprite.setTexture(textureIdle);   // Revert to the Idle texture
    }
}

void Button::render(sf::RenderWindow& window) {
    window.draw(sprite);
}

// =========== CLASS BASE TEXT BOX ==================

BaseTextBox::BaseTextBox(float x, float y, const std::string& fontPath, const std::string& backgroundImagePath)
    : padding(5.f), isActive(false) {
    if (!font.loadFromFile(fontPath)) {
        throw std::runtime_error("Failed to load font");
    }

    if (!backgroundTexture.loadFromFile(backgroundImagePath)) {
        throw std::runtime_error("Failed to load background image");
    }

    backgroundSprite.setTexture(backgroundTexture);
    backgroundSprite.setPosition(x, y);

    text.setFont(font);
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Black);
    text.setPosition(x + padding + 10, y + padding + 6);
}

bool BaseTextBox::isClicked(const sf::Vector2i& mousePos) {
    return backgroundSprite.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
}

// =============== CLASS SCROLL BOX ====================

ScrollBox::ScrollBox(float x, float y, const std::string& fontPath, const std::string& backgroundImagePath, unsigned int maxLines)
    : BaseTextBox(x, y, fontPath, backgroundImagePath), maxVisibleLines(maxLines), scrollOffset(0) {}

void ScrollBox::updateLines(const std::vector<std::string>& newLines) {
    lines.clear();

    for (const auto& line : newLines) {
        lines.push_back(line);
    }

    scrollOffset = 0;
}

void ScrollBox::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (isClicked(static_cast<sf::Vector2i>(sf::Mouse::getPosition(window)))) {
            activate();
        }
        else {
            deactivate();
        }
    }

    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
            if (scrollOffset > 0) {
                scrollOffset--;
            }
        }
        else {
            if (scrollOffset + maxVisibleLines < lines.size()) {
                scrollOffset++;
            }
        }
    }
}

void ScrollBox::render(sf::RenderWindow& window) {
    window.draw(backgroundSprite);
    sf::Text lineText = text;
    float yOffset = backgroundSprite.getPosition().y + padding;

    for (unsigned int i = scrollOffset; i < scrollOffset + maxVisibleLines && i < lines.size(); ++i) {
        lineText.setString(lines[i]);
        lineText.setPosition(backgroundSprite.getPosition().x + padding + 10, yOffset + 6);
        window.draw(lineText);
        yOffset += lineText.getCharacterSize() + 10;
    }
}

// ================= CLASS TEXT BOX ====================

TextBox::TextBox(float x, float y, const std::string& fontPath, const std::string& backgroundImagePath)
    : BaseTextBox(x, y, fontPath, backgroundImagePath), cursorVisible(true) {
    cursor.setSize(sf::Vector2f(2.f, text.getCharacterSize()));
    cursor.setFillColor(sf::Color::Black);
    cursor.setPosition(text.getPosition().x, text.getPosition().y);
}

string TextBox::getInput() {
    return inputString;
}

void TextBox::clear() {
    inputString.clear();
}

void TextBox::handleEvent(const sf::Event& event, sf::RenderWindow& window, int maxLength) {
    if (event.type == sf::Event::MouseButtonPressed) {
        if (isClicked(static_cast<sf::Vector2i>(sf::Mouse::getPosition(window))))
            activate();
        else
            deactivate();
    }

    if (isActive) {
        if (event.type == sf::Event::TextEntered) {
            char enteredChar = static_cast<char>(event.text.unicode);

            if (event.text.unicode == '\b' && !inputString.empty()) {
                inputString.pop_back();
            }
            else if (isalnum(enteredChar) || enteredChar == '@' || enteredChar == '.' ||
                enteredChar == '_' || enteredChar == '-') {

                if (inputString.length() < maxLength) {
                    inputString += enteredChar;
                }
            }
        }
    }
    text.setString(inputString);
}

void TextBox::render(sf::RenderWindow& window) {
    if (isActive) {
        if (blinkClock.getElapsedTime().asSeconds() > 0.5f) {
            cursorVisible = !cursorVisible;
            blinkClock.restart();
        }
        cursor.setPosition(text.getPosition().x + text.getGlobalBounds().width, text.getPosition().y + 2);
    }
    window.draw(backgroundSprite);
    window.draw(text);
    if (cursorVisible && isActive) {
        window.draw(cursor);
    }
}

// ================== CLASS STATE =================

void State::setBackground(const std::string& backgroundImagePath) {
    if (backgroundTexture.loadFromFile(backgroundImagePath)) {
        background.setTexture(backgroundTexture);
    }
}

void State::addButton(const std::string& idleImagePath, const std::string& activeImagePath, float x, float y) {
    buttons.emplace_back(idleImagePath, activeImagePath, x, y);
}

void State::render(sf::RenderWindow& window) {
    window.draw(background);
    for (auto& button : buttons) {
        button.render(window);
    }
}

// ================== CLASS MENU ==================

Menu::Menu() {
    if (!backgroundTexture.loadFromFile("graphic/background.png")) {
        throw std::runtime_error("Failed to load background texture");
    }
    background.setTexture(backgroundTexture);
    background.setPosition(0, 0);

    addButton("graphic/run_button.png", "graphic/run_button_hover.png", 288.f, 248.f);
    addButton("graphic/stop_button.png", "graphic/stop_button_hover.png", 288.f, 248.f);
    addButton("graphic/exit_button.png", "graphic/exit_button_hover.png", 288.f, 340.f);
    addButton("graphic/about_us_button.png", "graphic/about_us_button_hover.png", 288.f, 432.f);
    addButton("graphic/instruc_button.png", "graphic/instruc_button_hover.png", 288.f, 525.f);
    addButton("graphic/config_button.png", "graphic/config_button_hover.png", 288.f, 617.f);
}

void Menu::handleInput(sf::Event& event, sf::RenderWindow& window) {
    for (size_t i = 0; i < buttons.size(); ++i) {
        buttons[i].update(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            if (buttons[i].isClicked(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))) {
                StateManager* stateManager = StateManager::getInstance();
                string pathImg;
                switch (i) {
                case 0:
                    if (run) {
                        handleStopButton(run, buttons[0], window);
                    }
                    else {
                        handleRunButton(run, buttons[1], window);
                    }
                    break;
                case 2:
                    handleExitButton(window);
                    break;
                case 3:
                    pathImg = "graphic/about_us_image.png";
                    stateManager->pushState(new Notification(pathImg));
                    break;
                case 4:
                    pathImg = "graphic/instruct_image.png";
                    stateManager->pushState(new Notification(pathImg));
                    break;
                case 5:
                    stateManager->pushState(new Config());
                default:
                    break;
                }
            }
        }
    }
}

void Menu::render(sf::RenderWindow& window) {
    window.draw(background);
    if (run) {
        buttons[1].render(window);
    }
    else {
        buttons[0].render(window);
    }
    for (int i = 2; i < buttons.size(); i++) {
        buttons[i].render(window);
    }
}

// =============== CLASS CONFIG ==================

Config::Config() : findBox(215.f, 221.f, "graphic/arial.ttf", "graphic/findBox.png"),
mailBox(215.f, 314.f, "graphic/arial.ttf", "graphic/emailbox.png", 9) {
    if (!backgroundTexture.loadFromFile("graphic/config_image.png")) {
        throw std::runtime_error("Failed to load background texture");
    }
    background.setTexture(backgroundTexture);
    background.setPosition(0, 0);

    emailList = getAdminList();

    addButton("graphic/plus_button.png", "graphic/plus_button_hover.png", 580.f, 221.f);
    addButton("graphic/minus_button.png", "graphic/minus_button_hover.png", 637.f, 221.f);
    addButton("graphic/arow_back.png", "graphic/arow_back_hover.png", 35.f, 35.f);
}

void Config::handleInput(sf::Event& event, sf::RenderWindow& window) {

    string email;

    findBox.handleEvent(event, window, 27);
    mailBox.handleEvent(event, window);

    for (size_t i = 0; i < buttons.size(); ++i) {
        buttons[i].update(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
        if (event.type == sf::Event::MouseButtonPressed) {
            if (buttons[i].isClicked(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))) {
                StateManager* stateManager = StateManager::getInstance();
                email = findBox.getInput();
                switch (i) {
                case 0:
                    if (isValidEmailAddress(email, emailList))
                        emailList.push_back(email);
                    findBox.clear();
                    break;
                case 1:
                    removeMail(email, emailList);
                    findBox.clear();
                    break;
                case 2:
                    updateAdmin(emailList);
                    stateManager->popState();
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void Config::render(sf::RenderWindow& window) {
    window.draw(background);
    for (int i = 0; i < buttons.size(); i++) {
        buttons[i].render(window);
    }
    mailBox.lines = emailList;
    findBox.render(window);
    mailBox.render(window);
}

// ============== CLASS NOTIFICATION ==================

Notification::Notification(string pathImg) {
    if (!backgroundTexture.loadFromFile(pathImg)) {
        throw std::runtime_error("Failed to load background texture");
    }
    background.setTexture(backgroundTexture);
    background.setPosition(0, 0);

    addButton("graphic/arow_back.png", "graphic/arow_back_hover.png", 35.f, 35.f);
}

void Notification::handleInput(sf::Event& event, sf::RenderWindow& window) {
    buttons[0].update(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        if (buttons[0].isClicked(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)))) {
            StateManager* stateManager = StateManager::getInstance();
            stateManager->popState();
        }
    }
}

// ===============================================

void runApp() {
    sf::RenderWindow window(sf::VideoMode(800, 800), "Remote PC Control via Email");

    StateManager* stateManager = StateManager::getInstance();

    stateManager->pushState(new Menu());

    sf::Event event;
    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            stateManager->handleInput(event, window);
        }

        window.clear();
        stateManager->render(window);
        window.display();
    }
}