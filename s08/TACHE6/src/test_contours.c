#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "contours.h"
#include "image.h"
#include "simplifications.h"

static void write_contour_data(FILE* out_file, const PointList* list) {
	ListNode* current_node = list->head;

	fprintf(out_file, "%u\n", list->len - 1);
	while (current_node) {
		fprintf(out_file, " %f %f\n", current_node->pos.x, current_node->pos.y);
		current_node = current_node->next;
	}
}

static u_int32_t sum_segements(const ContourList* list) {
	u_int32_t rv = 0;

	ContourListNode* current_node = list->head;
	while (current_node) {
		rv += current_node->contour->len - 1;
		current_node = current_node->next;
	}

	return rv;
}

static void write_all_contour_data(FILE* out_file, const ContourList* list) {
	ContourListNode* current_node = list->head;

	fprintf(out_file, "%u\n", list->len);
	while (current_node) {
		fprintf(out_file, "\n");
		write_contour_data(out_file, current_node->contour);
		current_node = current_node->next;
	}

	fprintf(out_file, "\ntotal segments: %u\n", sum_segements(list));
}

void show_help() {
	printf("Usage: ./test_contours -i <input_file> [<flag> <value>]\n\n"
		"flags: \n"
		"====== \n"
		"\n"
		"-o <out_file>\n"
		"\n"
		"-s <style> :\n"
		" · stroke: (default) créé juste le contour de l’image.\n"
		" · fill: remplie tout à l’intérieur de l’image.\n"
		"\n"
		"-m <methode-simplification>\n"
		" · segments\n"
		" · bezier2\n"
		" · bezier3\n"
		"\n"
		"-d <distance-seuil>\n"
	);
}

typedef enum {
	NoSimplification,
	Segments,
	BezierOrder2,
	BezierOrder3,
} SimplificationMethod;

typedef struct {
	char* in_file_name;
	FILE* out_file;
	RenderStyle style;
	SimplificationMethod simplification;
	double simplification_factor;
} Args;

// Writes a char, then set the cursor forward
void write_and_set_cursor(char** destination, const char* source) {
	while (*source) {
		**destination = *source;
		source++;
		(*destination)++;
	}
}

// Gives ownership of returned pointer, cannot fail
void genere_fichier_sortie_defaut(Args* args) {
	const size_t nb_components = 3; 
	const size_t len_components = 8; 
	char components[nb_components][len_components] = {
		"",  // style
		"",  // simp
		"",  // factor
	};

	{   // Set `components` variable
		switch (args->style) {
			case Fill:   strcpy(components[0], "fill");   break;
			case Stroke: strcpy(components[0], "stroke"); break;
		}

		switch (args->simplification) {
			case NoSimplification: strcpy(components[1], "noSimp");  break;
			case Segments:         strcpy(components[1], "segment"); break;
			case BezierOrder2:     strcpy(components[1], "bezier2"); break;
			case BezierOrder3:     strcpy(components[1], "bezier3"); break;
		}

		double s = args->simplification_factor;  // ’cause I’m lazy
		u_int8_t offset = log10(s) + 2;
		sprintf(components[2], "%d_", (int) args->simplification_factor);
		sprintf(components[2] + offset, "%d", (int) (10 * (s - (int) s)));
	}

	const size_t len_extensions = (len_components * nb_components) + nb_components;
	const size_t len_buffer = strlen(args->in_file_name) + len_extensions;
	char out_file_name[len_buffer];
	strcpy(out_file_name, args->in_file_name);

	char* cursor = NULL;

	{   // Set the cursor to right after the last '.' in the input name,
		// or at the end of the string if there is none.
		int i;
		for (i = 0; out_file_name[i] != '\0'; i++) {
			if (out_file_name[i] == '.') cursor = out_file_name + i;
		}
		if (!cursor) cursor = out_file_name + i;
	}

	for (int i = 0; i < nb_components; i++) {
		write_and_set_cursor(&cursor, "_");
		write_and_set_cursor(&cursor, components[i]);
	}

	write_and_set_cursor(&cursor, ".eps\0");

	args->out_file = fopen(out_file_name, "w");
}

Args arg_parse(int argc, char** argv) {
	if (argc == 1 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		show_help();
		exit(0);
	}

	if (argc % 2 == 0) {
		fprintf(stderr, "Bad argument number, check help if needed\n");
		exit(1);
	}

	Args rv = {};

	for (int i=1; i < argc; i += 2) {
		if (strcmp(argv[i], "-i") == 0) {
			rv.in_file_name = argv[i + 1];
			continue;
		}

		if (strcmp(argv[i], "-o") == 0) {
			rv.out_file = fopen(argv[i + 1], "w");
			continue;
		}

		if (strcmp(argv[i], "-s") == 0) {
			if (strcmp(argv[i + 1], "stroke") == 0) {
				rv.style = Stroke;
				continue;
			}
			if (strcmp(argv[i + 1], "fill") != 0) {
				fprintf(stderr, "Warning: unknown render style `%s`. "
						"Using `fill` as default.\n", argv[i + 1]);
			}
			rv.style = Fill;
			continue;
		}

		if (strcmp(argv[i], "-m") == 0) {
			if (strcmp(argv[i + 1], "segments") == 0) {
				rv.simplification = Segments;
				continue;
			}
			if (strcmp(argv[i + 1], "bezier2") == 0) {
				rv.simplification = BezierOrder2;
				continue;
			}
			if (strcmp(argv[i + 1], "bezier3") == 0) {
				rv.simplification = BezierOrder3;
				continue;
			}
			fprintf(stderr, "Warning: unknown simplification method `%s`. "
					"Using `NoSimplification` as default.\n", argv[i + 1]);
			continue;
		}

		if (strcmp(argv[i], "-d") == 0) {
			if (sscanf(argv[i + 1], "%lf", &rv.simplification_factor) != 1) {
				fprintf(stderr, "Warning: expected number after `-d` flag, "
					"got `%s`. Using 0 as default.\n", argv[i + 1]);
			}
			continue;
		}
	}

	if (!rv.in_file_name) {
		fprintf(stderr, "Error: no input file given. Aborting\n");
		exit(1);
	}

	if (!rv.out_file) genere_fichier_sortie_defaut(&rv);

	return rv;
}

int main (int argc, char** argv) {
	Args args = arg_parse(argc, argv);

	Image image = lire_fichier_image(args.in_file_name);
	ContourList* contours = get_all_contours_image(&image, args.style);

	ContourList contours_simplifies = {};
	const ContourListNode* current_contour = contours->head;
	const double distance_seuil = 2.0;
	while (current_contour) {
		append_contour(&contours_simplifies,
				simplification_douglas_peucker(current_contour->contour, distance_seuil));
		current_contour = current_contour->next;
	}

	const bool print_node_data = false;
	if (print_node_data) { // Partie 1
		write_all_contour_data(args.out_file, &contours_simplifies);
	} else { // Partie 2
		serialise_contour_list(args.out_file, &contours_simplifies, image.hauteur, image.largeur);
	}

	delete_contour_list(contours);
	supprimer_image(&image);
	return 0;
}
