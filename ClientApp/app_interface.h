#ifndef _APP_INTERFACE_H_
#define _APP_INTERFACE_H_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "mail_service.h"

bool isMouseOver(const sf::RectangleShape& sprite, const sf::RenderWindow& window);

void runApp();

#endif // !_APP_INTERFACE_H_
