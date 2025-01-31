#include "object.h"
#include "graphics.h"
#include "world.h"

Object::Object(SDL_Rect body, Texture_base* texture) :
    body(body)
   ,tex(texture)
{

}

Object::~Object()
{
    delete tex;
}

void Object::draw(World* world, SDL_GPUCommandBuffer* cmdbuf, SDL_GPURenderPass* renderPass, double deltaT)
{
    draw(world, cmdbuf, renderPass, ShaderObjData{body}, deltaT);
}

void Object::draw(World* world, SDL_GPUCommandBuffer* cmdbuf, SDL_GPURenderPass* renderPass, ShaderObjData objData, double deltaT)
{
    tex->update(deltaT);
    tex->draw(world, cmdbuf, renderPass, objData);

}
void Object::update(double deltaTime)
{
    (void)deltaTime;
}

void Object::move(double x, double y)
{
    body.x = x;
    body.y = y;
}

void Object::moveDelta(double x, double y)
{
    body.x += x;
    body.y += y;
}

void Object::resize(double height, double width)
{
    body.h = height;
    body.w = width;
}

void Object::resizeDelta(double height, double width)
{
    body.h += height;
    body.w += width;
}