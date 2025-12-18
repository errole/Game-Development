#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <stdlib.h>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "Entity.h"
#include "Utilities.h"

using namespace std;

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
GLuint barTexture=0;
GLuint spriteSheet=0;
GLuint fontTexture=0;
GLuint backGroundTexture=0;
enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_WIN, STATE_LOSE};
int state = STATE_MAIN_MENU;

Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;

vector<Entity> entities;
vector<Entity> bullets;
vector<Entity> enemyBullets;
Entity humanShip;
Entity leftBorder;
Entity rightBorder;
Entity backGround;
Entity lossLine;
Entity enemyBulletReachLine;

void reset(){
    humanShip = Entity(0.0f,-2.8f,.2f,.2f,5);
    
    entities.clear();
    for(int i=0;i<12;i++){
        for(int j=0;j<5;j++){
            Entity myEntity;
            myEntity = Entity(-5.0+i*.6,2.8-j*.6,.3,.3,1);
            entities.push_back(myEntity);
        }
    }
    enemyBullets.clear();
    bullets.clear();
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
    
    leftBorder = Entity(-5.45f,0.0f,.1f,3.0f);
    rightBorder = Entity(5.45f,0.0f,.1f,3.0f);
    lossLine = Entity(0.0f,-2.3f,5.55f,.05f);
    enemyBulletReachLine = Entity(0.0f,-3.0f,5.55f,.05f);
    
    backGround = Entity(0.0f, 0.0f,5.55f,3.0f);
    
    reset();
}

void ProcessEvents(){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            done = true;
        }else if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                Entity newBullet(humanShip.x,humanShip.y,.1,.1,0,5);
                bullets.push_back(newBullet);
            }
        }
    }
}

void UpdateGameLevel(){
    //Update timer for smoother movements
    float ticks = (float)SDL_GetTicks()/1000.0f;
    float elapsed = ticks - lastFrameTicks;
    lastFrameTicks = ticks;
    
    //Get input for movement of ship
    if(keys[SDL_SCANCODE_RIGHT]) {
        humanShip.x += elapsed * humanShip.speed_x;
    }else if(keys[SDL_SCANCODE_LEFT]){
        humanShip.x += elapsed * -humanShip.speed_x;
    }
    
    //Check for humanShip collision
    if(rightBorder.isCollision(humanShip)){
        humanShip.x += elapsed * -humanShip.speed_x;
    }else if(leftBorder.isCollision(humanShip)){
        humanShip.x += elapsed * humanShip.speed_x;
    }
    
    //Random Bullet movement from UFOs
    if(enemyBullets.size()<=3){
        int randNum = rand() % entities.size();
        Entity newBullet(entities[randNum].x,entities[randNum].y,.1,.1,0,-5);
        enemyBullets.push_back(newBullet);
    }
    
    
    //Bullet Movements
    for(int i=0;i<bullets.size();i++){
        bullets[i].y += elapsed * bullets[i].speed_y;
    }
    for(int i=0;i<enemyBullets.size();i++){
        enemyBullets[i].y += elapsed * enemyBullets[i].speed_y;
    }
    
    //Remove Bullets
    for(int i=0;i<enemyBullets.size();i++){
        if(enemyBulletReachLine.isCollision(enemyBullets[i])){
            enemyBullets.erase(enemyBullets.begin()+i);
        }
    }
        
    //UFO movements
    for(int i=0;i<entities.size();i++){
        if(rightBorder.isCollision(entities[i])){
            for(int i=0;i<entities.size();i++){
                entities[i].speed_x = -1;
                entities[i].y -= elapsed * 5;
            }
        }else if(leftBorder.isCollision(entities[i])){
            for(int i=0;i<entities.size();i++){
                entities[i].speed_x = 1;
                entities[i].y -= elapsed * 5;
            }
        }
        entities[i].x += elapsed * entities[i].speed_x;
    }
    
    //Collison Checks for Bullets
    for(int i=0;i<entities.size();i++){
        for(int j=0;j<bullets.size();j++){
            if(bullets[j].isCollision(entities[i])){
                entities.erase(entities.begin()+i);
                bullets.erase(bullets.begin()+j);
            }
        }
    }
    
    //Collison Checks for Bullets
    for(int j=0;j<enemyBullets.size();j++){
        if(enemyBullets[j].isCollision(humanShip)){
            state = STATE_LOSE;
        }
    }
    
    //Win Check
    if(entities.size()==0){
        state = STATE_WIN;
    }
    for(int i=0;i<entities.size();i++){
        if(entities[i].isCollision(lossLine)){
            state = STATE_LOSE;
        }
    }
}

void RenderGameLevel(ShaderProgram program){
    glClear(GL_COLOR_BUFFER_BIT);
    float textureCoord[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0,
                            0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    backGround.DrawSprite( program, backGroundTexture, textureCoord);

    float u=434.0f/1024.0f;
    float v=234.0f/1024.0f;
    float width= 91.0f/1024.0f;
    float height= 91.0f/1024.0f;
    float ufoGreenUV[] = { u, v+height, u+width, v+height, u+width, v,
                            u, v+height, u+width, v, u, v};
   
    u=325.0f/1024.0f;
    v=0.0f/1024.0f;
    width= 98.0f/1024.0f;
    height= 75.0f/1024.0f;
    float playerShip[] = {u, v+height, u+width, v+height, u+width, v,
                            u, v+height, u+width, v, u, v};

    u=856.0f/1024.0f;
    v=57.0f/1024.0f;
    width= 9.0f/1024.0f;
    height= 37.0f/1024.0f;
    float lasers[] = {u, v+height, u+width, v+height, u+width, v,
                        u, v+height, u+width, v, u, v};

    humanShip.DrawSprite( program, spriteSheet, playerShip);
    rightBorder.DrawSprite( program, barTexture, textureCoord);
    leftBorder.DrawSprite( program, barTexture, textureCoord);
    enemyBulletReachLine.DrawSprite( program, barTexture, textureCoord);
    lossLine.DrawSprite( program, barTexture, textureCoord);
    for(int i=0;i<entities.size();i++){
        entities[i].DrawSprite( program, spriteSheet, ufoGreenUV);
    }
    for(int i=0;i<bullets.size();i++){
        bullets[i].DrawSprite( program, spriteSheet, lasers);
    }
    for(int i=0;i<enemyBullets.size();i++){
        enemyBullets[i].DrawSprite( program, spriteSheet, lasers);
    }
    
    SDL_GL_SwapWindow(displayWindow);
}

void UpdateMainMenu(){
    if(keys[SDL_SCANCODE_SPACE]) {
        state= STATE_GAME_LEVEL;
    }
}

void UpdateWin(){
    if(keys[SDL_SCANCODE_SPACE]) {
        state= STATE_GAME_LEVEL;
    }
}

void UpdateLose(){
    if(keys[SDL_SCANCODE_SPACE]) {
        reset();
        state= STATE_GAME_LEVEL;
    }
}

void RenderMainMenu(ShaderProgram program){
    glClear(GL_COLOR_BUFFER_BIT);
    float textureCoord[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    backGround.DrawSprite( program, backGroundTexture, textureCoord);
    
    float adjust[] = {-3.33,0,0};
    DrawText(&program, fontTexture, "SPACE INVADERS!", 1, -.5, adjust);
    adjust[0]= -2;
    adjust[1] = -.5;
    DrawText(&program, fontTexture, "Press Space to Start", .5, -.3, adjust);
    SDL_GL_SwapWindow(displayWindow);
}

void RenderWin(ShaderProgram program){
    glClear(GL_COLOR_BUFFER_BIT);
    float textureCoord[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    backGround.DrawSprite( program, backGroundTexture, textureCoord);
    
    float adjust[] = {-2.0,0,0};
    DrawText(&program, fontTexture, "You WIN!", 1, -.5, adjust);
    adjust[0]= -2.0;
    adjust[1] = -.5;
    DrawText(&program, fontTexture, "Press Space to Start", .5, -.3, adjust);
    SDL_GL_SwapWindow(displayWindow);
}

void RenderLose(ShaderProgram program){
    glClear(GL_COLOR_BUFFER_BIT);
    float textureCoord[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    backGround.DrawSprite( program, backGroundTexture, textureCoord);
    
    float adjust[] = {-3.33,0,0};
    DrawText(&program, fontTexture, "You Lose Earthling!", 1, -.6, adjust);
    adjust[0]= -2;
    adjust[1] = -.5;
    DrawText(&program, fontTexture, "Press Space to Start", .5, -.3, adjust);
    SDL_GL_SwapWindow(displayWindow);
}

int main(int argc, char *argv[])
{
    Setup();
    
    spriteSheet = LoadTexture("sheet.png");
    barTexture = LoadTexture("bar.png");
    fontTexture = LoadTexture("font1.png");
    backGroundTexture = LoadTexture("purple.png");
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    glUseProgram(program.programID);
    program.setModelMatrix(modelMatrix);
    program.setProjectionMatrix(projectionMatrix);
    program.setViewMatrix(viewMatrix);
    
    while (!done) {
        ProcessEvents();
        
        switch(state) {
            case STATE_MAIN_MENU:
                UpdateMainMenu();
                break;
            case STATE_GAME_LEVEL:
                UpdateGameLevel();
                break;
            case STATE_WIN:
                UpdateWin();
                break;
            case STATE_LOSE:
                UpdateLose();
                break;
        }
        
        switch(state) {
            case STATE_MAIN_MENU:
                RenderMainMenu(program);
                break;
            case STATE_GAME_LEVEL:
                RenderGameLevel(program);
                break;
            case STATE_WIN:
                RenderWin(program);
                break;
            case STATE_LOSE:
                RenderLose(program);
                break;
        }
    }
    SDL_Quit();
    return 0;
}



