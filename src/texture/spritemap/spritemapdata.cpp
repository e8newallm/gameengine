#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/schema.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "spritemapdata.h"
#include "SpriteMapSchema.h"

SpriteMapData::SpriteMapData() :
    textures()
    , sprites()
    , animations()
{
}

void SpriteMapData::loadFromFile(SDL_Renderer* rend, const char* configLocation)
{
    std::ifstream file(configLocation);

    if(!file.is_open())
    {
        throw std::runtime_error(std::string("Could not open Spritemap JSON \"") + configLocation + "\"");
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    loadFromString(rend, ss.str().c_str(), configLocation);
}

void SpriteMapData::loadFromString(SDL_Renderer* rend, const char* spriteConfig, const char* source)
{
    rapidjson::Document schema;
    schema.Parse(SpriteMapSchema);
    rapidjson::SchemaDocument schemaDoc(schema);
    rapidjson::SchemaValidator validator(schemaDoc);

    rapidjson::Document config;
    config.Parse(spriteConfig);

    if(config.HasParseError())
    {
        throw std::runtime_error(std::string("\"") + source + "\" is invalid JSON");
    }

    if(!config.Accept(validator))
    {
        rapidjson::StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        throw std::runtime_error(std::string("\"") + source + "\" has failed to pass SpriteMap schema");
    }

    if(config["Textures"].IsArray())
    {
        for(rapidjson::Value& value : config["Textures"].GetArray())
        {
            SDL_Surface* surface = IMG_Load(value.GetString());
            if(surface == nullptr)
            {
                throw std::runtime_error(std::string("\"") + source + "\" could not load texture file \"" + value.GetString() + "\"");
            }

            std::pair<const char*, SDL_Texture*> newTexture = std::make_pair(value.GetString(),
                                        SDL_CreateTextureFromSurface(rend, surface));
            textures.insert(newTexture);
        }
    }
    else
    {
        SDL_Surface* surface = IMG_Load(config["Textures"].GetString());
        if(surface == nullptr)
        {
            throw std::runtime_error(std::string("\"") + source + "\" could not load texture file \"" + config["Textures"].GetString() + "\"");
        }
        std::pair<const char*, SDL_Texture*> newTexture = std::make_pair(config["Textures"].GetString(),
                                            SDL_CreateTextureFromSurface(rend, surface));
        textures.insert(newTexture);
    }
    
    for(rapidjson::Value& value : config["Sprites"].GetArray())
    {
        Sprite newEntry;
        rapidjson::GenericObject<false, rapidjson::Value> sprite = value.GetObject();
        if(sprites.find(sprite.FindMember("name")->value.GetString()) != sprites.end())
        {
            throw std::runtime_error(std::string("\"") + source + "\" has two sprites named (" + sprite.FindMember("name")->value.GetString() + ")");
        }
        if(textures.find(sprite.FindMember("texture")->value.GetString()) == textures.end())
        {
            throw std::runtime_error(std::string("\"") + source + "\" has a sprite (" + 
                                    sprite.FindMember("name")->value.GetString() + ") referencing a texture not named in the JSON");
        }

        newEntry.position = { sprite.FindMember("x")->value.GetInt()
                             , sprite.FindMember("y")->value.GetInt()
                             , sprite.FindMember("height")->value.GetInt()
                             , sprite.FindMember("width")->value.GetInt() };
        newEntry.texture = textures.find(sprite.FindMember("texture")->value.GetString())->second;
        newEntry.textureName = sprite.FindMember("texture")->value.GetString();
        sprites.insert(std::make_pair(sprite.FindMember("name")->value.GetString(), newEntry));
    }

    if(config.HasMember("Animations"))
    {
        for(rapidjson::Value& value : config["Animations"].GetArray())
        {
            Animation newAni;
            rapidjson::GenericObject<false, rapidjson::Value> animation = value.GetObject();
            if(animations.find(animation.FindMember("name")->value.GetString()) != animations.end())
            {
                throw std::runtime_error(std::string("\"") + source + "\" has two animations named (" + animation.FindMember("name")->value.GetString() + ")");
            }

            newAni.FPS = animation.FindMember("FPS")->value.GetDouble();
            for(rapidjson::Value& i : animation.FindMember("frames")->value.GetArray())
            {
                if(sprites.find(i.GetString()) == sprites.end())
                {
                    throw std::runtime_error(std::string("\"") + source + "\" has a animation (" + 
                                        animation.FindMember("name")->value.GetString() + ") referencing a sprite not named in the JSON");
                }

                newAni.sprites.push_back((char*)i.GetString());
            }
            animations.insert(std::make_pair(animation.FindMember("name")->value.GetString(), newAni));
        }
    }
}

void SpriteMapData::save(const char* spriteConfig)
{
    std::string data = serialise();

    std::ofstream output;
    output.open(spriteConfig, std::ofstream::out | std::ofstream::trunc);
    output << data;
    output.close();
}

std::string SpriteMapData::serialise()
{
    rapidjson::Document schema;
    schema.Parse(SpriteMapSchema);
    rapidjson::SchemaDocument schemaDoc(schema);
    rapidjson::SchemaValidator validator(schemaDoc);

    rapidjson::Document config;
    rapidjson::Document::AllocatorType& allocator = config.GetAllocator();
    config.SetObject();

    if(textures.size() == 0)
    {
        throw std::runtime_error("Serialise: Spritemap JSON has no textures defined!");
    }

    rapidjson::Value texArray(rapidjson::kArrayType);
    for (std::map<std::string, SDL_Texture*>::iterator it = textures.begin(); it != textures.end(); it++)
    {
        texArray.PushBack(rapidjson::Value(it->first.c_str(), allocator), allocator);
    }
    config.AddMember("Textures", texArray, allocator);

    rapidjson::Value sprArray(rapidjson::kArrayType);
    for (std::map<std::string, Sprite>::iterator it = sprites.begin(); it != sprites.end(); it++)
    {
        rapidjson::Value value;
        value.SetObject();
        value.AddMember("name", rapidjson::Value().SetString(it->first.c_str(), allocator), allocator);
        value.AddMember("texture", rapidjson::Value().SetString(it->second.textureName.c_str(), allocator), allocator);
        value.AddMember("x", rapidjson::Value().SetUint64(it->second.position.x), allocator);
        value.AddMember("y", rapidjson::Value().SetUint64(it->second.position.y), allocator);
        value.AddMember("width", rapidjson::Value().SetUint64(it->second.position.w), allocator);
        value.AddMember("height", rapidjson::Value().SetUint64(it->second.position.h), allocator);
        sprArray.PushBack(value, allocator);
    }
    config.AddMember("Sprites", sprArray, allocator);

    if(!animations.empty())
    {
        rapidjson::Value aniArray(rapidjson::kArrayType);
        for (std::map<std::string, Animation>::iterator it = animations.begin(); it != animations.end(); it++)
        {
            rapidjson::Value value;
            value.SetObject();
            value.AddMember("name", rapidjson::Value().SetString(it->first.c_str(), allocator), allocator);
            value.AddMember("FPS", rapidjson::Value().SetFloat(it->second.FPS), allocator);
            rapidjson::Value frameArray(rapidjson::kArrayType);
            for (std::vector<std::string>::iterator itTwo = it->second.sprites.begin(); itTwo != it->second.sprites.end(); itTwo++)
            {
                frameArray.PushBack(rapidjson::Value().SetString(itTwo->c_str(), allocator), allocator);
            }
            value.AddMember("frames", frameArray, allocator);
            aniArray.PushBack(value, allocator);
        }
        config.AddMember("Animations", aniArray, allocator);
    }

    if(!config.Accept(validator))
    {
        rapidjson::StringBuffer sb;
        validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
        throw std::runtime_error("Serialise: Spritemap JSON has failed to pass SpriteMap schema");
    }

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    config.Accept(writer);

    return buffer.GetString();
}