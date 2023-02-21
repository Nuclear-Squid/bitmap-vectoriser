#ifndef _CONTOURS_H_
#define _CONTOURS_H_

#include "geometrie2d.h"
#include "types_macros.h"
#include "image.h"

typedef enum {
	Stroke, Fill,
} RenderStyle;

#define LINEWIDTH 0.2

typedef struct ListNode_ {
	Point pos;
	struct ListNode_* next;
} ListNode;

typedef struct {
	UINT len;
	RenderStyle style;
	ListNode* head;
	ListNode* tail;
} PointList;

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
	UINT x, y;
} PixelPos;

typedef struct ContourListNode_ {
	PointList* contour;
	struct ContourListNode_* next;
} ContourListNode;

typedef struct {
	UINT len;
	ContourListNode* head;
	ContourListNode* tail;
} ContourList;

ContourList* get_all_contours_image(const Image* image, RenderStyle style);

void serialise_contour_list(FILE* output_stream, const ContourList* list,
                    double hauteur_image, double largeur_image);

void delete_contour_list(ContourList* list);


#endif
