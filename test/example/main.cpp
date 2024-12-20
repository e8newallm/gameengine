#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_thread.h>

#include <iostream>
#include <ctime>

#include "object.h"
#include "physicsobject.h"
#include "image.h"
#include "context.h"

#include "texture.h"
#include "spritemap.h"

#include "mousestate.h"
#include "keystate.h"

#include "player.h"

bool close = false;

int main()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("GAME", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       1000, 1000, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

    View viewport( {1000, 1000}, {0, 0});
    viewport.setZoom(1.0);

    Context state(rend, &viewport);

    PhysicsContext* phyContext = state.getPhysicsContext();
    state.addImage(new Image({0, 0, 1000, 1000}, new Texture("tex/background.png"), UINT8_MAX));
    phyContext->addPhyObj(new PhysicsObject({0, 960, 1000, 40}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("tex/Tile.png")));
    phyContext->addPhyObj(new PhysicsObject({0, 900, 500, 60}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("tex/Tile.png")));
    phyContext->addPhyObj(new PhysicsObject({700, 900, 500, 60}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("tex/Tile.png")));
    phyContext->addPhyObj(new PhysicsObject({700, 600, 200, 60}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("tex/Tile.png")));
    phyContext->addPhyObj(new PhysicsObject({0, 0, 50, 900}, PHYOBJ_STATIC | PHYOBJ_COLLIDE, new Texture("tex/Tile.png")));
    phyContext->addPhyObj(new Player({500, 920, 40, 40}, PHYOBJ_COLLIDE, new SpriteMap(rend, "tex/spritemap.json")));
    state.startPhysics();

    while (!close)
    {
        MouseState::reset();

        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    close = 1;
                    break;
                }
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    KeyState::updateKey(event.key.keysym.scancode, event.type);
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    MouseState::updateButton(event.button);
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    MouseState::updateMove(event.motion);
                    break;
                }
                case SDL_MOUSEWHEEL:
                {
                    MouseState::updateWheel(event.wheel);
                    break;
                }
                default:
                {
                       
                }
            }
        }

        if(MouseState::scrollDelta() != 0)
        {
            viewport.setZoom(std::max(std::min(viewport.getZoom() + 0.05 * (float)MouseState::scrollDelta(), 2.0), 0.1));
        }
        if(MouseState::buttonDown(SDL_BUTTON_RIGHT))
        {
            SDL_Point newPosition = viewport.getPosition();
            SDL_Point delta = MouseState::mouseDelta();
            newPosition.y -= delta.y / viewport.getZoom();
            newPosition.x += delta.x / viewport.getZoom();
            viewport.setPosition(newPosition);
        }
        
        SDL_RenderClear(rend);
        state.draw();
        SDL_RenderPresent(rend);
        //SDL_Delay(10);
    }
    state.stopPhysics();
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
 
    return 0;
}