#ifndef _APP_INTERFACE_H_
#define _APP_INTERFACE_H_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "mail_service.h"

bool isMouseOver(const sf::RectangleShape& object, const sf::RenderWindow& window);

void updateButtonTexture(vector<sf::RectangleShape*> Button, vector<sf::Texture>& texHover, vector<sf::Texture>& texNormal, sf::RenderWindow& window);

void draw(vector<sf::RectangleShape*> button, sf::RenderWindow& window);

void setButtonPosition(vector<sf::RectangleShape*> button);

void setButtonSize(vector<sf::RectangleShape*> button);

bool loadButtonImages(vector<sf::Texture>& texNormal, vector<sf::Texture>& texHover, vector<string> buttonName);

void handleRunButton(sf::RectangleShape& runButton);

void handleStopButton(sf::RectangleShape& stopButton, sf::RenderWindow& window);

void handleExitButton(sf::RenderWindow& window);

void handleAboutUsButton(sf::RenderWindow& window);

void createText(sf::Text& inputText, const sf::Font& font, int sz, float x, float y);

bool isValidEmail(const std::string& email);

void handleEmailScrolling(sf::Event event, sf::RectangleShape& emailBox, int& scrollOffset, const std::vector<std::string>& emails, size_t maxEmailsToDisplay);

void handleTextInput(sf::Event event, std::string& userInput, sf::Text& inputText, std::vector<std::string>& emails, bool& isTyping);

void handleMouseClickEvents(sf::Event event, bool& isTyping, string& userInput, sf::Text& inputText,
    vector<std::string>& emails, sf::RectangleShape& textBox, sf::RectangleShape& plusButton, sf::RectangleShape& minusButton);

void updateCursor(bool isTyping, sf::Clock& clock, bool& cursorVisible, sf::RectangleShape& cursor, const sf::Text& inputText);

void renderEmailList(sf::RenderWindow& window, const sf::Font& font, const std::vector<std::string>& emails, int maxEmailsToDisplay, int scrollOffset);

vector<string> loadAdmin();

bool updateAdmin(vector<string> gmails);

void handleConfigButton(sf::RenderWindow& window);

void handle(sf::Event event, sf::RenderWindow& window, vector<sf::RectangleShape*> button);

void runApp();

#endif // !_APP_INTERFACE_H_
