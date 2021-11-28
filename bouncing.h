#pragma once

#include "la.h"


#define GRID_WIDTH 800
#define GRID_HEIGHT 600

typedef struct _box  {
    Veci2d pos;
    Veci2d vel;
    Veci2d acc;
    int w;
    int h;

} Box;

Box box_init(int pos_x, int pos_y, int width, int height);
void box_update(Box *box);
void box_set_acc(Box *box, Veci2d acc);
void box_set_vel(Box *box, Veci2d vel);

typedef struct _grid {
    Box **boxes;
    int step_count;
} Grid;


int grid_init(Grid *grid);
void grid_destroy(Grid grid);
int place_box_in_grid(Grid *grid, Box *box);
void grid_step(Grid *grid);
int grid_get_size(Grid g);
int grid_get_width(Grid g);