#include "bouncing.h"

#include <stdlib.h>
#include <string.h>

Box box_init(int pos_x, int pos_y, int width, int height) {

    Box box = { .pos.x = pos_x,
                .pos.y = pos_y,
                .w = width,
                .h = height,
                .vel.x = 0,
                .vel.y = 0,
                .acc.x = 0,
                .acc.y = 0 };
    return box;

}

void box_update(Box *box) {
    box->vel = sum_veci2d(box->vel, box->acc);
    box->pos = sum_veci2d(box->pos, box->vel);

    Veci2d new_vel = veci2d_init(1, 1);

    // Collision between walls
    if (box->pos.x + box->w > GRID_WIDTH || box->pos.x <= 0) {
        new_vel = veci2d_init(-1, 1);
    }

    if (box->pos.y + box->h > GRID_HEIGHT || box->pos.y <= 0) {
        new_vel = veci2d_init(1, -1);
        
    }
    box->vel = mul_veci2d(box->vel, new_vel);

    // TODO: Implement collision between the boxes
    // TODO: Change color of the box when it collides

}

void box_set_acc(Box *box, Veci2d acc) {
    set_veci2d(&box->acc, acc);
}

void box_set_vel(Box *box, Veci2d vel) {
    set_veci2d(&box->vel, vel);
}

int grid_init(Grid *grid) {
    grid->boxes = malloc (GRID_HEIGHT * GRID_WIDTH * sizeof(Box));
    grid->step_count = 0;
    if (grid->boxes == NULL) {
        return 0;
    }

    memset(grid->boxes, 0, GRID_HEIGHT * GRID_WIDTH * sizeof(Box));

    return 1;
}

void grid_destroy(Grid grid) {
    free(grid.boxes);
}


int place_box_in_grid(Grid *grid, Box *box) {
    int idx = box->pos.x + box->pos.y * GRID_WIDTH;
    if (grid->boxes[idx] == NULL) {
        grid->boxes[idx] = box;
        return 1;
    }

    return 0;
}

void grid_step(Grid *grid) {
    int size = GRID_HEIGHT * GRID_WIDTH;
    for (int i = 0; i < size; ++i) {
        if (grid->boxes[i] != NULL) {
            box_update(grid->boxes[i]);
        }
    }

    grid->step_count++;
}

int grid_get_size(Grid g) {
    (void)g;
    return GRID_HEIGHT * GRID_WIDTH;   
}

int grid_get_width(Grid g) {
    (void)g;
    return GRID_WIDTH;
}
