#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include "raylib.h"
#include "union_find.h"
#include <unistd.h>
#include "config.h"

#ifndef NDEBUG
#define LOG(x, ...) printf(x, __VA_ARGS__)
#else
#define LOG(x, ...) ;
#endif

#define WIDTH 1280
#define HEIGHT 720

RenderTexture2D target;
RenderTexture2D target_2;

typedef struct
{
    uint16_t x;
    uint16_t y;
    union_find_t *s;
} node_t;

typedef struct entry
{
    uint32_t d;
    int a;
    int b;
} entry_t;

// ret < 0 if a < b
int compare_edges(const void *a, const void *b)
{
    const entry_t *e1 = a;
    const entry_t *e2 = b;
    return e1->d - e2->d;
}

int main(int argc, char **argv)
{
    size_t num_edges_in_tree = 0;
    size_t num_nodes = 150;
    if (argc == 2)
    {
        sscanf(argv[1], "%lld", &num_nodes);
    }
    const size_t num_edges = (num_nodes * (num_nodes - 1)) / 2;
    srand((unsigned)time(NULL));
    node_t *V = calloc(num_nodes, sizeof(node_t));
    if (V == NULL)
    {
        printf("Could not allocate %lld bytes.\n", num_nodes * sizeof(node_t));
        return -1;
    }
    for (int i = 0; i < num_nodes; i++)
    {

        V[i].x = 10 + (rand() % (WIDTH - 20));
        V[i].y = 10 + (rand() % (HEIGHT - 20));
        V[i].s = uf_make_set(&V[i]);
    }
    entry_t *E = calloc(num_edges, sizeof(entry_t));
    if (E == NULL)
    {
        printf("Could not allocate %lld bytes.\n", num_edges * sizeof(entry_t));
        free(V);
        return -1;
    }
    {
        size_t abc = 0;
        for (size_t i = 0; i < num_nodes; i++)
        {
            for (size_t j = i + 1; j < num_nodes; j++)
            {
                E[abc].a = i;
                E[abc].b = j;
                int32_t x = (int32_t)V[i].x - (int32_t)V[j].x;
                int32_t y = (int32_t)V[i].y - (int32_t)V[j].y;
                E[abc].d = (x * x + y * y);
                abc++;
            }
            printf("%lld/%lld\n", i, num_nodes);
        }
    }
    qsort(E, num_edges, sizeof(entry_t), compare_edges);
    InitWindow(WIDTH, HEIGHT, "Kruskal's algorithm");
    // ToggleFullscreen();
    target = LoadRenderTexture(WIDTH, HEIGHT);
    target_2 = LoadRenderTexture(WIDTH, HEIGHT);
    BeginTextureMode(target);
    ClearBackground((Color){0, 0, 0, 0});
    for (size_t i = 0; i < num_nodes; i++)
    {
        DrawCircle(V[i].x, V[i].y, 1.5, RAYWHITE);
    }
    EndTextureMode();
    BeginTextureMode(target_2);
    {
        ClearBackground((Color){0, 0, 0, 0});
        for (size_t i = 0; i < num_edges; i++)
        {
            DrawLineEx((Vector2){V[E[i].a].x, V[E[i].a].y}, (Vector2){V[E[i].b].x, V[E[i].b].y}, 1, (Color){245, 245, 245, 125});
        }
    }
    EndTextureMode();
    for (size_t i = 0; num_edges_in_tree != num_nodes - 1 && i < num_edges; i++)
    {
        if (IsMouseButtonDown(0))
        {
            ToggleFullscreen();
        }
        union_find_t *a = uf_find(V[E[i].a].s);
        union_find_t *b = uf_find(V[E[i].b].s);
        if (a != b)
        {
            BeginTextureMode(target);
            union_find_t *p = uf_merge(V[E[i].a].s, V[E[i].b].s);
            V[E[i].a].s = p;
            V[E[i].b].s = p;
            DrawLineEx((Vector2){V[E[i].a].x, V[E[i].a].y}, (Vector2){V[E[i].b].x, V[E[i].b].y}, 2, BLUE);
            EndTextureMode();
            num_edges_in_tree++;
        }
        else
        {
            BeginTextureMode(target_2);
            DrawLineEx((Vector2){V[E[i].a].x, V[E[i].a].y}, (Vector2){V[E[i].b].x, V[E[i].b].y}, 2, BLACK);
            EndTextureMode();
        }
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target_2.texture, (Rectangle){0, 0, (float)target_2.texture.width, (float)-target_2.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0.0, WHITE);
        DrawTexturePro(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0.0, WHITE);
        DrawLineEx((Vector2){V[E[i].a].x, V[E[i].a].y}, (Vector2){V[E[i].b].x, V[E[i].b].y}, 2, RED);
        // DrawCircle(s->a->x, s->a->y, s->d, (Color){245, 245, 245, 50});
        {
            char buffer[8] = {'\0'};
            sprintf((char *)&buffer, "%.2f%%", ((float)num_edges_in_tree / (float)num_nodes) * 100.0);
            DrawText(buffer, 10, 10, 20, RAYWHITE);
        }
        EndDrawing();
        // WaitTime(0.25);
        LOG("%lld/%lld\n", i, num_nodes - 1);
        if (WindowShouldClose())
        {
            goto CLEANUP;
        }
    }
    while (!WindowShouldClose())
    {
        if (IsMouseButtonDown(0))
        {
            ToggleFullscreen();
        }
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0.0, WHITE);
        EndDrawing();
    }
CLEANUP:
    CloseWindow();
    for (size_t i = 0; i < num_nodes; i++)
    {
        free(V[i].s);
    }
    free(V);
    free(E);
    return 0;
}