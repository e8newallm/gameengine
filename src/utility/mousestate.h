#ifndef MOUSESTATE_H
#define MOUSESTATE_H

#include <SDL3/SDL.h>

#include <array>

namespace GameEng
{
    class MouseState
    {
        public:
            static void update();
            static void updateButton(SDL_MouseButtonEvent buttonPress);
            static void updateMove(SDL_MouseMotionEvent mouseMove);
            static void updateWheel(SDL_MouseWheelEvent wheelScroll);

            static void reset();

            static inline SDL_FPoint mousePos()
                { return {xMouse, yMouse}; };

            static inline SDL_FPoint mouseDelta()
                { return {xDeltaMouse, yDeltaMouse}; };

            static inline Sint32 scrollDelta()
                { return scrollAmount; };

            static inline bool buttonDown(Uint8 button)
                { return mouseButtonDown[button]; };

            static inline bool doubleClicked(Uint8 button)
                { return mouseButton[button].clicks == 2; };

            static inline bool clicked(Uint8 button)
                { return mouseButton[button].clicks == 1; };

            static inline SDL_Point clickPosition(Uint8 button)
                { return {(int)mouseButton[button].x, (int)mouseButton[button].y}; };

        private:
            static inline float xMouse, yMouse;
            static inline float xDeltaMouse, yDeltaMouse;
            static inline Sint32 scrollAmount;
            static inline std::array<bool, SDL_BUTTON_X2+1> mouseButtonDown;
            static inline std::array<SDL_MouseButtonEvent, SDL_BUTTON_X2+1> mouseButton;

            MouseState() = delete;
            MouseState(const MouseState&) = delete;
            MouseState& operator=(const MouseState&) = delete;
    };
}

#endif