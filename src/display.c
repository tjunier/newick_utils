/* 

Copyright (c) 2009 Thomas Junier and Evgeny Zdobnov, University of Geneva
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
* Neither the name of the University of Geneva nor the names of its
    contributors may be used to endorse or promote products derived from this
    software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
/* display - shows Newick as tree graph */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "config.h"
#include "common.h"
#include "list.h"
#include "parser.h"
#include "rnode.h"
#include "svg_graph.h"
#include "text_graph.h"
#include "tree.h"
#include "graph_common.h"
#include "svg_graph_common.h"
#include "error.h"

struct parameters {
	int 	width;
	bool 	svg;
	FILE 	*css_map;	
	FILE 	*url_map;
	FILE 	*ornament_map;			/* SVG */
	char 	*leaf_label_style;		/* CSS */
	char 	*inner_label_style;		/* CSS */
	char 	*edge_label_style;		/* CSS */
	char 	*plain_node_style;		/* CSS */
	double 	leaf_vskip;
	enum 	graph_style style;		/* radial or orthogonal */
	char 	*branch_length_unit;
	double 	label_angle_correction;
	double 	left_label_angle_correction;
	int 	root_length;
	double 	label_char_width;	/* for estimating label length */
	bool 	no_scale_bar;		/* suppresses scale bar if true */
	enum 	inner_lbl_pos inner_label_pos;	/* where to put the label */
	bool	scale_zero_at_root;	/* if false, at max depth */
	int	label_space_correction;	/* between a node and its label */
};

void help(char* argv[])
{
	printf(
"Displays a tree as a graph, as text or SVG.\n"
"\n"
"Synopsis\n"
"--------\n"
"\n"
"%s [options] <tree filename|->\n"
"\n"
"Input\n"
"-----\n"
"\n"
"Argument is the name of the file containing one or more trees,\n"
"or '-' (in which case the tree is read on stdin).\n"
"\n"
"Output\n"
"------\n"
"\n"
"Outputs a graph representing the tree, either as text (default) or\n"
"as SVG (option -s). Underscores in labels are replaced with spaces.\n"
"Trees with no branch lengths are taken to be cladograms and are\n"
"drawn with leaves aligned. Otherwise, the tree is assumed to be a\n"
"phylogram: branch lengths are honored and a scale bar is drawn.\n"
"\n"
"SVG graphics can be converted to other formats with converters\n"
"such as Inkscape (e.g. to PDF) or ImageMagick (e.g. to PNG or JPEG).\n"
"Inkscape is available from www.inkscape.org and ImageMagick from\n"
"www.imagemagick.org.\n"
"\n"
"Options\n"
"-------\n"
"\n"
"    -a <number>: rotate all labels by this amount (radians, default: 0)\n"
"       [only SVG radial]\n"
"    -A <number>: rotate left-side labels by this amount (radians,\n"
"       default: 0.0349 (=~ 2°)) [only SVG radial]\n"
"    -b <string>: CSS for branch length labels. [only SVG]\n"
"       Default: 'font-size:small;font-family:sans'.\n"    
"       setting 'opacity:0' disables printing of branch lengths.\n"
"    -c <filename>: use specified file as CSS map [only SVG]. A CSS map\n"
"       is a text file which specifies a style (CSS) for a clade.\n"
"       Each line has the following structure:\n"
"       <CSS> <flag> <label>+\n"
"       <CSS> is a valid CSS style specification (no spaces allowed)\n"
"       e.g. 'font-size:small;font-family:italics;stroke:green'.\n"
"       <flag> is either 'Individual' or 'Clade' (case is not important, can\n"
"       be abbreviated down to 'I' or 'C'). If set to 'Clade', the style is\n"
"       applied to the clade defined by the labels. If 'Individual', the\n"
"       style is applied to each node individually.\n"
"       <label>+ is a nonempty, whitespace-separated list of Newick labels\n"
"       (spaces are allowed IFF the label is enclosed in ''). If a label\n"
"       matches more than one node, all matching nodes are used.\n"
"       Default: no CSS map, whole tree is black (unless specified otherwise\n"
"       via option -d).\n"
"       <CSS>, <flag>, and <labels>+ are separated by whitespace.\n"
"       The following specifies red stroke for a clade defined by A, B and C;\n"
"       and 2-pixel wide, blue stroke for individual nodes D, E, and F:\n"
"       stroke:red			Clade	A B C\n"
"       stroke:blue;stroke-width:2	I	D E F\n"
"       If no colormap is specified, the default is (but see option -d):\n"
"       stroke:black;fill:none;stroke-width:1;stroke-linecap:round\n"
"    -d <string>: CSS for 'plain' tree nodes (i.e., unless overridden by -c)\n"
"       [only SVG]\n"
"       Default: stroke:black;fill:none;stroke-width:1;stroke-linecap:round\n"
"    -h: prints this message and exits\n"
"    -i <string>: CSS for inner node labels. [only SVG]\n"
"       Default: 'font-size:small;font-family:sans'.\n"    
"       setting 'visibility:hidden' disables printing of inner node labels.\n"
"    -I <char> sets the position of the inner node label. Valid options are\n"
"       'l' (near the leaves), 'm' (middle) or 'r' (near the root). Default\n"
"       is 'l'.\n"
"    -l <string>: CSS for leaf node labels. [only SVG]\n"
"       Default: 'font-size:medium;font-family:sans'.\n"    
"       setting 'visibility:hidden' disables printing of leaf node labels.\n"
"       Note: if you change this, you will probably need to adjust the\n"
"       space allocated to leaf labels - see option -W.\n"
"    -n <number> add this number of pixels to the horizontal position of\n"
"       node labels. [SVG only]\n"
"    -o <filename>: use specified file as ornament map. Works like the CSS\n"
"       map (see option -c), except that it specifies arbitrary SVG snippets\n"
"       instead of CSS styles. For example, the following\n"
"       \"<circle style='fill:red' r='5'>\" Clade A B C\n"
"       will draw a red circle of radius 5 at the root of the clade defined\n"
"       by nodes A, B, and C. Keyword 'Individual' is also accepted and\n"
"       works like for CSS.\n"
"    -R <integer>: use that many pixels for the root [only SVG]\n"
"    -r: draw a radial tree (default: orthogonal) [only SVG]\n"
"    -s: output graph as SVG (default: ASCII graphics). All output is on\n"
"       stdout, so if there is more than one tree, stdout will be a\n"
"       concatenation of SVG documents. These can be split into individual\n"
"       files with the csplit(1) command:\n"
"\n"
"       $ nw_display -s many_trees.nw > multiple_svg\n"
"       $ csplit -zs -f tree_ -b '%%02d.svg' multiple_svg '/<?xml/' {*}\n"
"\n"
"       will generate as many SVG files as there are Newick trees in\n"
"       many_trees.nw. The files will be named tree_01.svg, tree_02.svg, etc.\n"
"    -S: suppress scale bar (ignored for cladograms)\n"
"    -t: set the zero of the scale at the maximum depth instead of the root.\n"
"       This is useful when the branch lengths are in time units: zero marks\n"
"       the present, and the scale shows the age of the ancestral nodes.\n"
"    -u <string>: string is used as unit name for scale bar (ignored\n"
"       if no scale bar is drawn).\n"
"    -U <URL_filename>: use specified URL map [only SVG]. A URL map\n"
"       is a text file which specifies a URL for a label.\n"
"       Each line has the following structure:\n"
"       <label> <URL>\n"
"       Clicking on a label will follow the link (if any).\n"
"    -v <number>: number of pixels between leaves (default: 40) [only SVG\n"
"       orthogonal]\n"
"    -w <number>: graph should be no wider than <number>, measured in\n"
"       characters for text and pixels for SVG. Defaults: 80 (text),\n"
"       300 (SVG)\n"
"    -W <number>: use this as an estimate of the width of a leaf label\n"
"       character (in pixels) [only SVG]. This affects the space left for\n"
"       the tree nodes. Default: 5.0 You will probably need this if you\n"
"       change the leaf label font properties (option -l), especially size.\n"
"       You will probably need trial and error to find the right value.\n"
"\n"
"LibXML\n"
"......\n"
"\n"
"If LibXML is being used, the handling of ornaments (-o) is more elaborate.\n"
"See the tutorial, section 2.2 (displaying) for details.\n"
#ifdef HAVE_LIBXML2
"This executable uses LibXML2.\n"
#else
"This executable does NOT use LibXML2.\n"
#endif
"\n"
"Examples\n"
"--------\n"
"\n"
"# display tree as ASCII\n"
"$ %s data/catarrhini\n"
"\n"
"# display tree as SVG, using a color map\n"
"$ %s -s -c data/color.map data/catarrhini\n"
"\n"
"# ditto, convert to other graphics formats\n"
"$ %s -s -c data/color.map data/catarrhini > catarrhini.svg\n"
"$ convert catarrhini.svg catarrhini.png	# PNG, with ImageMagick\n"
"$ convert catarrhini.svg catarrhini.jpeg	# JPEG, with ImageMagick\n"
"$ inkscape -z -f catarrhini.svg -A catarrhini.pdf # PDF, with Inkscape\n" 
"\n"
"# the same; no branch lengths\n"
"$ %s -s -b 'visibility:hidden' -c data/color.map data/catarrhini\n"
"\n"
"# radial tree, leaf labels in italics\n"
"$ %s -s -r -l 'font-style:italics' data/catarrhini\n",
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0],
	argv[0]
	      );
}

enum inner_lbl_pos get_inner_label_pos(char *optarg)
{
	switch (optarg[0]) {
		case 'l':
			return INNER_LBL_LEAVES;
		case 'm':
			return INNER_LBL_MIDDLE;
		case 'r':
			return INNER_LBL_ROOT;
	}
	return -1;
}

struct parameters get_params(int argc, char *argv[])
{
	struct parameters params;

	params.width =	-1;
	params.svg = false;
	params.css_map = NULL;
	params.ornament_map = NULL;
	params.url_map = NULL;
	params.leaf_label_style = "font-size:medium;font-family:sans";
	params.inner_label_style = "font-size:small;font-family:sans";
	params.edge_label_style = "font-size:small;font-family:sans";
	params.plain_node_style = NULL;	/* svg_graph.c has default */
	params.leaf_vskip = 40.0;
	params.style = SVG_ORTHOGONAL;
	params.branch_length_unit = "substitutions/site";
	params.label_angle_correction = 0.0;
	params.left_label_angle_correction = 0.0;
	params.root_length = ROOT_SPACE;
	params.label_char_width = 8.0;
	params.no_scale_bar = false;
	params.inner_label_pos = INNER_LBL_LEAVES;
	params.scale_zero_at_root = true;
	params.label_space_correction = 0;	/* px */

	int opt_char;
	const int DEFAULT_WIDTH_PIXELS = 300;
	const int DEFAULT_WIDTH_CHARS = 80;
	int pos;
	
	/* parse options and switches */
	while ((opt_char = getopt(argc, argv, "a:A:b:c:d:hi:I:l:n:o:rR:sStu:U:v:w:W:")) != -1) {
		switch (opt_char) {
		case 'a':
			params.label_angle_correction = atof(optarg);
			break;
		case 'A':
			params.left_label_angle_correction = atof(optarg);
			break;
		case 'b':
			params.edge_label_style = optarg;
			break;
		case 'c':
			params.css_map = fopen(optarg, "r");
			if (NULL == params.css_map) {
				perror(NULL); exit(EXIT_FAILURE);
			}
			break;
		case 'd':
			params.plain_node_style = optarg;
			break;
		case 'h':
			help(argv);
			exit(EXIT_SUCCESS);
		case 'i':
			params.inner_label_style = optarg;
			break;
		case 'I':
			pos = get_inner_label_pos(optarg);
			if (pos < 0)
				fprintf(stderr, "WARNING: invalid arg '%s' "
					"to -I - using default.\n", optarg);
			else
				params.inner_label_pos = pos;
			break;
		case 'l':
			params.leaf_label_style = optarg;
			break;
		case 'o':
			params.ornament_map = fopen(optarg, "r");
			if (NULL == params.ornament_map) {
				perror(NULL); exit(EXIT_FAILURE);
			}
			break;
		case 'n':
			params.label_space_correction = atoi(optarg);
			break;
		case 'r':
			params.style = SVG_RADIAL;
			break;
		case 'R':
			params.root_length = atoi(optarg);
			break;
		case 's':
			params.svg = true;
			break;
		case 'S':
			params.no_scale_bar = true;
			break;
		case 't':
			params.scale_zero_at_root = false;
			break;
		case 'u':
			params.branch_length_unit = optarg;
			break;
		case 'U':
			params.url_map = fopen(optarg, "r");
			if (NULL == params.url_map) {
				perror(NULL); exit(EXIT_FAILURE);
			}
			break;
		case 'v':
			params.leaf_vskip = atof(optarg);
			break;
		case 'w':
			params.width = strtod(optarg, NULL);
			if (0 == params.width) {
				fprintf(stderr,
			"Argument to -w must be a positive integer.\n");
				exit(EXIT_FAILURE);
			}
			break;
		case 'W':
			params.label_char_width = atof(optarg);
			break;
		}
	}
	/* if width not set, use default (depends on whether SVG or not) */
	if (-1 == params.width) {
		if (params.svg) 
			params.width = DEFAULT_WIDTH_PIXELS;
		else
			params.width = DEFAULT_WIDTH_CHARS;
	}
	/* check arguments */
	if (1 == (argc - optind)) {
		if (0 != strcmp("-", argv[optind])) {
			FILE *fin = fopen(argv[optind], "r");
			extern FILE *nwsin;
			if (NULL == fin) {
				perror(NULL);
				exit(EXIT_FAILURE);
			}
			nwsin = fin;
		}
	} else {
		fprintf(stderr, "Usage: %s [-aAbchilsuUvw] <filename|->\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	return params;
}

/* There are so many parameters to an SVG tree that we cannot pass them all
 * to display_svg_tree() without loss of readability and clarity. Therefore
 * svg_graph.c has external variables for those parameters. Since I don't like
 * the idea of directly changing a variable in another module, those variables
 * are static and accessed through setters. This is the purpose of the
 * following function. Note that I could just pass a struct parameters to
 * display_svg_tree(), but then this would make svg_graph.c dependent on struct
 * parameters, increasing coupling. I don't like this. */

/* Also, I could set all these variables directly in get_parameters(), but I
 * think it's best to keep functions distinct -- get_parameters()'s job is to,
 * well, get the parameters, other functions then have to act on them. */

void set_svg_parameters(struct parameters params)
{
	set_width(params.width);
	set_leaf_vskip(params.leaf_vskip);
	set_label_angle_correction(params.label_angle_correction);
	set_left_label_angle_correction(params.left_label_angle_correction);
	set_URL_map_file(params.url_map);
	set_clade_CSS_map_file(params.css_map);
	set_ornament_map_file(params.ornament_map);
	set_leaf_label_style(params.leaf_label_style);
	set_inner_label_style(params.inner_label_style);
	set_inner_label_pos(params.inner_label_pos);
	set_edge_label_style(params.edge_label_style);
	set_plain_node_style(params.plain_node_style);
	set_root_length(params.root_length);
	set_label_char_width(params.label_char_width);
	set_scalebar_zero_at_root(params.scale_zero_at_root);
	add_to_label_space(params.label_space_correction);
}

/* Prints an XML comment containing the command line parameters, so that the
 * result is easier to reproduce. */

void svg_run_params_comment(int argc, char *argv[])
{
	int i;

	/* Shows the command-line arguments - but not the executable itself,
	 * since it can cause tests to fail when using libtool's shell
	 * wrappers. */
	printf ("\n<!-- arguments: " );
	for (i = 1; i < argc; i++) /* i = 1 intentional */
		printf ("%s ", argv[i]);
	printf (" -->\n" );
}

int main(int argc, char *argv[])
{
	struct rooted_tree *tree;
	struct parameters params;
	bool align_leaves;
	bool with_scale_bar;
	enum display_status status; 
	/* Stays NULL for text, but not for SVG */
	void (*node_destroyer)(void *) = NULL;

	params = get_params(argc, argv);

	if (params.svg) {
		set_svg_parameters(params);
		if(! svg_init()) {
			fprintf (stderr, "%s\n", get_last_error_message());
			exit(EXIT_FAILURE);
		}
		node_destroyer = destroy_svg_node_data;
	}

	while (NULL != (tree = parse_tree())) {
		align_leaves = is_cladogram(tree);
		/* show scale bar IFF tree is NOT a cladogram. Since
		 * is_cladogram() takes some time to run, we just look
		 * up  'align_leaves' which has the same value. */
		with_scale_bar = !align_leaves;
		/* User can also suppress scale bar */
		if (params.no_scale_bar) with_scale_bar = false;

		if (params.svg) {
			svg_header(leaf_count(tree), with_scale_bar, params.style);
			svg_run_params_comment(argc, argv);
			status = display_svg_tree(tree, params.style,
					align_leaves, with_scale_bar,
					params.branch_length_unit);
			switch(status) {
				case DISPLAY_OK:
					break;
					assert(0);
				case DISPLAY_MEM_ERROR:
					perror(NULL);
					exit(EXIT_FAILURE);
				/* The following two should never happen */
				case DISPLAY_UNKNOWN_STYLE:
				default:
					assert(0);
			}
			svg_footer();
		} else {
			prettify_labels(tree);
			status = display_tree(tree, params.width,
					align_leaves,
					params.inner_label_pos,
					with_scale_bar,
					params.branch_length_unit,
					params.scale_zero_at_root);
			switch(status) {
				case DISPLAY_OK:
					break;
					assert(0);
				case DISPLAY_MEM_ERROR:
					perror(NULL);
					exit(EXIT_FAILURE);
				/* The following two should never happen */
				case DISPLAY_UNKNOWN_STYLE:
				default:
					assert(0);
			}
		}
		destroy_all_rnodes(node_destroyer);
		destroy_tree(tree);
	}

	return 0;
}
