//
//  Entity.hpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/1/16.
//

#ifndef Entity_h
#define Entity_h

#include <stdio.h>
#include "ShaderProgram.h"

#endif /* Entity_hpp */

class Entity {
public:
    float x;
    float y;
    float angle;
    int textureID;
    float width;
    float height;
    float direction_x = 0;
    float direction_y = 0;
    float speed_x = 0;
    float speed_y = 0;
    
    Entity(){};
    
    Entity(float x, float y, float width, float height, float speed_x = 0, float speed_y = 0, float angle = 0):x(x), y(y), width(width), height(height), speed_x(speed_x), speed_y(speed_y), angle(angle) {};
    
    void DrawSprite(ShaderProgram& program, GLuint& texture, float textureCoord[]);
    bool isCollision(Entity other);
};