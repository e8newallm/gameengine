#ifndef WORLD_H
#define WORLD_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <vector>
#include <array>
#include <mutex>

#include "timer.h"
#include "view.h"

class Image;
class PhysicsObject;

class World
{
    public:
        World(SDL_Renderer* rend, View viewport);
        ~World();
        void draw(double deltaTime);
        void update();
        void addImage(Image* newImage);
        void addPhyObj(PhysicsObject* obj);

        //Physics
        void startPhysics();
        void stopPhysics();
        void runPhysics();
        
        bool physicsRunning() { return phyRunning; };

        double getGravity() { return gravity; };
        void setGravity(double newGravity) { gravity = newGravity; };

        const std::vector<PhysicsObject*>& getphyObjects() { return phyObjects; };

        View& getView();
        SDL_Renderer* getRend();
        
    private:
        View viewport;
        SDL_Renderer* rend;

        std::array<std::vector<Image*>, UINT8_MAX+1> images;

        //Physics
        bool phyRunning;

        Timer<> lastPhysics;
        std::mutex usageLock;
        double gravity = 0.00005f;
        const double pps = 60.0f;
        std::vector<PhysicsObject*> phyObjects;
};

#endif