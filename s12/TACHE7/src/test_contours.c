#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "contours.h"
#include "image.h"
#include "simplifications.h"
#include "linked_lists.h"

static void write_contour_data(FILE* out_file, const LL_Points* list) {
	fprintf(out_file, "%u\n", list->len - 1);
	Point* current_point;
	LL_for_each_ptr(list, current_point)
		fprintf(out_file, " %f %f\n", current_point->x, current_point->y);
}

static u_int32_t sum_segements(const LL_Contours* list) {
	u_int32_t rv = 0;
	LL_Points* current_contour;
	LL_for_each_ptr(list, current_contour) rv += current_contour->len - 1;
	return rv;
}

static void write_all_contour_data(FILE* out_file, const LL_Contours* list) {
	fprintf(out_file, "%u\n", list->len);
	LL_Points* current_contour;
	LL_for_each_ptr(list, current_contour) {
		fprintf(out_file, "\n");
		write_contour_data(out_file, current_contour);
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
	double distance_seuil;
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
#define nb_components 3
#define len_components 8
	/* const size_t nb_components = 3; */ 
	/* const size_t len_components = 8; */ 
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

		u_int8_t offset = log10(args->distance_seuil) + 2;
		sprintf(components[2], "%d", (int) args->distance_seuil);
		double s = args->distance_seuil;  // ’cause I’m lazy
		u_int8_t decimales = (int) (10 * (s - (int) s));
		if (decimales != 0)
				sprintf(components[2] + offset, "_%d", decimales);
	}

	const size_t len_extensions = (len_components * nb_components) + nb_components;
	const size_t len_buffer = strlen(args->in_file_name) + len_extensions;
	char out_file_name[len_buffer];
	for (int i = 0; i < len_buffer; i++) out_file_name[i] = '\0';
	strcpy(out_file_name, args->in_file_name);

	char* cursor = NULL;

	{   // Set the cursor to right on the last '.' in the input name,
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
#undef nb_components
#undef len_components
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

	Args rv = { 0, };

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
			if (sscanf(argv[i + 1], "%lf", &rv.distance_seuil) != 1) {
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
	LL_Contours* contours = get_all_contours_image(&image);

	if (args.simplification == NoSimplification) {
		write_all_contour_data(args.out_file, contours);
		goto no;
	}

	fprintf(args.out_file, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	fprintf(args.out_file, "%%%%BoundingBox: -1 -1 %u %u\n",
				image.largeur + 1, image.hauteur + 1);

	LL_Points* current_contour;
	LL_for_each_ptr(contours, current_contour) {
		if (args.simplification == Segments) {
			LL_Points* contour_simplifie = simplification_douglas_peucker(current_contour, args.distance_seuil);
			Point* current_point = LL_pop_ptr(contour_simplifie);
			fprintf(args.out_file, "%f %f moveto\n", current_point->x, image.hauteur - current_point->y);
			free(current_point);

			LL_for_each_ptr(contour_simplifie, current_point)
				fprintf(args.out_file, "%f %f lineto\n", current_point->x, image.hauteur - current_point->y);

			LL_delete(contour_simplifie);
		}
		else if (args.simplification == BezierOrder2) {
			LL_Bezier2* contour_simplifie = simplification_bezier2(current_contour, args.distance_seuil);
			Bezier2* curve = contour_simplifie->head->content;
			fprintf(args.out_file, "%f %f moveto\n", curve->start.x, image.hauteur - curve->start.y);

			LL_for_each_ptr(contour_simplifie, curve) {
				Bezier3 curve3 = bezier3_from_bezier2(*curve);
				fprintf(args.out_file, "%f %f %f %f %f %f curveto\n",
						curve3.control1.x, image.hauteur - curve3.control1.y,
						curve3.control2.x, image.hauteur - curve3.control2.y,
						curve3.end.x, image.hauteur - curve3.end.y);
			}

			LL_delete(contour_simplifie);
		}
		else if (args.simplification == BezierOrder3) {
			LL_Bezier3* contour_simplifie = simplification_bezier3(current_contour, args.distance_seuil);
			Bezier3* curve = contour_simplifie->head->content;
			fprintf(args.out_file, "%f %f moveto\n", curve->start.x, image.hauteur - curve->start.y);

			LL_for_each_ptr(contour_simplifie, curve) {
				fprintf(args.out_file, "%f %f %f %f %f %f curveto\n",
						curve->control1.x, image.hauteur - curve->control1.y,
						curve->control2.x, image.hauteur - curve->control2.y,
						curve->end.x, image.hauteur - curve->end.y);
			}

			LL_delete(contour_simplifie);
		}
	}
	
	if (args.style == Fill) {
		fprintf(args.out_file, "fill\n");
	}
	else {
		fprintf(args.out_file, "0.1 setlinewidth\nstroke\n");
	}

	fprintf(args.out_file, "showpage\n");

no:

	LL_delete(contours);
	supprimer_image(&image);
	fclose(args.out_file);
	return 0;
}
