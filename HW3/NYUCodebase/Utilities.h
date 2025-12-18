//
//  Utilities.hpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/1/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Utilities_h
#define Utilities_h

#include <stdio.h>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"
#include "Matrix.h"
using namespace std;

#endif /* Utilities_hpp */

void DrawText(ShaderProgram *program, GLuint fontTexture, std::string text, float size, float spacing, float adjust[3]);

GLuint LoadTexture(const char *image_path);


class SheetSprite {
public:
    void Draw();
    float size;
    GLuint textureID;
    float u;
    float v;
    float width;
    float height;
    
    SheetSprite();
    SheetSprite(GLuint textureID, float u, float v, float width, float height, float size);
};


