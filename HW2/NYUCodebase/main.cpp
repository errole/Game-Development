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

//Global Setup Parameters
SDL_Window* displayWindow;
SDL_Event event;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
bool done = false;
float lastFrameTicks = 0.0f;
float elapsed = 0.0;
GLuint BarTexture=0;
GLuint PLeftWinsTexture=0;
GLuint PRightWinsTexture=0;


Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;


class Entity {
public:
    float x;
    float y;
    float angle;
    int textureID;
    float width;
    float height;
    float speed;
    float direction_x = 0;
    float direction_y = 0;
    
    Entity() {};
    Entity(float x, float y, float width, float height, float speed = 0, float angle = 0):x(x),y(y),width(width),height(height) ,speed(speed), angle(angle) {};
    
    void DrawSprite(ShaderProgram& program, GLuint& texture, float textureCoord[]) {
        
        float vertices[] = {x-width, y-height,x+width, y-height,x+width, y+height,
            x-width, y-height,x+width, y+height,x-width, y+height};
        Matrix modelMatrix;
        //modelMatrix.Translate(0.0, 0.0, 0.0);
        //modelMatrix.Rotate();
        
        program.setModelMatrix(modelMatrix);
        glBindTexture(GL_TEXTURE_2D, texture);
        glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program.positionAttribute);
        glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, textureCoord);
        glEnableVertexAttribArray(program.texCoordAttribute);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    bool isCollision(Entity other){
        if(y-height > other.y+other.height || y+height < other.y-other.height ||
           x-width > other.x+other.width || x+width < other.x-other.width){
            return 0;
        }else{
            return 1;
        }
    }
};


Entity paddle1;
Entity paddle2;
Entity ball;
Entity botmBoarder;
Entity topBoarder;
Entity PLeftWins;

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

void Setup(){
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 360);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    glClearColor(.0f,.0f,.3f,.0f);
    projectionMatrix.setOrthoProjection(-5.55, 5.55, -3.0f, 3.0f, -1.0f, 1.0f);
}

void ProcessEvents(){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }
    }
}

void Update(){
    //Update timer for smoother movements
    float ticks = (float)SDL_GetTicks()/1000.0f;
    float elapsed = ticks - lastFrameTicks;
    lastFrameTicks = ticks;
    
    //Get input for movement of paddles
    if(keys[SDL_SCANCODE_W]) {
        paddle1.y += elapsed * paddle1.speed;
    }else if(keys[SDL_SCANCODE_S]){
        paddle1.y += elapsed * -paddle1.speed;
    }
    if(keys[SDL_SCANCODE_UP]){
        paddle2.y += elapsed * paddle2.speed;
    }else if(keys[SDL_SCANCODE_DOWN]){
        paddle2.y += elapsed * -paddle2.speed;
    }
    
    //Check for paddle1 collision
    if(paddle1.isCollision(topBoarder)){
        paddle1.y += elapsed * -paddle1.speed;
    }else if(paddle1.isCollision(botmBoarder)){
        paddle1.y += elapsed * paddle1.speed;
    }
    
    //Check for paddle2 collision
    if(paddle2.isCollision(topBoarder)){
        paddle2.y += elapsed * -paddle2.speed;
    }else if(paddle2.isCollision(botmBoarder)){
        paddle2.y += elapsed * paddle2.speed;
    }
    
    //Check for ball collision
    if(ball.isCollision(topBoarder)){
        ball.angle = -ball.angle;
    }else if(ball.isCollision(botmBoarder)){
        ball.angle = -ball.angle;
    }else if(ball.isCollision(paddle1)){
        ball.angle = (360-atan((ball.y-paddle1.y)/(ball.x-paddle1.x))*180/3.14);
    }else if(ball.isCollision(paddle2)){
        ball.angle = (180-atan((paddle2.y-ball.y)/(paddle2.x-ball.x))*180/3.14);
    }
    ball.x += cos(ball.angle*3.14/180) * elapsed * ball.speed;
    ball.y += sin(ball.angle*3.14/180) * elapsed * ball.speed;
    
}

void Render(ShaderProgram program){
    glClear(GL_COLOR_BUFFER_BIT);
    float textureCoords00[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0,0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    
    botmBoarder.DrawSprite( program, BarTexture, textureCoords00);
    topBoarder.DrawSprite( program, BarTexture, textureCoords00);
    paddle1.DrawSprite( program, BarTexture, textureCoords00);
    paddle2.DrawSprite( program, BarTexture, textureCoords00);
    ball.DrawSprite( program, BarTexture, textureCoords00);
    
    if(!(ball.x-ball.width > -5.55)){
        PLeftWins.DrawSprite( program, PRightWinsTexture, textureCoords00);
    }else if(!(ball.x+ball.width < 5.55)){
        PLeftWins.DrawSprite( program, PLeftWinsTexture, textureCoords00);
    }
    
    SDL_GL_SwapWindow(displayWindow);
}


int main(int argc, char *argv[])
{
    Setup();
    BarTexture = LoadTexture("bar.png");
    PLeftWinsTexture = LoadTexture("LeftPlayerWin.png");
    PRightWinsTexture = LoadTexture("RightPlayerWin.png");
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
    program.setModelMatrix(modelMatrix);
    program.setProjectionMatrix(projectionMatrix);
    program.setViewMatrix(viewMatrix);
    
    
    
    paddle1 = Entity(-5.4f,0.0f,.1f,.7f,5);
    paddle2 = Entity(5.4f,0.0f,.1f,.7f,5);
    ball = Entity(0.0f,0.0f,.1f,.1f,5.0f);
    botmBoarder = Entity(0.0f,-2.9f,5.55f,.1f);
    topBoarder = Entity(0.0f,2.9f,5.55f,.1f);
    PLeftWins = Entity(0.0f,0.0f,2.0f,1.0f);
    
    while (!done) {
        ProcessEvents();
        Update();
        Render(program);
    }
    
    SDL_Quit();
    return 0;
}
