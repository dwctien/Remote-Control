#ifndef _APPUI_H_
#define _APPUI_H_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "mail_service.h"
#include "stack"
#include <mutex>

extern std::mutex mtx;

// ================== CLASS BUTTON ==================

class Button {
public:
    bool isHovered;
    sf::Texture textureIdle;
    sf::Texture textureActive;
    sf::Sprite sprite;

    Button(const std::string& idleImagePath, const std::string& activeImagePath, float x, float y);

    bool isClicked(const sf::Vector2f& mousePos) const;

    void update(const sf::Vector2f& mousePos);

    void render(sf::RenderWindow& window);
};

// =========== CLASS BASE TEXT BOX ==================

class BaseTextBox {
protected:
    sf::Sprite backgroundSprite;
    sf::Texture backgroundTexture;
    sf::Text text;
    sf::Font font;
    float padding;
    bool isActive;

public:
    BaseTextBox(float x, float y, const std::string& fontPath, const std::string& backgroundImagePath);

    virtual void render(sf::RenderWindow& window) = 0;
    bool isClicked(const sf::Vector2i& mousePos);
    void activate() { isActive = true; }
    void deactivate() { isActive = false; }
};

// ================== SCROLL BOX ====================

class ScrollBox : public BaseTextBox {
public:
    unsigned int maxVisibleLines;
    unsigned int scrollOffset;
    std::vector<std::string> lines;
    ScrollBox(float x, float y, const std::string& fontPath, const std::string& backgroundImagePath, unsigned int maxLines);

    void updateLines(const std::vector<std::string>& newLines);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void render(sf::RenderWindow& window) override;
};

// ================== TEXT BOX =====================

class TextBox : public BaseTextBox {
private:
    std::string inputString;
    sf::RectangleShape cursor;
    sf::Clock blinkClock;
    bool cursorVisible;

public:
    TextBox(float x, float y, const std::string& fontPath, const std::string& backgroundImagePath);

    string getInput();
    void clear();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, int maxLength);
    void render(sf::RenderWindow& window) override;
};

// ================== CLASS STATE ==================

class State {
protected:
    sf::Sprite background;
    sf::Texture backgroundTexture;
    std::vector<Button> buttons;
public:
    virtual ~State() = default;

    virtual void handleInput(sf::Event& event, sf::RenderWindow& window) = 0;
    virtual void render(sf::RenderWindow& window);
    void setBackground(const std::string& backgroundImagePath);
    void addButton(const std::string& idleImagePath, const std::string& activeImagePath, float x, float y);
};

// ============ CLASS STATE MANAGER ================

class StateManager {
private:
    std::stack<State*> states;

public:
    static StateManager* getInstance() {
        static StateManager instance;
        return &instance;
    }

    ~StateManager() {
        while (!states.empty()) {
            delete states.top();
            states.pop();
        }
    }

    void pushState(State* state) {
        states.push(state);
    }

    void popState() {
        if (!states.empty()) {
            delete states.top();
            states.pop();
        }
    }

    State* getCurrentState() {
        if (!states.empty()) return states.top();
        return nullptr;
    }

    void handleInput(sf::Event& event, sf::RenderWindow& window) {
        if (!states.empty()) {
            states.top()->handleInput(event, window);
        }
    }

    void render(sf::RenderWindow& window) {
        if (!states.empty()) {
            states.top()->render(window);
        }
    }
};

// ================== CLASS MENU ==================

class Menu : public State {
    bool run = false;
public:
    Menu();

    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window);
};

// =============== CLASS CONFIG ==================

class Config : public State {
    vector<string> emailList;
    TextBox findBox;
    ScrollBox mailBox;
public:
    Config();

    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
    void render(sf::RenderWindow& window);
};

// ================ CLASS ABOUT ==================

class Notification : public State {
public:
    Notification(string pathImg);

    void handleInput(sf::Event& event, sf::RenderWindow& window) override;
};

void runApp();

#endif // !_APPUI_H_