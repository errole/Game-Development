//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Errol Elbasan on 3/1/16.
//

#include "Entity.h"
    
void Entity::DrawSprite(ShaderProgram& program, GLuint& texture, float textureCoord[]) {
        
    float vertices[] = {x-width, y-height,x+width, y-height,x+width, y+height,
        x-width, y-height,x+width, y+height,x-width, y+height};
    Matrix modelMatrix;
    
    program.setModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D, texture);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoord);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
bool Entity::isCollision(Entity other){
    if(y-height > other.y+other.height || y+height < other.y-other.height ||
        x-width > other.x+other.width || x+width < other.x-other.width){
        return 0;
    }else{
        return 1;
    }
}

