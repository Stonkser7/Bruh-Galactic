#include "Menu.h"

int Menu::getSelectedButton() {
	return selectedButton;
}
void Menu::initMenu(GameWindow* gwindow) {
	menuBackgroundTexture.loadFromFile("Textures\\menuBackgroundTexture.jpg");
	buttonBackgroundTexture.loadFromFile("Textures\\menuButtonBackgroundTexture.png");
	menuBackground.setTexture(&menuBackgroundTexture);
	menuBackground.setSize(Vector2f(gwindow->x, gwindow->y));
	menuBackground.setPosition(0, 0);
	selectedButton = 0;
	menuFont.loadFromFile("Fonts\\Hacked Cyr.ttf");
	caption.setFont(menuFont);
	caption.setString("BRUH GALACTIC");
	caption.setFillColor(Color(25, 25, 112));
	caption.setPosition(Vector2f(gwindow->x / 2.6, gwindow->y / 2.5));
	caption.setCharacterSize(50);
	isMenuTouched = false;
}
void Menu::addMenuButton(string title, ButtonAction action) {
	Button button;
	button.title.setFont(menuFont);
	button.title.setString(title);
	button.title.setFillColor(Color(139, 0, 0));
	button.title.setCharacterSize(30);
	button.title.setPosition(Vector2f(caption.findCharacterPos(2 + buttons.size()).x, caption.getPosition().y + caption.getGlobalBounds().height * 2 + buttons.size() * button.title.getGlobalBounds().height * 2));
	button.buttonBackground.setSize(Vector2f(button.title.getGlobalBounds().width, button.title.getGlobalBounds().height));
	button.buttonBackground.setPosition(Vector2f(button.title.getGlobalBounds().left, button.title.getGlobalBounds().top));
	button.buttonBackground.setTexture(&buttonBackgroundTexture);
	button.action = action;
	buttons.push_back(button);
}
void Menu::controlMenu() {
	if (Keyboard::isKeyPressed(Keyboard::Key::W) && delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
		/*if (!menu.isMenuTouched) {
			menu.isMenuTouched = !menu.isMenuTouched;
		}*/
		selectedButton--;
		if (selectedButton < 0) {
			selectedButton = buttons.size() - 1;
		}
		delayBetweenMenuPresses.restart();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::S) && delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
		/*if (!menu.isMenuTouched) {
			menu.isMenuTouched = !menu.isMenuTouched;
		}*/
		selectedButton++;
		if (selectedButton > buttons.size() - 1) {
			selectedButton = 0;
		}
		delayBetweenMenuPresses.restart();
	}
}
void Menu::selectMenuButton(Game* gameClass) {
	if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && delayBetweenMenuPresses.getElapsedTime().asMilliseconds() > 200) {
		(gameClass->*buttons[selectedButton].action)();
		delayBetweenMenuPresses.restart();
	}
}