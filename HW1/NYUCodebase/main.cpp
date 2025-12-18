#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "Matrix.h"
#include "ShaderProgram.h"


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(surface);
    return textureID;
}

void DrawSprite(ShaderProgram& program, GLuint& texture,float vertices[],float textureCoords[],
                float x, float y, float angle) {
    
    Matrix modelMatrix;
    
    modelMatrix.Translate(x, y, 0.0);
    modelMatrix.Rotate(angle);
    
    program.setModelMatrix(modelMatrix);
    glBindTexture(GL_TEXTURE_2D, texture);
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 360);
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    GLuint ShipTexture = LoadTexture("Ship.png");
    GLuint EvilShipTexture = LoadTexture("EvilShip.png");
    GLuint laserRed02Texture = LoadTexture("laserRed02.png");
    GLuint explosionTexture = LoadTexture("explosion.png");
    
    Matrix projectionMatrix;
    Matrix modelMatrix;
    Matrix viewMatrix;
    projectionMatrix.setOrthoProjection(-5.55, 5.55, -3.0f, 3.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    glClearColor(.0f,.0f,.3f,.0f);
    float lastFrameTicks = 0.0f;
    float variable;
    
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float)SDL_GetTicks()/1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        
        float vertices00[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5,-0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
        float textureCoords00[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0,0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        DrawSprite( program, ShipTexture, vertices00, textureCoords00, 0, 0, 0);
        
        float vertices01[] = {0.5, 0.5, 1.0, 0.5, 1.0, 1.0,0.5, 0.5, 1.0, 1.0, 0.5, 1.0,};
        float textureCoords01[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0,0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        DrawSprite( program, ShipTexture, vertices01, textureCoords01, 0, 0, 0);
        
        float vertices02[] = {-1.0, 0.5, -0.5, 0.5, -0.5, 1.0,-1.0, 0.5, -0.5, 1.0, -1.0, 1.0};
        float textureCoords02[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        DrawSprite( program, ShipTexture, vertices02, textureCoords02, 0, 0, 0);
        
        float vertices1[] = {-0.1, 0.8, 0.1, 0.8, 0.1, 1.3, -0.1, 0.8, 0.1, 1.3, -0.1, 1.3};
        float textureCoords1[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        DrawSprite( program, laserRed02Texture, vertices1, textureCoords1, 0, -variable, 0);
        
        float vertices2[] = {-1.5, 1.0, 1.5, 1.0, 1.5, 3.0, -1.5, 1.0, 1.5, 3.0, -1.5, 3.0};
        float textureCoords2[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
        DrawSprite( program, EvilShipTexture, vertices2, textureCoords2, 0, 0, 0);
        
        if(variable > .6){
            float vertices2[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5,-0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
            float textureCoords2[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0,0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
            DrawSprite( program, explosionTexture, vertices2, textureCoords2, 0, 0, 0);
        }
        variable += elapsed;
        SDL_GL_SwapWindow(displayWindow);
    }
    //wefwefwefewf
    SDL_Quit();
    return 0;
}
