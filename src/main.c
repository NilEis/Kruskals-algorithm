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
    node_t *a;
    node_t *b;
    struct entry *next;
} entry_t;

void main(int argc, char **argv)
{
    uint32_t num_edges_in_tree = 0;
    uint32_t num_nodes = 150;
    if (argc == 2)
    {
        sscanf(argv[1], "%" SCNu32, &num_nodes);
    }
    srand((unsigned)time(NULL));
    node_t *nodes = (node_t *)calloc(num_nodes, sizeof(node_t));
    for (int i = 0; i < num_nodes; i++)
    {

        nodes[i].x = 10 + (rand() % (WIDTH - 20));
        nodes[i].y = 10 + (rand() % (HEIGHT - 20));
        nodes[i].s = uf_make_set(&nodes[i]);
    }
    entry_t *s = NULL;
    for (int i = 0; i < num_nodes; i++)
    {
        printf("%d/%d\n", i, num_nodes);
        for (int j = i + 1; j < num_nodes; j++)
        {
            entry_t *m = (entry_t *)malloc(sizeof(entry_t));
            m->a = &nodes[i];
            m->b = &nodes[j];
            int32_t x = (int32_t)nodes[i].x - (int32_t)nodes[j].x;
            int32_t y = (int32_t)nodes[i].y - (int32_t)nodes[j].y;
            m->d = (x * x + y * y);
            m->next = NULL;
            if (s == NULL)
            {
                LOG("m ist NULL %d\n", m->d);
                s = m;
            }
            else if (s->d >= m->d)
            {
                m->next = s;
                s = m;
                LOG("m ist kleiner als der start: %d -> %d\n", m->d, s->d);
            }
            else
            {
                entry_t *tmp = s;
                while (tmp->next != NULL && tmp->next->d <= m->d)
                {
                    LOG("s ist kleiner als m: %d -> %d\n", tmp->d, m->d);
                    tmp = tmp->next;
                }
                LOG("m eingefuegt hinter tmp: %d -> %d\n", tmp->d, m->d);
                m->next = tmp->next;
                tmp->next = m;
            }
        }
    }
    InitWindow(WIDTH, HEIGHT, "Kruskal's algorithm");
    //ToggleFullscreen();
    target = LoadRenderTexture(WIDTH, HEIGHT);
    target_2 = LoadRenderTexture(WIDTH, HEIGHT);
    BeginTextureMode(target);
    ClearBackground((Color){0, 0, 0, 0});
    for (int i = 0; i < num_nodes; i++)
    {
        DrawCircle(nodes[i].x, nodes[i].y, 4, RAYWHITE);
    }
    EndTextureMode();
    BeginTextureMode(target_2);
    {
        ClearBackground((Color){0, 0, 0, 0});
        entry_t *tmp = s;
        while (tmp != NULL)
        {
            DrawLineEx((Vector2){tmp->a->x, tmp->a->y}, (Vector2){tmp->b->x, tmp->b->y}, 1, (Color){245, 245, 245, 125});
            tmp = tmp->next;
        }
    }
    EndTextureMode();
    entry_t *st = s;
    while (!WindowShouldClose())
    {
        if(IsMouseButtonDown(0))
        {
            ToggleFullscreen();
        }
        if (s == NULL || num_edges_in_tree == num_nodes - 1)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0.0, WHITE);
            EndDrawing();
            //s = st;
        }
        else
        {
            union_find_t *a = uf_find(s->a->s);
            union_find_t *b = uf_find(s->b->s);
            if (a != b)
            {
                BeginTextureMode(target);
                union_find_t *p = uf_merge(s->a->s, s->b->s);
                s->a->s = p;
                s->b->s = p;
                DrawLineEx((Vector2){s->a->x, s->a->y}, (Vector2){s->b->x, s->b->y}, 2, BLUE);
                EndTextureMode();
                num_edges_in_tree++;
            }
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(target_2.texture, (Rectangle){0, 0, (float)target_2.texture.width, (float)-target_2.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0.0, WHITE);
            DrawTexturePro(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0.0, WHITE);
            DrawLineEx((Vector2){s->a->x, s->a->y}, (Vector2){s->b->x, s->b->y}, 2, RED);
            //DrawCircle(s->a->x, s->a->y, s->d, (Color){245, 245, 245, 50});
            {
                char buffer[8] = {'\0'};
                sprintf((char*)&buffer, "%.2f%%", ((float)num_edges_in_tree / (float)num_nodes) * 100.0);
                DrawText(buffer, 10, 10, 20, RAYWHITE);
            }
            EndDrawing();
            WaitTime(0.25);
            LOG("%d\n", s->d);
            s = s->next;
        }
    }
    CloseWindow();
    for (int i = 0; i < num_nodes; i++)
    {
        free(nodes[i].s);
    }
    free(nodes);
    return;
}