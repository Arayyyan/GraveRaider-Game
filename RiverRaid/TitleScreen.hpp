#pragma once
#include "GamesEngineeringBase.h"
#include "renderer.hpp"
#include "vec2f.hpp"

enum class GameState {
	TitleScreen,
	Playing
};
class TitleScreen {
public:
	TitleScreen() {
		//when I want to add a bg image
		//titleImage.load("./Resources/title_screen.png");

	}

   
    void DrawChar(GamesEngineeringBase::Window& window, char c, int startX, int startY, int scale, int r, int g, int b) {
        const char* pattern = "";
        //used AI to generate this switch case for the custom title
        switch (c) {
        case 'G': pattern = "01110|10000|10111|10001|01110"; break;
        case 'R': pattern = "11110|10001|11110|10100|10010"; break;
        case 'A': pattern = "01110|10001|11111|10001|10001"; break;
        case 'V': pattern = "10001|10001|10001|01010|00100"; break;
        case 'E': pattern = "11111|10000|11110|10000|11111"; break;
        case 'I': pattern = "11100|01000|01000|01000|11100"; break;
        case 'D': pattern = "11110|10001|10001|10001|11110"; break;
        case 'P': pattern = "11110|10001|11110|10000|10000"; break;
        case 'L': pattern = "10000|10000|10000|10000|11111"; break;
        case 'Y': pattern = "10001|10001|01010|00100|00100"; break;
        default: return;
        }

        int row = 0;
        std::string line;
        for (int i = 0; i <= (int)strlen(pattern); ++i) {
            if (pattern[i] == '|' || pattern[i] == '\0') {
                for (int col = 0; col < (int)line.length(); ++col) {
                    if (line[col] == '1') {
                        for (int sx = 0; sx < scale; ++sx) {
                            for (int sy = 0; sy < scale; ++sy) {
                                window.draw(startX + (col * scale) + sx, startY + (row * scale) + sy, r, g, b);
                            }
                        }
                    }
                }
                line = "";
                row++;
            }
            else {
                line += pattern[i];
            }
        }
    }

    void DrawString(GamesEngineeringBase::Window& window, std::string text, int startX, int startY, int scale, int r, int g, int b) {
        int cursorX = startX;
        for (char c : text) {
            if (c == ' ') {
                cursorX += 6 * scale;
            }
            else {
                DrawChar(window, c, cursorX, startY, scale, r, g, b);

                //The letter I needs narrower spacing so this solves that
                if (c == 'I') {
                    cursorX += 4 * scale;
                }
                else {
                    cursorX += 6 * scale;
                }
            }
        }
    }

    void Draw(GamesEngineeringBase::Window& window) {
        unsigned int winW = window.getWidth();
        unsigned int winH = window.getHeight();

        for (unsigned int y = 0; y < winH; ++y) {
            for (unsigned int x = 0; x < winW; ++x) {
                window.draw(x, y, 15, 15, 25);
            }
        }

        int titleX = static_cast<int>(winW / 2 - 220);
        int titleY = static_cast<int>(winH / 2 - 100);
        int textWidth = 12 * 6 * 4;
        int centeredTitleX = titleX + (440 - textWidth) / 2;
        DrawString(window, "GRAVE RAIDER", centeredTitleX, titleY, 4, 255, 255, 255);

        
        buttonX = static_cast<int>(winW / 2 - 100);
        buttonY = static_cast<int>(winH / 2 + 20);
        buttonW = 200;
        buttonH = 50;
        for (int y = 0; y < buttonH; ++y) {
            for (int x = 0; x < buttonW; ++x) {
                int px = buttonX + x;
                int py = buttonY + y;
                if (px >= 0 && px < static_cast<int>(winW) && py >= 0 && py < static_cast<int>(winH)) {
                    window.draw(px, py, 50, 110, 160);  
                }
            }
        }

     
        DrawString(window, "PLAY", buttonX + 68, buttonY + 13, 3, 255, 255, 255);
    }

    bool isPlayButtonClicked(const GamesEngineeringBase::Window& window) const {
        if (window.mouseButtonPressed(GamesEngineeringBase::MouseLeft)) {
            int mx = window.getMouseInWindowX();
            int my = window.getMouseInWindowY();
            if (mx >= buttonX && mx <= buttonX + buttonW && my >= buttonY && my <= buttonY + buttonH) {
                return true;
            }
        }
        return false;
    }

private:
    int buttonX = 0;
    int buttonY = 0;
    int buttonW = 0;
    int buttonH = 0;
};