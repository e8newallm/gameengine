#include <iostream>

#include "object.h"
#include "keystate.h"

Object::Object(SDL_Rect body, Texture_base* texture) :
    body(body)
   ,updateBody()
   ,tex(texture)
{
    SDL_QueryTexture(tex->getTexture().texture, NULL, NULL, NULL, NULL);
    
    updateBody.x = body.x;
    updateBody.y = body.y;
    updateBody.h = body.h;
    updateBody.w = body.w;
}

Object::~Object()
{
    SDL_DestroyTexture(tex->getTexture().texture);
}

void Object::draw(SDL_Renderer* rend, double deltaT)
{
    draw(rend, &body, deltaT);
}

void Object::draw(SDL_Renderer* rend, SDL_Rect* bodyPos, double deltaT)
{
    tex->update(deltaT);
    TexRequest texture = tex->getTexture();
    SDL_RenderCopyEx(rend, texture.texture, &(texture.position), bodyPos, 0.0, NULL, SDL_FLIP_NONE);
}

void Object::update(double deltaTime)
{
    (void)deltaTime;
}

void Object::move(double x, double y)
{
    updateBody.x = x;
    updateBody.y = y;
}

void Object::moveInstantly(double x, double y)
{
    body.x = x;
    body.y = y;
}


void Object::moveDelta(double x, double y)
{
    updateBody.x += x;
    updateBody.y += y;
}

void Object::moveDeltaInstantly(double x, double y)
{
    body.x += x;
    body.y += y;
}

void Object::resize(double height, double width)
{
    updateBody.h = height;
    updateBody.w = width;
}

void Object::resizeDelta(double height, double width)
{
    updateBody.h += height;
    updateBody.w += width;
}