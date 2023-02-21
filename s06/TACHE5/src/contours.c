#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "types_macros.h"
#include "image.h"
#include "geometrie2d.h"

// ====<<+---------------------------+>>====
// ====<<| LinkedList Implementation |>>====
// ====<<+---------------------------+>>====

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

PointList* new_PointList(RenderStyle style) {
	PointList* rv = calloc(1, sizeof(PointList));
	rv->style = style;
	return rv;
}

void append_coordinates(PointList* list, double x, double y) {
	assert(list);
	list->len++;
	if (!list->head) {
		list->head = calloc(1, sizeof(ListNode));
		list->head->pos = set_point(x, y);
		list->tail = list->head;
	} else {
		list->tail->next = calloc(1, sizeof(ListNode));
		list->tail->next->pos = set_point(x, y);
		list->tail = list->tail->next;
	}
}

static void delete_nodes(ListNode* node) {
	if (!node) return;
	ListNode* next_node = node->next;
	free(node);
	delete_nodes(next_node);
}

void delete_list(PointList* list) {
	delete_nodes(list->head);
	free(list);
}

static void serialise_nodes(FILE* output_stream, double hauteur_image,
                            RenderStyle style, ListNode* node) {
	if (!node) {
		switch (style) {
			case Fill  : fprintf(output_stream, "fill\n");   return;
			case Stroke:
				fprintf(output_stream, "0.2 setlinewidth\nstroke\n");
				return;
		}
	}

	fprintf(output_stream, "%f %f lineto ",
	        node->pos.x,
	        hauteur_image - node->pos.y);
	serialise_nodes(output_stream, hauteur_image, style, node->next);
}

// TODO: be able to serialise multiple shapes on one drawing.
void serialise_list(FILE* output_stream, const PointList* list,
                    double hauteur_image, double largeur_image) {
	// `%%` -> escape `%`
	fprintf(output_stream, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	fprintf(output_stream, "%%%%BoundingBox: 0.0 0.0 %f %f\n",
	        largeur_image, hauteur_image);

	fprintf(output_stream, "%f %f moveto ",
	        list->head->pos.x,
	        hauteur_image - list->head->pos.y);
	serialise_nodes(output_stream, hauteur_image, list->style, list->head->next);

	fprintf(output_stream, "showpage\n");
}

// ====<<+----------------------+>>====
// ====<<| Robot Implementation |>>====
// ====<<+----------------------+>>====

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

void step_robot(Robot* robot, const Image* image) {
	// Get pixels in front of the robot
	PixelPos space_front_left_per_dir[] = {
		[Nord]  = { robot->pos.x    , robot->pos.y },
		[Est]   = { robot->pos.x + 1, robot->pos.y },
		[Sud]   = { robot->pos.x + 1, robot->pos.y + 1 },
		[Ouest] = { robot->pos.x    , robot->pos.y + 1 }
	};
	PixelPos forward_left  = space_front_left_per_dir[robot->direction];
	PixelPos forward_right = space_front_left_per_dir[ROTATE_RIGHT(robot->direction)];

	// Determine next orientation :
	if (get_pixel_image(*image, forward_left.x, forward_left.y)) {
		robot->direction = ROTATE_LEFT(robot->direction);
	} else if (!get_pixel_image(*image, forward_right.x, forward_right.y)) {
		robot->direction = ROTATE_RIGHT(robot->direction);
	}

	// Step forward
	switch (robot->direction) {
		case Nord:  robot->pos.y--; break;
		case Sud:   robot->pos.y++; break;
		case Est:   robot->pos.x++; break;
		case Ouest: robot->pos.x--; break;
	}
}

// Gives ownership of the return value to caller.
PointList* get_contour(const Image* image, Image* mask, UINT depart_x, UINT depart_y,
                       RenderStyle style) {
	Robot robot = {
		.pos = {  // Specify fields individually to properlly cast them.
			.x = depart_x - 1,
			.y = depart_y - 1,
		},
		.direction = Est,
	};

	PointList* rv = new_PointList(style);
	append_coordinates(rv, robot.pos.x, robot.pos.y);
	do {
		step_robot(&robot, image);
		append_coordinates(rv, robot.pos.x, robot.pos.y);
		if (robot.direction == Est) {
			set_pixel_image(*mask, robot.pos.x, robot.pos.y + 1, BLANC);
		}
	} while (robot.pos.x != depart_x - 1 || robot.pos.y != depart_y - 1);

	return rv;
}

// ====<<+-----------------------------+>>====
// ====<<| Get every contours in image |>>====
// ====<<+-----------------------------+>>====

typedef struct ContourListNode_ {
	PointList* contour;
	struct ContourListNode_* next;
} ContourListNode;

typedef struct {
	UINT len;
	ContourListNode* head;
	ContourListNode* tail;
} ContourList;

// Gives ownership of the contour to the list
void append_contour(ContourList* list, PointList* contour) {
	assert(list);
	list->len++;
	if (!list->head) {
		list->head = calloc(1, sizeof(ContourListNode));
		list->head->contour = contour;
		list->tail = list->head;
	} else {
		list->tail->next = calloc(1, sizeof(ContourListNode));
		list->tail->next->contour = contour;
		list->tail = list->tail->next;
	}
}

Image get_mask(const Image* image) {
	Image rv = creer_image(image->largeur, image->hauteur);
	for (int y = 1; y <= image->hauteur; y++) {
		for (int x = 1; x <= image->largeur; x++) {
			Pixel new_pix = get_pixel_image(*image, x, y) &&
					!get_pixel_image(*image, x, y - 1);
			set_pixel_image(rv, x, y, new_pix);
		}
	}

	return rv;
}

ContourList get_all_contours_image(const Image* image, RenderStyle style) {
	ContourList rv = {};
	Image mask = get_mask(image);

	for (int y = 1; y <= mask.hauteur; y++) {
		for (int x = 1; x <= mask.largeur; x++) {
			if (get_pixel_image(mask, x, y)) {
				append_contour(&rv, get_contour(image, &mask, x, y, style));
			}
		}
	}

	supprimer_image(&mask);
	return rv;
}

void serialise_contour_list(FILE* output_stream, const ContourList* list,
                    double hauteur_image, double largeur_image) {
	// `%%` -> escape `%`
	fprintf(output_stream, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	fprintf(output_stream, "%%%%BoundingBox: 0.0 0.0 %f %f\n",
	        largeur_image, hauteur_image);

	ContourListNode* current_node = list->head;

	while (current_node) {
		fprintf(output_stream, "%f %f moveto ",
				current_node->contour->head->pos.x,
				hauteur_image - current_node->contour->head->pos.y);
		serialise_nodes(output_stream, hauteur_image, current_node->contour->style, current_node->contour->head->next);
		current_node = current_node->next;
	}

	fprintf(output_stream, "showpage\n");
}


// ====<<+---------------------+>>====
// ====<<| Evaluate everything |>>====
// ====<<+---------------------+>>====

void show_help() {
	printf("Usage: ./test_contours <input_file> [style] <out_file>\n\n"
		"style :\n"
		"· stroke: (default) créé juste le contour de l’image.\n"
		"· fill: remplie tout à l’intérieur de l’image.\n");
}

// J’avais absolument aucune idée que c’était possible, c’est dégueulasse,
// mais c’est marrant donc je le laisse.
struct args {
	char* in_file_name;
	FILE* out_file;
	RenderStyle style;
} arg_parse(int argc, char** argv) {
	if (argc > 4) {
		printf("Arguments invalides, lancez le programme avec `-h` pour plus d’infos\n");
		exit(1);
	}

	if (argc == 1 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		show_help();
		exit(0);
	}

	if (argc == 2) {
		printf("Erreur: nom de fichier de sortie non précisé\n");
		exit(1);
	}

	struct args rv = { 
		.in_file_name = argv[1],
		.style = Stroke,
	};

	if (argc == 3) {
		rv.out_file = fopen(argv[2], "w");
	}

	if (argc == 4) {
		if (strcmp(argv[2], "fill") == 0) {
			rv.style = Fill;
		} else if (strcmp(argv[2], "stroke") != 0) {
			printf("Attention: nom de style inconnu, utilisera le défaut: stroke\n");
		}

		rv.out_file = fopen(argv[3], "w");
	}

	return rv;
}

int main (int argc, char** argv) {
	struct args parametters = arg_parse(argc, argv);

	Image image = lire_fichier_image(parametters.in_file_name);
	ContourList contours = get_all_contours_image(&image, parametters.style);

	serialise_contour_list(parametters.out_file, &contours, image.hauteur, image.largeur);

	/* delete_list(points); */

	supprimer_image(&image);
	return 0;
}
