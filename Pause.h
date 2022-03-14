#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Button.h"
#include "GameWindow.h"

using namespace sf;
using namespace std;

class Pause {
private:
	int selectedButton;
	Font pauseFont;
	Clock delayBetweenPausePresses;

	Texture boxTexture;
	Texture buttonBackgroundTexture;
	Texture backgroundBlurTexture;
public:
	Text caption;
	RectangleShape pauseBackgroundBlur;
	RectangleShape pauseBox;
	vector <Button> buttons;

	int getSelectedButton();
	void initPause(GameWindow* gwindow);
	void addPauseButton(string title, ButtonAction action, GameWindow* gwindow);
	void controlPause();
	void selectPauseButton(Game* gameClass);
};