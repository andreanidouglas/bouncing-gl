#include "la.h"

Veci2d sum_veci2d (Veci2d a, Veci2d b) {
    Veci2d v;

    v.x = a.x + b.x;
    v.y = a.y + b.y;

    return v;

}

Veci2d veci2d_init(int x, int y) {
    Veci2d res = {.x = x, .y = y};
    return res;
}

void set_veci2d(Veci2d *dst, Veci2d src) {
    dst->x = src.x;
    dst->y = src.y;
}

Veci2d muls_veci2d (Veci2d a, int s) {
    Veci2d v;

    v.x = a.x * s;
    v.y = a.y * s;

    return v;
}

Veci2d mul_veci2d (Veci2d a, Veci2d b) {
    Veci2d v;

    v.x = a.x * b.x;
    v.y = a.y * b.y;

    return v;

}