#ifndef PLAYER_H
#define PLAYER_H

#include "physicsobject.h"
#include "spritemap.h"

class Player : public PhysicsObject
{
    public:
        Player(SDL_Rect body, int flags, Texture_base* texture) :
        PhysicsObject(body, flags, texture)
        {((SpriteMap*)texture)->setSprite("sprite01");};
        
        using PhysicsObject::draw;
        void draw(SDL_Renderer* rend, double percent, View viewport);
        virtual void update(double deltaTime, PhysicsContext* context) override;

    private:
        const float speed = 0.2f;
        const float jump = 0.2f;
        double MAXHEIGHT = 99999.0f;
};

#endif