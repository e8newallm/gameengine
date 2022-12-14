#ifndef CONTEXT_H
#define CONTEXT_H

#include <iostream>

#include "object/physicsobject.h"
#include "object/object.h"
#include "object/image.h"
#include "keystate.h"
#include "view.h"

class Context
{
    public:
        Context(SDL_Renderer* rend, View* viewport, SDL_ThreadFunction phyFunction = nullptr);
        void draw();
        
        PhysicsContext* getPhysicsContext() { return phyContext; };
        void startPhysics();
        void stepPhysics();
        void stopPhysics();
        bool physicsRunning() { return phyRunning; };

        void addImage(Image* newImage);
        
    private:
        bool phyRunning;
        SDL_ThreadFunction phyFunction;
        SDL_Thread* physThread;

        View* viewport;
        SDL_Renderer* rend;
        KeyState& keyState;
        PhysicsContext* phyContext;

        std::vector<Image*> backgroundImages, foregroundImages;
};

#endif