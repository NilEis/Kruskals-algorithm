#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include "raylib.h"
#include "union_find.h"
#include <unistd.h>

#define WIDTH 1200
#define HEIGHT 800

RenderTexture2D target;

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
    uint32_t num_nodes = 15;
    if (argc == 2)
    {
        sscanf(argv[1], "%" SCNu32, &num_nodes);
    }
    InitWindow(WIDTH, HEIGHT, "Kruskal's algorithm");
    target = LoadRenderTexture(WIDTH, HEIGHT);
    srand((unsigned)time(NULL));
    node_t *nodes = (node_t *)calloc(num_nodes, sizeof(node_t));
    BeginTextureMode(target);
    for (int i = 0; i < num_nodes; i++)
    {

        nodes[i].x = 10 + (rand() % (WIDTH - 20));
        nodes[i].y = 10 + (rand() % (HEIGHT - 20));
        nodes[i].s = uf_make_set(&nodes[i]);
        DrawCircle(nodes[i].x, nodes[i].y, 5, RAYWHITE);
    }
    EndTextureMode();
    entry_t *s = NULL;
    for (int i = 0; i < num_nodes; i++)
    {
        for (int j = i + 1; j < num_nodes; j++)
        {
            entry_t *m = (entry_t *)malloc(sizeof(entry_t));
            m->a = &nodes[i];
            m->b = &nodes[j];
            int32_t x = (int32_t)nodes[i].x - (int32_t)nodes[j].x;
            int32_t y = (int32_t)nodes[i].y - (int32_t)nodes[j].y;
            m->d = x * x + y * y;
            m->next = NULL;
            if (s == NULL)
            {
                s = m;
            }
            else if (s->d > m->d)
            {
                m->next = s;
                s = m;
            }
            else
            {
                entry_t *tmp = s;
                while (tmp->d < m->d)
                {
                    if (tmp->next == NULL)
                    {
                        break;
                    }
                    tmp = tmp->next;
                }
                m->next = tmp->next;
                tmp->next = m;
            }
        }
    }
    entry_t *st = s;
    while (!WindowShouldClose())
    {
        if (s == NULL)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(target.texture, (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height}, (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, (Vector2){0, 0}, 0.0, WHITE);
            EndDrawing();
            //s = st;
        }
        else
        {
            printf("%d\n", s->d);
            union_find_t *a = uf_find(s->a->s);
            union_find_t *b = uf_find(s->b->s);
            if (a != b)
            {
                BeginTextureMode(target);
                union_find_t *p = uf_merge(s->a->s, s->b->s);
                s->a->s = p;
                s->b->s = p;
                DrawLineEx((Vector2){s->a->x, s->a->y}, (Vector2){s->b->x, s->b->y}, 5, BLUE);
                EndTextureMode();
            }
            s = s->next;
        }
    }
    for (int i = 0; i < num_nodes; i++)
    {
        free(nodes->s);
    }
    free(nodes);
    return;
}