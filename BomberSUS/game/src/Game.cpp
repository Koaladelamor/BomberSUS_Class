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

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

float version = 0.0;
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
Vector3 player1Position;

int LoadMap() {
    std::string temp;
    std::fstream file;

    file.open("level.sus", std::ifstream::in);

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

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp != "TEXTURES") {
        std::cout << "Error 5 : Textures not found" << std::endl;
        return 5;
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

    //Image image = LoadImage("grass.png");
    //Texture2D cubicmap = LoadTextureFromImage(image);

    //BACKGROUND

    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp != "BACKGROUND") {
        std::cout << "Error 6 : Background not found" << std::endl;
        return 6;
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
    std::getline(file, temp);
    std::getline(file, temp, ';');
    std::cout << temp << std::endl;
    if (temp != "FOREGROUND") {
        std::cout << "Error 7 : Foreground not found" << std::endl;
        return 7;
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
        std::cout << "Error 8 : Objects not found" << std::endl;
        return 8;
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
            /*if (j == objects_w - 1) {
                std::cout << objects[i][j];
            }
            else {
                std::cout << objects[i][j] << " - ";
            }*/
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
            if (fg_positions[i][j].x == pos.x && fg_positions[i][j].y == pos.y, fg_positions[i][j].z == pos.z) {
                if (objects[i][j] == "X") { return true; }
            }
        }
    }
    return false;
}

int draw()
{
    // Initialization
        //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Mi tremendo juego");

    LoadTextures();

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = { 0.0f, 15.0f, 2.0f };  // Camera position
    camera.target = { 0.0f, 0.0f, 0.0f };      // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera mode type


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

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        for (size_t i = 0; i < background_h; i++)
        {
            for (size_t j = 0; j < background_w; j++)
            {
                if (background[i][j] == "C") {
                    std::string t = background[i][j];
                    DrawCubeTexture(level_textures[t], positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                    //DrawCubeWires(positions[i][j], 1.02f, 1.02f, 1.02f, DARKBROWN);
                }
                if (background[i][j] == "V") {
                    std::string t = background[i][j];
                    DrawCubeTexture(level_textures[t], positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                    //DrawCubeWires(positions[i][j], 1.02f, 1.02f, 1.02f, DARKBROWN);
                }
            }
        }

        for (size_t i = 0; i < foreground_h; i++)
        {
            for (size_t j = 0; j < foreground_w; j++)
            {
                if (foreground[i][j] == "L") {
                    std::string t = foreground[i][j];
                    DrawCubeTexture(level_textures[t], fg_positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                    //DrawCubeWires(fg_positions[i][j], 1.02f, 1.02f, 1.02f, DARKGRAY);
                }
                else if (foreground[i][j] == "P") {
                    std::string t = foreground[i][j];
                    DrawCubeTexture(level_textures[t], fg_positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                    //DrawCubeWires(fg_positions[i][j], 1.02f, 1.02f, 1.02f, GRAY);
                }
                else if (foreground[i][j] == "T") {
                    std::string t = foreground[i][j];
                    DrawCubeTexture(level_textures[t], fg_positions[i][j], 1.0f, 1.0f, 1.0f, WHITE);
                    //DrawCubeWires(fg_positions[i][j], 1.02f, 1.02f, 1.02f, BLACK);
                }
                //DrawCube(fg_positions[i][j], 1.0f, 1.0f, 1.0f, GREEN);
                //DrawCubeWires(fg_positions[i][j], 1.02f, 1.02f, 1.02f, DARKGREEN);
            }
        }

        for (size_t i = 0; i < objects_h; i++)
        {
            for (size_t j = 0; j < objects_w; j++)
            {
                if (objects[i][j] == "1") {
                    std::string t = foreground[i][j];
                    player1Position = fg_positions[i][j];
                    if (IsKeyPressed(KEY_D) && objects[i][j + 1] == "0") {
                        player1Position = fg_positions[i][j + 1];
                        objects[i][j] = "0";
                        objects[i][j + 1] = "1";
                    }
                    else if (IsKeyPressed(KEY_A) && objects[i][j-1] == "0") {
                        player1Position = fg_positions[i][j-1];
                        objects[i][j] = "0";
                        objects[i][j-1] = "1";
                    }
                    else if (IsKeyPressed(KEY_W) && objects[i-1][j] == "0") {
                        player1Position = fg_positions[i-1][j];
                        objects[i][j] = "0";
                        objects[i-1][j] = "1";
                    }
                    else if (IsKeyPressed(KEY_S) && objects[i + 1][j] == "0") {
                        player1Position = fg_positions[i + 1][j];
                        objects[i][j] = "0";
                        objects[i + 1][j] = "1";
                    }

                    DrawSphere(player1Position, 0.4f, BLUE);

                        /*Vector3 right = {player1Position.x - 1, player1Position.y, player1Position.z};
                        if(!Collision(right)){
                            player1Position = right;
                        }*/
                    
                }
                else if (objects[i][j] == "2") {
                    std::string t = foreground[i][j];
                    DrawSphere(fg_positions[i][j], 0.4f, RED);
                }
                else if (objects[i][j] == "3") {
                    std::string t = foreground[i][j];
                    DrawSphere(fg_positions[i][j], 0.4f, GREEN);
                }
                else if (objects[i][j] == "4") {
                    std::string t = foreground[i][j];
                    DrawSphere(fg_positions[i][j], 0.4f, YELLOW);
                }
            }
        }

        //DrawCube(fg_initialPos, 1.0f, 1.0f, 1.0f, GREEN);
        //DrawCube(initialPos, 1.0f, 1.0f, 1.0f, RED);
        //DrawCubeWires(cubePosition, 2.0f, 2.0f, 2.0f, MAROON);

        //DrawGrid(10, 1.0f);

        EndMode3D();

        //DrawText("Welcome to the third dimension!", 10, 40, 20, DARKGRAY);

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
    LoadMap();
    draw();
}