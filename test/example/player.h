#ifndef PLAYER_H
#define PLAYER_H

#include "physicsobject.h"

class Player : public PhysicsObject
{
    public:
        Player(double x, double y, double height, double width, int flags, SDL_Texture* texture);
        Player() : PhysicsObject() {};

        virtual void update(double deltaTime, PhysicsContext* context) override;

    private:
        const float speed = 0.2f;
        const float jump = 0.2f;
        double MAXHEIGHT = 99999.0f;
};

#endif