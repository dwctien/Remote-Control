#include "app_interface.h"
#include "globals.h"

std::thread threadCheckMail;

bool isMouseOver(const sf::RectangleShape& sprite, const sf::RenderWindow& window) {
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return sprite.getGlobalBounds().contains(mousePos);
}

void updateButtonTexture(vector<sf::RectangleShape*> Button,vector<sf::Texture> &texHover,vector<sf::Texture> &texNormal,sf::RenderWindow& window)
{
    for(int i = 0; i < Button.size(); i++)
        if (isMouseOver(*Button[i], window)) {
            Button[i]->setTexture(&texHover[i]);
        }
        else {
            Button[i]->setTexture(&texNormal[i]);
        }
}

void draw(vector<sf::RectangleShape*> button, sf::RenderWindow& window)
{
    if(isRunning)
        window.draw(*button[button.size() - 1]);
    else
        window.draw(*button[0]);

    for(int i = 1; i < button.size() - 1; i++)
        window.draw(*button[i]);
}

void setButtonPosition(vector<sf::RectangleShape*> button)
{
    float x = 275, y = 250;
    button[button.size() - 1]->setPosition(x, y);
    for (int i = 0; i < button.size() - 1; i++)
        button[i]->setPosition(x, y + i * 100);
}

void setButtonSize(vector<sf::RectangleShape*> button)
{
    sf::Vector2f size(250.f, 70.f);
    for (int i = 0; i < button.size(); i++)
        button[i]->setSize(size);
}

bool loadButtonImages(vector<sf::Texture>& texNormal, vector<sf::Texture>& texHover, vector<string> buttonName)
{
    for (int i = 0; i < texNormal.size(); i++)
        if (!texNormal[i].loadFromFile("graphic/" + buttonName[i] + "_button.png") ||
            !texHover[i].loadFromFile("graphic/" + buttonName[i] + "_button_hover.png"))
        {
            std::cerr << "Failed to load button images!" << std::endl;
            return false;
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
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (backButton.getGlobalBounds().contains(mousePos))
                {
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

void handleConfigButton(sf::RenderWindow& window)
{

}

vector<string> handleGetAdmin(string server_ip)
{
    vector<string> admin;
    string response_subject, response_body, fileName = "";
    vector<uint8_t> response_data;
    runClient("", server_ip, "get_Admin", response_subject, response_body, fileName, response_data);
    saveVectorToFile(fileName, response_data);
    ifstream file("admin.txt");
    if (file.is_open())
    {
        string line;
        while (getline(file, line))
        {
            if (!line.empty())
                admin.push_back(line);
        }
        file.close();
    }
    return admin;
}

string handleAddAdmin(string admin, string server_ip)
{
    string response_subject, response_body, fileName = "";
    vector<uint8_t> response_data;
    runClient(admin, server_ip, "add_Admin",response_subject, response_body, fileName, response_data);
    return response_subject;
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
        else if (button[3]->getGlobalBounds().contains(mousePos))
        {
            handleAddAdmin("blabla@gmail.com", "127.0.0.1");
        }
        else if (button[4]->getGlobalBounds().contains(mousePos))
        {
            handleGetAdmin("127.0.0.1");
        }
        else if (button[button.size() - 1]->getGlobalBounds().contains(mousePos) && isRunning) {
            handleStopButton(*button[button.size() - 1], window);
        }
    }
    else if (event.type == sf::Event::Closed) {
        handleExitButton(window);
    }
}

void runApp()
{
    sf::RenderWindow window(sf::VideoMode(800, 800), "REMOTE CONTROL WITH GMAIL", sf::Style::Close | sf::Style::Titlebar);
    sf::RectangleShape runButton, exitButton, aboutUsButton, instrucButton, configButton, stopButton;
    sf::Texture backgroundTexture;
    sf::Texture runTextureNormal, runTextureHover;
    sf::Texture stopTextureNormal, stopTextureHover;
    sf::Texture aboutUsTextureNormal, aboutUsTextureHover;
    sf::Texture exitTextureNormal, exitTextureHover;
    sf::Texture instrucTextureNormal, instrucTextureHover;
    sf::Texture configTextureNormal, configTextureHover;
    
    vector<sf::RectangleShape*> button = { &runButton, &exitButton, &aboutUsButton, &instrucButton, &configButton , &stopButton};
    vector<sf::Texture> texNormal = { runTextureNormal , exitTextureNormal, aboutUsTextureNormal, instrucTextureNormal, configTextureNormal, stopTextureNormal };
    vector<sf::Texture> texHover = { runTextureHover , exitTextureHover, aboutUsTextureHover, instrucTextureHover, configTextureHover, stopTextureHover};
    vector<std::string> buttonName = { "run", "exit", "about_us", "instruc", "config" , "stop"};

    if (!backgroundTexture.loadFromFile("graphic/background.png"))
        return;

    sf::Sprite background(backgroundTexture);

    if (!loadButtonImages(texNormal, texHover, buttonName))
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
