#include "Pause.h"

int Pause::getSelectedButton() {
	return selectedButton;
}
void Pause::initPause(GameWindow* gwindow) {
	boxTexture.loadFromFile("Textures\\GUI\\pauseBox.jpg");
	buttonBackgroundTexture.loadFromFile("Textures\\GUI\\pauseButtonBackground.jpg");
	backgroundBlurTexture.loadFromFile("Textures\\GUI\\gameBackgroundBlur.jpg");
	pauseBackgroundBlur.setTexture(&backgroundBlurTexture);
	pauseBackgroundBlur.setSize(Vector2f(gwindow->x, gwindow->y));
	pauseBackgroundBlur.setPosition(0, 0);
	pauseBackgroundBlur.setFillColor(Color(68, 68, 68, 230));
	pauseBox.setTexture(&boxTexture);
	pauseBox.setSize(Vector2f(gwindow->x / 3, gwindow->y / 3));
	pauseBox.setPosition(static_cast<float>(gwindow->x) / 3, static_cast<float>(gwindow->y) / 3);
	selectedButton = 0;
	pauseFont.loadFromFile("Fonts\\Hacked Cyr.ttf");
	caption.setFont(pauseFont);
	caption.setString("PAUSE");
	caption.setFillColor(Color(128, 128, 0));
	caption.setCharacterSize(70);
	caption.setPosition(Vector2f(pauseBox.getPosition().x + pauseBox.getSize().x / 2 - caption.getGlobalBounds().width / 2, pauseBox.getPosition().y));
}
void Pause::addPauseButton(string title, ButtonAction action, GameWindow* gwindow) {
	Button button;
	button.title.setFont(pauseFont);
	button.title.setString(title);
	button.title.setFillColor(Color(255, 228, 225));
	button.title.setCharacterSize(30);
	button.title.setPosition(Vector2f(pauseBox.getPosition().x + pauseBox.getSize().x / 2 - button.title.getGlobalBounds().width / 2, caption.getPosition().y + caption.getGlobalBounds().height * 2 + buttons.size() * button.title.getGlobalBounds().height * 2));
	button.buttonBackground.setSize(Vector2f(button.title.getGlobalBounds().width, button.title.getGlobalBounds().height));
	button.buttonBackground.setPosition(Vector2f(button.title.getGlobalBounds().left, button.title.getGlobalBounds().top));
	button.buttonBackground.setTexture(&buttonBackgroundTexture);
	button.action = action;
	buttons.push_back(button);
}
void Pause::controlPause() {
	if (Keyboard::isKeyPressed(Keyboard::Key::W) && delayBetweenPausePresses.getElapsedTime().asMilliseconds() > 200) {
		selectedButton--;
		if (selectedButton < 0) {
			selectedButton = buttons.size() - 1;
		}
		delayBetweenPausePresses.restart();
	}
	if (Keyboard::isKeyPressed(Keyboard::Key::S) && delayBetweenPausePresses.getElapsedTime().asMilliseconds() > 200) {
		selectedButton++;
		if (selectedButton > buttons.size() - 1) {
			selectedButton = 0;
		}
		delayBetweenPausePresses.restart();
	}
}
void Pause::selectPauseButton(Game* gameClass) {
	if (Keyboard::isKeyPressed(Keyboard::Key::Enter) && delayBetweenPausePresses.getElapsedTime().asMilliseconds() > 200) {
		(gameClass->*buttons[selectedButton].action)();
		delayBetweenPausePresses.restart();
	}
}