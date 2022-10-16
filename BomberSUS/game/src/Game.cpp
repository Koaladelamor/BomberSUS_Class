/*******************************************************************************************
*
*   raylib [core] example - Initialize 3d camera mode
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <math.h>

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

#pragma region Variables

bool gameOver = false;
bool player1Wins = false;
bool player2Wins = false;
float version = 0.0;
std::string bgMusic_string = "";
float cameraHeight = -1.0;
std::string title = "";
int totalTextures = 0;
std::map<std::string, std::string> textures;
std::map<std::string, Texture2D> level_textures;

int background_w = 0;
int background_h = 0;
std::string** background;

int foreground_w = 0;
int foreground_h = 0;
std::string** foreground;

int objects_w = 0;
int objects_h = 0;
std::string** objects;

Vector3** fg_positions;

struct Player {
    Vector3 position;
    Color color;
    int maxBombs;
    int num;
    bool dead;
};

struct Bombs {
    float pos_x;
    float pos_z;
    float timeToExplode;
};

std::vector <Player*> players;

std::vector <Bombs*> player1Bombs;
std::vector <Bombs*> player2Bombs;

std::vector <Vector3> powerUps;

#pragma endregion

int LoadMap() {
    std::string temp;
    std::fstream file;

    file.open("level.sus", std::ifstream::in);

    //Open file and check format

    if (!file.is_open()) {
        file.close();
        std::cout << "Error 1 : Couldn't open the file" << std::endl;
        return 1;
    }

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp != "SUS") {
        std::cout << "Error 2 : Incorrect format" << std::endl;
        return 2;
    }

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;

    version = std::stof(temp);

    std::getline(file, temp);

    //Title

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp != "TITLE") {
        std::cout << "Error 3 : Title not found" << std::endl;
        return 3;
    }

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp == "")
    {
        std::cout << "Error 4 : Title is empty" << std::endl;
        return 4;
    }
    else
    {
        title = temp;
    }

    std::getline(file, temp);

    //Music

    if (version >= 0.5) {
        std::getline(file, temp, ';');
        std::cout << temp << std::endl;
        if (temp != "MUSIC") {
            std::cout << "Error 5 : Music not found" << std::endl;
            return 5;
        }

        std::getline(file, temp, ';');
        std::cout << temp << std::endl;
        std::string extensionName = temp.substr(temp.find("."));
        std::cout << "EXTENSION NAME IS: " << extensionName << std::endl;
        if (extensionName != ".ogg") {
            std::cout << "Error 6 : Extension name of bg music is not correct. Need .ogg" << std::endl;
            return 6;
        }
        bgMusic_string = temp;

        std::getline(file, temp);
    }

    //Camera Height

    if (version >= 1.0) {
        std::getline(file, temp, ';');
        std::cout << temp << std::endl;
        if (temp != "CAMERA_HEIGHT") {
            std::cout << "Error 7 : Camera height not found" << std::endl;
            return 7;
        }

        std::getline(file, temp, ';');
        std::cout << temp << std::endl;
        cameraHeight = std::stof(temp);

        std::getline(file, temp);
    }

    if (version <= 0.21) {
        std::getline(file, temp);
        std::getline(file, temp);
    }
    else if (version <= 0.51) {
        std::getline(file, temp);
    }
    //Textures

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp != "TEXTURES") {
        std::cout << "Error 8 : Textures not found" << std::endl;
        return 8;
    }

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    totalTextures = std::stoi(temp);
    //std::cout << totalTextures << std::endl;
    std::getline(file, temp);

    for (size_t i = 0; i < totalTextures; i++)
    {
        std::string key;
        std::getline(file, key, ';');
        //std::cout << temp << std::endl;

        std::string value;
        std::getline(file, value, ';');
        //std::cout << temp << std::endl;

        textures.insert(std::pair<std::string, std::string>(key, value));
        std::cout << key << " : " << textures[key] << std::endl;

        std::getline(file, temp);
    }


    //BACKGROUND

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp != "BACKGROUND") {
        std::cout << "Error 9 : Background not found" << std::endl;
        return 9;
    }

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    background_w = std::stoi(temp);

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    background_h = std::stoi(temp);

    std::getline(file, temp);

    background = new std::string * [background_h];

    for (size_t i = 0; i < background_h; i++) {
        background[i] = new std::string[background_w];
    }

    for (size_t i = 0; i < background_h; i++)
    {
        for (size_t j = 0; j < background_w; j++)
        {
            std::getline(file, temp, ';');
            background[i][j] = temp;
            std::cout << background[i][j] << " - ";
        }
        std::getline(file, temp);
        std::cout << std::endl;
    }

    //FOREGROUND
    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp != "FOREGROUND") {
        std::cout << "Error 10 : Foreground not found" << std::endl;
        return 10;
    }

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    foreground_w = std::stoi(temp);

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    foreground_h = std::stoi(temp);

    std::getline(file, temp);

    foreground = new std::string * [foreground_h];

    for (size_t i = 0; i < foreground_h; i++) {
        foreground[i] = new std::string[foreground_w];
    }

    for (size_t i = 0; i < foreground_h; i++)
    {
        for (size_t j = 0; j < foreground_w; j++)
        {
            std::getline(file, temp, ';');
            foreground[i][j] = temp;
            std::cout << foreground[i][j] << " - ";
        }
        std::getline(file, temp);
        std::cout << std::endl;
    }

    //OBJECTS

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp != "OBJECTS") {
        std::cout << "Error 11 : Objects not found" << std::endl;
        return 11;
    }

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    objects_w = std::stoi(temp);

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    objects_h = std::stoi(temp);

    std::getline(file, temp);

    objects = new std::string * [objects_h];

    for (size_t i = 0; i < objects_h; i++) {
        objects[i] = new std::string[objects_w];
    }

    for (size_t i = 0; i < objects_h; i++)
    {
        for (size_t j = 0; j < objects_w; j++)
        {
            std::getline(file, temp, ';');

            objects[i][j] = temp;
            if (j == objects_w - 1) {
                std::cout << objects[i][j];
            }
            else {
                std::cout << objects[i][j] << " - ";
            }
        }
        std::getline(file, temp);
        std::cout << std::endl;
    }
}

void LoadTextures() {
    std::map<std::string, std::string>::iterator it;
    for (it = textures.begin(); it != textures.end(); it++)
    {
        std::cout << it->first << " : " << it->second << std::endl;
        const char* img_string = it->second.c_str();
        Image img = LoadImage(img_string);
        Texture2D levelTexture = LoadTextureFromImage(img);
        level_textures.insert(std::pair<std::string, Texture2D>(it->first.c_str(), levelTexture));
    }
}

bool Collision(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++)
    {
        for (size_t j = 0; j < objects_w; j++)
        {
            if (fg_positions[i][j].x == pos.x && fg_positions[i][j].y == pos.y && fg_positions[i][j].z == pos.z) {
                if (objects[i][j] != "0" && objects[i][j] != "E") { return true; }
            }
        }
    }
    return false;
}

bool Destructible(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++)
    {
        for (size_t j = 0; j < objects_w; j++)
        {
            if (fg_positions[i][j].x == pos.x && fg_positions[i][j].y == pos.y && fg_positions[i][j].z == pos.z) {
                if (objects[i][j] == "D" || objects[i][j] == "U") { return true; }
            }
        }
    }
    return false;
}

bool PowerUp(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++)
    {
        for (size_t j = 0; j < objects_w; j++)
        {
            if (fg_positions[i][j].x == pos.x && fg_positions[i][j].y == pos.y && fg_positions[i][j].z == pos.z) {
                if (objects[i][j] == "E") { return true; }
            }
        }
    }
    return false;
}

void Destroy(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++)
    {
        for (size_t j = 0; j < objects_w; j++)
        {
            if (fg_positions[i][j].x == pos.x && fg_positions[i][j].y == pos.y && fg_positions[i][j].z == pos.z) 
            {
                //Modificamos mapa
                foreground[i][j] = "0";

                if (objects[i][j] == "U") 
                {
                    objects[i][j] = "E";
                    Vector3 newPowerUp = { pos.x, 1, pos.z };
                    powerUps.push_back(newPowerUp);
                    return;
                }
                else if (objects[i][j] == "D") 
                {
                    objects[i][j] = "0";
                    return;
                }
            }
        }
    }
}

void PickPowerUp(Vector3 pos) {
    for (size_t i = 0; i < objects_h; i++)
    {
        for (size_t j = 0; j < objects_w; j++)
        {
            if (fg_positions[i][j].x == pos.x && fg_positions[i][j].y == pos.y && fg_positions[i][j].z == pos.z)
            {
                //Modificamos mapa
                if (objects[i][j] == "E") {
                    objects[i][j] = "0";
                }

                //Eliminamos power up del vector recorriendo el vector y viendo si coincide la posición
                if (powerUps.size() > 0) {
                    for (int i = 0; i < powerUps.size(); i++)
                    {
                        if (powerUps[i].x == pos.x && powerUps[i].z == pos.z) {
                            powerUps.erase(powerUps.begin() + i);
                            return;
                        }
                    }
                }
            }
        }
    }
}

int Draw()
{
    // Initialization
        //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, title.c_str());

    LoadTextures();

    

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    if (version >= 1.0) {
        camera.position = { 0.0f, cameraHeight, 2.0f };  // Camera position
    }
    else {
        camera.position = { 0.0f, 20.0f, 2.0f };  // Camera position
    }
    camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type

#pragma region MapInit




    float x_margin = background_w % 2 == 0 ? .5 : 0;
    float z_margin = background_h % 2 == 0 ? .5 : 0;
    float x_offset = background_w / 2;
    float z_offset = background_h / 2;

    Vector3 initialPos = { -x_offset + x_margin, 0, -z_offset + z_margin };
    
    x_margin = foreground_w % 2 == 0 ? .5 : 0;
    z_margin = foreground_h % 2 == 0 ? .5 : 0;
    x_offset = foreground_w / 2;
    z_offset = foreground_h / 2;

    Vector3 fg_initialPos = { -x_offset + x_margin, 1, -z_offset + z_margin };
    Vector3 tempPos = initialPos;

    //Vector3 fg_initialPos = { -6.5f, 1.0f, -6.5f };
    //Vector3 initialPos = { -5.5f, 0.0f, -5.5f };
    Vector3** positions = new Vector3 * [background_h];
    fg_positions = new Vector3 * [foreground_h];

    for (size_t i = 0; i < background_h; i++)
    {
        positions[i] = new Vector3[background_w];
        for (size_t j = 0; j < background_w; j++)
        {
            tempPos.x += 1.0f;
            positions[i][j] = tempPos;
        }
        tempPos.z += 1.0f;
        tempPos.x = initialPos.x;
    }

    tempPos = fg_initialPos;
    for (size_t i = 0; i < foreground_h; i++)
    {
        fg_positions[i] = new Vector3[foreground_w];
        for (size_t j = 0; j < foreground_w; j++)
        {
            tempPos.x += 1.0f;
            fg_positions[i][j] = tempPos;
        }
        tempPos.z += 1.0f;
        tempPos.x = fg_initialPos.x;
    }

    for (size_t i = 0; i < objects_h; i++)
    {
        for (size_t j = 0; j < objects_w; j++)
        {
            if (objects[i][j] == "1") {
                objects[i][j] = "0";
                Player* newPlayer = new Player;
                newPlayer->position = fg_positions[i][j];
                newPlayer->color = GREEN;
                newPlayer->maxBombs = 1;
                newPlayer->num = 1;
                newPlayer->dead = false;
                players.push_back(newPlayer);
                //std::cout << " PLAYER 1 POSITION: " << newPlayer->position.x << newPlayer->position.y << newPlayer->position.z << std::endl;
            }
            else if (objects[i][j] == "2") {
                objects[i][j] = "0";
                Player* newPlayer = new Player;
                newPlayer->position = fg_positions[i][j];
                newPlayer->color = RED;
                newPlayer->maxBombs = 1;
                newPlayer->num = 2;
                newPlayer->dead = false;
                players.push_back(newPlayer);
            }
        }
    }
#pragma endregion

    Music bgMusic;
    if (version >= 0.5) {
        InitAudioDevice();
        bgMusic = LoadMusicStream(bgMusic_string.c_str());
        bgMusic.looping = true;
        PlayMusicStream(bgMusic);
    }

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        // Update
        //----------------------------------------------------------------------------------
        float deltaTime = GetFrameTime();

        if (version >= 0.5) {
            UpdateMusicStream(bgMusic);
        }
        //Inserto bombas en el vector para luego dibujarlas si vector.size > 0
        if (IsKeyPressed(KEY_SPACE) && players[0]->maxBombs > player1Bombs.size()) {
            Bombs* newBomb = new Bombs;
            newBomb->pos_x = players[0]->position.x;
            newBomb->pos_z = players[0]->position.z;
            newBomb->timeToExplode = 1.5f;
            player1Bombs.push_back(newBomb);
        }

        if (IsKeyPressed(KEY_ENTER) && players[1]->maxBombs > player2Bombs.size()) {
            Bombs* newBomb = new Bombs;
            newBomb->pos_x = players[1]->position.x;
            newBomb->pos_z = players[1]->position.z;
            newBomb->timeToExplode = 1.5f;
            player2Bombs.push_back(newBomb);
        }

        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------

        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        //Render background
        for (size_t i = 0; i < background_h; i++)
        {
            for (size_t j = 0; j < background_w; j++)
            {
                if (background[i][j] == "C") {
                    std::string t = background[i][j];
                    DrawCubeTexture(level_textures[t], positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                }
                if (background[i][j] == "V") {
                    std::string t = background[i][j];
                    DrawCubeTexture(level_textures[t], positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                }
            }
        }

        //Render foreground
        for (size_t i = 0; i < foreground_h; i++)
        {
            for (size_t j = 0; j < foreground_w; j++)
            {
                if (foreground[i][j] == "L") {
                    std::string t = foreground[i][j];
                    DrawCubeTexture(level_textures[t], fg_positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                }
                else if (foreground[i][j] == "P") {
                    std::string t = foreground[i][j];
                    DrawCubeTexture(level_textures[t], fg_positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                }
                else if (foreground[i][j] == "T") {
                    std::string t = foreground[i][j];
                    DrawCubeTexture(level_textures[t], fg_positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                }
            }
        }

        //Players render and input
        for (int i = 0; i < players.size(); i++)
        {
            if (players[i]->num == 1) {


                if (IsKeyPressed(KEY_D) && !Collision({ players[i]->position.x + 1, players[i]->position.y, players[i]->position.z })) {

                    players[i]->position = { players[i]->position.x + 1, players[i]->position.y, players[i]->position.z };
                    if (PowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                         players[i]->maxBombs++;
                        PickPowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_A) && !Collision({ players[i]->position.x - 1, players[i]->position.y, players[i]->position.z })) {

                    players[i]->position = { players[i]->position.x - 1, players[i]->position.y, players[i]->position.z };
                    if (PowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        PickPowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_W) && !Collision({ players[i]->position.x, players[i]->position.y, players[i]->position.z - 1 })) {

                    players[i]->position = { players[i]->position.x, players[i]->position.y, players[i]->position.z - 1 };
                    if (PowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        PickPowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_S) && !Collision({ players[i]->position.x, players[i]->position.y, players[i]->position.z + 1 })) {

                    players[i]->position = { players[i]->position.x, players[i]->position.y, players[i]->position.z + 1 };
                    if (PowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        PickPowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }

                if (!players[i]->dead) {
                    DrawSphere(players[i]->position, 0.4f, players[i]->color);
                }
            }

            else if (players[i]->num == 2) {


                if (IsKeyPressed(KEY_RIGHT) && !Collision({ players[i]->position.x + 1, players[i]->position.y, players[i]->position.z })) {

                    players[i]->position = { players[i]->position.x + 1, players[i]->position.y, players[i]->position.z };
                    if (PowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        PickPowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_LEFT) && !Collision({ players[i]->position.x - 1, players[i]->position.y, players[i]->position.z })) {

                    players[i]->position = { players[i]->position.x - 1, players[i]->position.y, players[i]->position.z };
                    if (PowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        PickPowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_UP) && !Collision({ players[i]->position.x, players[i]->position.y, players[i]->position.z - 1 })) {

                    players[i]->position = { players[i]->position.x, players[i]->position.y, players[i]->position.z - 1 };
                    if (PowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        PickPowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                else if (IsKeyPressed(KEY_DOWN) && !Collision({ players[i]->position.x, players[i]->position.y, players[i]->position.z + 1 })) {

                    players[i]->position = { players[i]->position.x, players[i]->position.y, players[i]->position.z + 1 };
                    if (PowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z })) {
                        players[i]->maxBombs++;
                        PickPowerUp({ players[i]->position.x, players[i]->position.y, players[i]->position.z });
                    }
                }
                if (!players[i]->dead) {
                    DrawSphere(players[i]->position, 0.4f, players[i]->color);
                }

            }
        }


        //Bombs render and logic
        if (player1Bombs.size() > 0) {
            for (int i = 0; i < player1Bombs.size(); i++)
            {
                player1Bombs[i]->timeToExplode -= deltaTime;
                if (player1Bombs[i]->timeToExplode > 0) {
                    DrawSphere({ player1Bombs[i]->pos_x, 1.0f, player1Bombs[i]->pos_z }, 0.1 + 0.2f * abs(sin(player1Bombs[i]->timeToExplode * 2)), BLACK);
                }
                else if(player1Bombs[i]->timeToExplode < 0) { 
                    //Miramos las 4 direcciones si hay un destructible en objects
                    if (Destructible({ player1Bombs[i]->pos_x + 1, 1, player1Bombs[i]->pos_z })) {
                        Destroy({ player1Bombs[i]->pos_x + 1, 1, player1Bombs[i]->pos_z });
                    }
                    else if (Destructible({ player1Bombs[i]->pos_x - 1, 1, player1Bombs[i]->pos_z })) {
                        if (PowerUp({ player1Bombs[i]->pos_x - 1, 1, player1Bombs[i]->pos_z })) {
                            Vector3 newPowerUp = { player1Bombs[i]->pos_x - 1, 1, player1Bombs[i]->pos_z };
                            powerUps.push_back(newPowerUp);
                        }
                        Destroy({ player1Bombs[i]->pos_x - 1, 1, player1Bombs[i]->pos_z });
                    }
                    else if (Destructible({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z + 1})) {
                        if (PowerUp({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z + 1 })) {
                            Vector3 newPowerUp = { player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z + 1 };
                            powerUps.push_back(newPowerUp);
                        }
                        Destroy({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z + 1});
                    }
                    else if (Destructible({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z -1})) {
                        if (PowerUp({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z - 1 })) {
                            Vector3 newPowerUp = { player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z - 1 };
                            powerUps.push_back(newPowerUp);
                        }
                        Destroy({ player1Bombs[i]->pos_x, 1, player1Bombs[i]->pos_z -1});
                    }
                    //Miramos si hay players
                    if (player1Bombs[i]->pos_x == players[0]->position.x && player1Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x == players[1]->position.x && player1Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player1Bombs[i]->pos_x + 1 == players[0]->position.x && player1Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x + 1 == players[1]->position.x && player1Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player1Bombs[i]->pos_x - 1 == players[0]->position.x && player1Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x - 1 == players[1]->position.x && player1Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player1Bombs[i]->pos_x == players[0]->position.x && player1Bombs[i]->pos_z + 1 == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x == players[1]->position.x && player1Bombs[i]->pos_z + 1 == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player1Bombs[i]->pos_x == players[0]->position.x && player1Bombs[i]->pos_z - 1 == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player1Bombs[i]->pos_x == players[1]->position.x && player1Bombs[i]->pos_z - 1 == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    player1Bombs.erase(player1Bombs.begin() + i);
                }
            }
        }

        if (player2Bombs.size() > 0) {
            for (int i = 0; i < player2Bombs.size(); i++)
            {
                player2Bombs[i]->timeToExplode -= deltaTime;
                if (player2Bombs[i]->timeToExplode > 0) {
                    DrawSphere({ player2Bombs[i]->pos_x, 1.0f, player2Bombs[i]->pos_z }, 0.1 + 0.2f * abs(sin(player2Bombs[i]->timeToExplode * 2)), BLACK);
                }
                else if (player2Bombs[i]->timeToExplode < 0) {
                    //Miramos las 4 direcciones si hay un destructible en objects y si es power up o no
                    if (Destructible({ player2Bombs[i]->pos_x + 1, 1, player2Bombs[i]->pos_z })) {
                        if (PowerUp({ player2Bombs[i]->pos_x + 1, 1, player2Bombs[i]->pos_z })) {
                            Vector3 newPowerUp = { player2Bombs[i]->pos_x + 1, 1, player2Bombs[i]->pos_z };
                            powerUps.push_back(newPowerUp);
                        }
                        Destroy({ player2Bombs[i]->pos_x + 1, 1, player2Bombs[i]->pos_z });
                    }
                    else if (Destructible({ player2Bombs[i]->pos_x - 1, 1, player2Bombs[i]->pos_z })) {
                        if (PowerUp({ player2Bombs[i]->pos_x - 1, 1, player2Bombs[i]->pos_z })) {
                            Vector3 newPowerUp = { player2Bombs[i]->pos_x - 1, 1, player2Bombs[i]->pos_z };
                            powerUps.push_back(newPowerUp);
                        }
                        Destroy({ player2Bombs[i]->pos_x - 1, 1, player2Bombs[i]->pos_z });
                    }
                    else if (Destructible({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z + 1 })) {
                        if (PowerUp({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z + 1 })) {
                            Vector3 newPowerUp = { player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z + 1 };
                            powerUps.push_back(newPowerUp);
                        }
                        Destroy({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z + 1 });
                    }
                    else if (Destructible({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z - 1 })) {
                        if (PowerUp({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z - 1 })) {
                            Vector3 newPowerUp = { player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z - 1 };
                            powerUps.push_back(newPowerUp);
                        }
                        Destroy({ player2Bombs[i]->pos_x, 1, player2Bombs[i]->pos_z - 1 });
                    }
                    //Miramos si hay players
                    if (player2Bombs[i]->pos_x == players[0]->position.x && player2Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x == players[1]->position.x && player2Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player2Bombs[i]->pos_x + 1 == players[0]->position.x && player2Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x + 1 == players[1]->position.x && player2Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player2Bombs[i]->pos_x - 1 == players[0]->position.x && player2Bombs[i]->pos_z == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x - 1 == players[1]->position.x && player2Bombs[i]->pos_z == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player2Bombs[i]->pos_x == players[0]->position.x && player2Bombs[i]->pos_z + 1 == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x == players[1]->position.x && player2Bombs[i]->pos_z + 1 == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    if (player2Bombs[i]->pos_x == players[0]->position.x && player2Bombs[i]->pos_z - 1 == players[0]->position.z) {
                        players[0]->dead = true;
                    }
                    else if (player2Bombs[i]->pos_x == players[1]->position.x && player2Bombs[i]->pos_z - 1 == players[1]->position.z) {
                        players[1]->dead = true;
                    }

                    player2Bombs.erase(player2Bombs.begin() + i);
                }
            }
        }

        if (powerUps.size() > 0) 
        {
            for (int i = 0; i < powerUps.size(); i++)
            {
                DrawCube(powerUps[i], 0.4f, 0.4f, 0.4f, RED);
            }
        }


        //DrawCube(fg_initialPos, 1.0f, 1.0f, 1.0f, GREEN);f
        //DrawCube(initialPos, 1.0f, 1.0f, 1.0f, RED);
        //DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);
        //DrawGrid(10, 1.0f);
        //DrawText("Welcome to the third dimension!", 20, 40, 20, DARKGRAY);



        EndMode3D();

        if (!gameOver) {
            if (players[1]->dead) {
                player1Wins = true;
                gameOver = true;
            }
            else if (players[0]->dead) {
                player2Wins = true;
                gameOver = true;
            }
        }

        if (player1Wins) {
            DrawText("Player 1 wins!", screenWidth / 2, 40, 20, DARKGRAY);

        }
        if (player2Wins) {
            DrawText("Player 2 wins!", screenWidth / 2, 40, 20, DARKGRAY);
        }

        DrawFPS(10, 10);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}



int main(void) {
    //Init
    LoadMap();
    

    //Game loop
    Draw();
}