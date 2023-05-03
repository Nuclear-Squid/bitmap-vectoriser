// vim:ft=c
#ifndef _CONTOURS_H_
#define _CONTOURS_H_

#include "geometrie2d.h"
#include "linked_lists.h"
#include "types_macros.h"
#include "image.h"

typedef enum {
	Stroke, Fill,
} RenderStyle;

#define LINEWIDTH 0.2

typedef enum {
	Nord, Est, Sud, Ouest
} Orientation;

#define ROTATE_LEFT(direction)  (((direction) + 3) % 4)
#define ROTATE_RIGHT(direction) (((direction) + 1) % 4)

typedef struct {
	Point pos;
	Orientation direction;
} Robot;

typedef struct {
	u32 x, y;
} PixelPos;

LL_Contours* get_all_contours_image(const Image*);

#endif
