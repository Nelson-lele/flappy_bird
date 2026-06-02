#include <iostream>
#include <random>
#include <print>
#include <vector>
#include "raylib.h"



struct Sprite
{
    // contain sprites for game floor
    Texture2D m_texture;
    Rectangle m_obj_rect;
    float m_velocity;
};

struct Pipe {
    Texture2D m_texture;
    Vector2 m_position;
    float m_velocity;
};



void ApplyGravity(Sprite *sprite)
{
    float gravity = 980.0f;
    sprite->m_velocity += gravity * GetFrameTime();
    sprite->m_obj_rect.y += sprite->m_velocity * GetFrameTime();

    float screen_height = GetScreenHeight();
    float bottom = sprite->m_obj_rect.y + sprite->m_obj_rect.height;

    if (bottom >= screen_height)
    {
        sprite->m_obj_rect.y = screen_height - sprite->m_obj_rect.height;
        sprite->m_velocity = 0.f;
    }
}
void MoveBird(Sprite *sprite)
{
        // ENTER KEY for jumping
    if (IsKeyDown(KEY_ENTER))
    {
        sprite->m_obj_rect.y -= 980.f * GetFrameTime();
        if (sprite->m_obj_rect.y <= 0)
        {
            sprite->m_obj_rect.y = 0.f;
        }
    }
}

float CalculatePipeSpace(int start,int end)
{
    std::random_device rand;
    std::mt19937 gen(rand());
    std::uniform_int_distribution<> rd(start, end);

    return rd(gen);
}
Rectangle GetBirdHitBox(Sprite* bird)
{
    return Rectangle{bird->m_obj_rect.x,bird->m_obj_rect.y,(float)bird->m_texture.width,(float)bird->m_texture.height};
}
Rectangle GetPipeHitBox(Pipe* pipe)
{
    return Rectangle{pipe->m_position.x,pipe->m_position.y,(float)pipe->m_texture.width,(float)pipe->m_texture.height};
}


bool PipeCollision(Rectangle &bird,Rectangle &pipe1,Rectangle &pipe2)
{
    if(CheckCollisionRecs(bird,pipe1) || CheckCollisionRecs(bird,pipe2))
    {
        return true;
    }
    return false;
}

std::vector<Pipe> activePipes; 
float spawnTimer = 0.0f;
const float PIPE_SPEED = 120.0f; // Pixels per second

void UpdateAndDrawPipes(Texture2D topPipe, Texture2D bottomPipe, float screenWidth,Sprite bird,float scroll)
{
    float dt = GetFrameTime();
    spawnTimer += dt;
    float randomGap = CalculatePipeSpace(300, 400);
    bool collision = false;

    Pipe tP;
    Pipe bP;
    if (spawnTimer >= 2.5f)
    {
        float spawnX = screenWidth + 50.0f; 

        tP = { topPipe,    { spawnX, 0.0f },              PIPE_SPEED };
        bP = { bottomPipe, { spawnX, (float)randomGap },  PIPE_SPEED };

        activePipes.push_back(tP);
        activePipes.push_back(bP);


        spawnTimer = 0.0f;
    }

    Rectangle bird_rect = GetBirdHitBox(&bird);

    for (auto pipe = activePipes.begin(); pipe != activePipes.end(); )
    {
        pipe->m_position.x -= pipe->m_velocity * dt; 
        Rectangle pipe_rect1 = GetPipeHitBox(&activePipes.at(0));
        Rectangle pipe_rect2 = GetPipeHitBox(&activePipes.at(1));

        collision = PipeCollision(bird_rect, pipe_rect1, pipe_rect2);

        DrawTextureEx(pipe->m_texture, pipe->m_position, 0.0f, 1.0f, RAYWHITE);

        if (pipe->m_position.x + pipe->m_texture.width < 0)
        {
            pipe = activePipes.erase(pipe);
        }
        else
        {
            ++pipe;
        }
        
        if(collision)
        {
            activePipes.at(0).m_velocity = 0;
            activePipes.at(1).m_velocity = 0;
            dt = 0;
            scroll += 2.0f;
            if(spawnTimer >= 0.5f)
                CloseWindow();
        }
    }
}

int main()
{

    InitWindow(1000, 500, "raylib [core] example - basic window");
    SetTargetFPS(60);

    Image pipe_image = LoadImage("/home/nelson/MyGame/assets/pipe2.png");
    Image pipe_one_img = LoadImage("/home/nelson/MyGame/assets/pipe.png");
    Texture2D floor_sprite = LoadTexture("/home/nelson/MyGame/assets/flap.png");

    Image image = LoadImage("/home/nelson/MyGame/assets/bird_sprite.png");
    ImageResize(&image, 32, 32);
    ImageResize(&pipe_image, 32, 256);
    ImageResize(&pipe_one_img, 32, 256);

    Texture2D bird_sprite = LoadTextureFromImage(image);

    float screen_height = GetScreenHeight();
    float screen_width = GetScreenWidth();
    float scrolling = 0;

    Texture2D pipe_one = LoadTextureFromImage(pipe_one_img);
    Texture2D pipe = LoadTextureFromImage(pipe_image);


    Sprite floor = Sprite{
        .m_texture = floor_sprite,
        .m_obj_rect = Rectangle{
            .x = 0,
            .y = 0,
            .width = screen_width,
            .height = screen_height}};

    Sprite bird = Sprite{
        .m_texture = bird_sprite,
        .m_obj_rect = Rectangle{
            .x = 100,
            .y = 100,
            .width = 32,
            .height = 32}};

    Rectangle rectangle_obj = Rectangle{
        .x = 0,
        .y = 0,
        .width = screen_width,
        .height = screen_height};

    Rectangle bird_rect = Rectangle{
        .x = 0,
        .y = 0,
        .width = 32,
        .height = 32};

    while (!WindowShouldClose())
    {

        // update screen
        ApplyGravity(&bird);
        MoveBird(&bird);
        

        scrolling -= 2.f;

        if (scrolling <= -floor.m_texture.width / 2)
        {
            scrolling = 0;
        }
        BeginDrawing();
        ClearBackground(SKYBLUE);
        Vector2 origin = {0.0f, 0.0f};
        float rotation = 0;
        // Background drawing
        DrawTextureEx(
            floor.m_texture,
            {scrolling, 0},
            rotation,
            1.f,
            RAYWHITE);
        DrawTextureEx(
            floor.m_texture,
            {floor.m_texture.width + scrolling},
            rotation,
            1.0f,
            RAYWHITE);
        // Bird sprite drawing
        DrawTexturePro(
            bird.m_texture,
            bird_rect,
            bird.m_obj_rect,
            origin,
            rotation,
            RAYWHITE);
        UpdateAndDrawPipes(pipe, pipe_one, screen_width, bird,scrolling);
        EndDrawing();
    }
    UnloadTexture(pipe_one);
    UnloadTexture(pipe);
    UnloadTexture(bird_sprite);
    UnloadTexture(floor_sprite);

    CloseWindow();
    return 0;
}