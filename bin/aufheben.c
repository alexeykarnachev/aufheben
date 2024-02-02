#include "raylib.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Aufheben");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 100, RED);
        EndDrawing();
    }
}
