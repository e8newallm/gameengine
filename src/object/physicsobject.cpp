#include <string>
#include <vector>
#include <cmath>

#include "physicsobject.h"
#include "graphics.h"
#include "logging.h"
#include "object.h"

namespace GameEng
{
    PhysicsObject::PhysicsObject(SDL_Rect body, int flags, Texture_base* texture) :
        Object(body, texture)
    ,_isStatic(flags & PHYOBJ_STATIC)
    ,_canCollide(flags & PHYOBJ_COLLIDE)
    ,nextBody(body)
    ,interBody(body)
    ,currentVelocity()
    ,nextVelocity()
    {
        body.x = nextBody.x;
        body.y = nextBody.y;
    }

    void PhysicsObject::velocity(double x, double y)
    {
        nextVelocity.x = x;
        nextVelocity.y = y;
    }

    void PhysicsObject::velocityDelta(double x, double y)
    {
        nextVelocity.x += x;
        nextVelocity.y += y;
    }

    bool PhysicsObject::onGround(const World& world) const
    {
        SDL_Rect groundCheck;
        groundCheck.x = body.x;
        groundCheck.y = body.y + body.h;
        groundCheck.w = body.w;
        groundCheck.h = 2;
        std::vector<Object*> objects = world.getObjects();
        for(uint64_t i = 0; i < objects.size(); i++)
        {
            const PhysicsObject* obj = dynamic_cast<PhysicsObject*>(objects[i]);
            if(obj != this && obj != nullptr && SDL_HasRectIntersection(&groundCheck, objects[i]->getBody()))
            {
                return true;
            }
        }
        return false;
    }

    ShaderObjData PhysicsObject::predraw()
    {
        struct ObjData {
            SDL_Rect body;
            SDL_FRect texBody;
        };

        ObjData* data = static_cast<ObjData*>(malloc(sizeof(ObjData)));
        data->body = *getBody();
        data->texBody = tex->getUV();
        return {data, sizeof(ObjData)};
    }

    void PhysicsObject::update(double deltaTime, World& world)
    {
        interBody.x = body.x + (nextBody.x - body.x) * world.getPhyInterpolation();
        interBody.y = body.y + (nextBody.y - body.y) * world.getPhyInterpolation();
        interBody.h = body.h;
        interBody.w = body.w;
        
        Object::update(deltaTime,world);
    }

    void PhysicsObject::runPhysics(double deltaTime, World& world)
    {
        if(isStatic())
            return;

        body.x = nextBody.x;
        body.y = nextBody.y;
        currentVelocity.x = nextVelocity.x;
        currentVelocity.y = nextVelocity.y;

        if(!onGround(world))
        {
            velocityDelta(0, world.getGravity() * deltaTime);
        }

        SDL_FPoint displacement;
        displacement.x = ((currentVelocity.x + nextVelocity.x) / 2) * deltaTime;
        displacement.y = ((currentVelocity.y + nextVelocity.y) / 2) * deltaTime;
        std::vector<Object*> objects = world.getObjects();
        SDL_Rect collisionArea;

        nextBody.x += displacement.x;
        for(uint64_t i = 0; i < objects.size(); i++)
        {
            const PhysicsObject* obj = dynamic_cast<PhysicsObject*>(objects[i]);

            if(obj != nullptr && SDL_GetRectIntersection(&nextBody, obj->getBody(), &collisionArea) && this != objects[i])
            {
                if(obj->getBody()->x < body.x)
                {
                    nextBody.x += collisionArea.w;
                }
                else
                {
                    nextBody.x -= collisionArea.w;
                }
                currentVelocity.x = 0;
                nextVelocity.x = 0;
            }
        }

        nextBody.y += displacement.y;
        for(uint64_t i = 0; i < objects.size(); i++)
        {
            const PhysicsObject* obj = dynamic_cast<PhysicsObject*>(objects[i]);
            if(obj != nullptr && SDL_GetRectIntersection(&nextBody, obj->getBody(), &collisionArea) && this != objects[i])
            {
                if(obj->getBody()->y < body.y)
                {
                    nextBody.y += collisionArea.h;
                }
                else
                {
                    nextBody.y -= collisionArea.h;
                }
                currentVelocity.y = 0;
                nextVelocity.y = 0;
            }
        }
    }
}