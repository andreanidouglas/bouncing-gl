#pragma once

typedef struct _vecf2d {
    float x;
    float y;
} Vecf2d;

typedef struct _veci2d {
    int x;
    int y;
} Veci2d;


Veci2d sum_veci2d (Veci2d a, Veci2d b);
Veci2d mul_veci2d (Veci2d a, Veci2d b);
Veci2d muls_veci2d (Veci2d a, int s);
void set_veci2d(Veci2d *dst, Veci2d src);
Veci2d veci2d_init(int x, int y);