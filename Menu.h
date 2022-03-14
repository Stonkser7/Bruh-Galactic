#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Button.h"
#include "GameWindow.h"

using namespace sf;
using namespace std;


class Menu {
private:
	int selectedButton;
	bool isMenuTouched;
	Font menuFont;

	Texture menuBackgroundTexture;
	Texture buttonBackgroundTexture;
public:
	Clock delayBetweenMenuPresses;
	Text caption;
	RectangleShape menuBackground;
	vector <Button> buttons;

	int getSelectedButton();
	void initMenu(GameWindow* gwindow);
	void addMenuButton(string title, ButtonAction action);
	void controlMenu();
	void selectMenuButton(Game* gameClass);
};