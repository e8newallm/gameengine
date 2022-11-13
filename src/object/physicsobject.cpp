#include <iostream>
#include <cmath>

#include "physicsobject.h"
#include "keystate.h"

std::vector<PhysicsObject*> PhysicsObject::collisionObjects, PhysicsObject::noncollisionObjects;

PhysicsObject::PhysicsObject(double x, double y, double height, double width, int flags, SDL_Texture* texture) :
    Object(x, y, height, width, texture)
   ,isStatic(flags & PHYOBJ_STATIC)
   ,canCollide(flags & PHYOBJ_COLLIDE)
{
    if(canCollide)
    {
        collisionObjects.push_back(this);
    }
    else
    {
        noncollisionObjects.push_back(this);
    }
}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::groundCheck()
{
    SDL_Rect groundCheck;
    groundCheck.x = body.x;
    groundCheck.y = body.y + body.h;
    groundCheck.w = body.w;
    groundCheck.h = 2;
    for(int i = 0; i < collisionObjects.size(); i++)
    {
        if(SDL_HasIntersection(&groundCheck, collisionObjects[i]->getBody()))
        {
            onGround = true;
            return;
        }
    }
    onGround = false;
    return;
}

bool PhysicsObject::isOnGround()
{
    return onGround;
}

void PhysicsObject::update(double deltaTime)
{    
    if(isStatic)
        return;

    if(!isOnGround())
        velocityDelta(0, 0.1*deltaTime);

    std::cout << "currentVelocity.x: " << currentVelocity.x << " currentVelocity.y" << currentVelocity.y << "\r\n";
    float magnitude = sqrt(currentVelocity.x * currentVelocity.x + currentVelocity.y * currentVelocity.y);
    if(magnitude > 5.0f)
    {
        while(magnitude > 0.0f)
        {
            std::cout << "MAG: " << magnitude << "\r\n";
            SDL_FPoint tempVelocity;
            tempVelocity = currentVelocity;
            tempVelocity.x /= 5.0f;
            tempVelocity.y /= 5.0f;
            moveDelta(tempVelocity.x, tempVelocity.y);
            body.x = updateBody.x;
            body.y = updateBody.y;
            body.h = updateBody.h;
            body.w = updateBody.w;
            if(detectCollision())
                return;
            magnitude -= 5.0f;
        }
    }
    else
    {
        moveDelta(currentVelocity.x, currentVelocity.y);
        body.x = updateBody.x;
        body.y = updateBody.y;
        body.h = updateBody.h;
        body.w = updateBody.w;
        detectCollision();
    }
}

bool PhysicsObject::detectCollision()
{
    for(int i = 0; i < collisionObjects.size(); i++)
    {
        if(SDL_HasIntersection(getBody(), collisionObjects[i]->getBody()) && this != collisionObjects[i])
        {
            std::cout << "COLLISION!\r\n";
            collision(collisionObjects[i]->getBody());
            return true;
        }
    }
    return false;
}

void PhysicsObject::collision(SDL_Rect* other)
{
    SDL_Rect collisionArea;
    if(SDL_IntersectRect(getBody(), other, &collisionArea))
    {
        if(collisionArea.h > collisionArea.w)
        {
            body.x -= collisionArea.w;
            currentVelocity.x = 0;
        }
        else
        {
            std::cout << "collisionArea.h: " << collisionArea.h << "\r\n";
            body.y -= collisionArea.h;
            currentVelocity.y = 0;
        }
    }
}

void PhysicsObject::updateObjects(double deltaTime, SDL_Renderer* rend)
{
    for(int i = 0; i < collisionObjects.size(); i++)
    {
        collisionObjects[i]->groundCheck();
        collisionObjects[i]->update(deltaTime);
        collisionObjects[i]->draw(rend);
    }
    for(int i = 0; i < noncollisionObjects.size(); i++)
    {
        noncollisionObjects[i]->update(deltaTime);
        noncollisionObjects[i]->draw(rend);
    }
}