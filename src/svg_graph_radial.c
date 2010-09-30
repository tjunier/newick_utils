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
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "list.h"
#include "rnode.h"
#include "tree.h"
#include "hash.h"
#include "node_pos_alloc.h"
#include "graph_common.h"
#include "svg_graph_common.h"
#include "math.h"
#include "masprintf.h"

extern enum inner_lbl_pos inner_label_pos;

const double PI = 3.1415926535;
const int Scale_bar_left_space = 10;
const int NUDGE_DISTANCE = 3;	/* px */
const int TEXT_ORNAMENTS_BASELINE_NUDGE = 3; /* px */

// TODO: the svg_ prefix that many variables have is probably not really
// necessary. Also, replace logical ints by booleans.

/* If this is zero, the label's baseline is aligned on the branch. Use it to
 * nudge the labels a small angle. Unfortunately the correct amount will depend
 * on the graph's diameter and the label font size. */
static double svg_label_angle_correction = 0.0;

/* The following applies to labels on the left side of the tree (because they
 * are subject to a 180° rotation, see draw_text_radial(). The default value
 * below was determined by trial and error. */
static double svg_left_label_angle_correction = -0.0349; /* -2°, in radians */ 

static int svg_root_length = ROOT_SPACE;

void set_svg_label_angle_correction(double corr)
{
	svg_label_angle_correction = corr;
}

void set_svg_left_label_angle_correction(double corr)
{
	svg_left_label_angle_correction = corr;
}

void set_svg_root_length(int length)
{
	svg_root_length = length;
}

static char * wrap_in_dummy_doc(const char *svg_snippet)
{
	const char * dummy_doc_start = "<dummy xmlns:xlink=\"http://www.w3.org/1999/xlink\">";
	const char * dummy_doc_stop = "</dummy>";
	char *doc_str;


	/* wrap snippet into dummy document */
	size_t start_length = strlen(dummy_doc_start);
	size_t snippet_length = strlen(svg_snippet);
	size_t stop_length = strlen(dummy_doc_stop);
	size_t doc_length = start_length + snippet_length + stop_length;
	/* + 1 for trailing '\0' */
	doc_str = malloc((doc_length + 1) * sizeof(char));
	if (NULL == doc_str) return NULL;

	strcpy(doc_str, dummy_doc_start);
	strcpy(doc_str + start_length, svg_snippet);
	strcpy(doc_str + start_length + snippet_length, dummy_doc_stop);

	return doc_str;
}

/* changes the x-attribute's sign */

static void change_x_sign(xmlNodePtr node)
{
	xmlChar *x = (xmlChar *) "x";
	xmlChar *x_value = xmlGetProp(node, x);
	if (NULL != x_value) {
		double x_val = atof((char *) x_value);
		x_val *= -1.0;
		char *new_value = masprintf("%g", x_val);
		xmlSetProp(node, x, (xmlChar *) new_value);
		free(new_value);
	}	
	xmlFree(x_value);
}	

/* nudges y-attribute above the baseline (for text) */

static void nudge_baseline(xmlNodePtr node)
{
	xmlChar *y_attr = (xmlChar*) "y";
	xmlChar *y = xmlGetProp(node, y_attr);
	double y_value; /* initialized below */
	if (NULL != y) 
		y_value = atof((char *) y);
	 else 
		y_value = 0 ;
	y_value -= TEXT_ORNAMENTS_BASELINE_NUDGE;
	char *new_y_val = masprintf("%g", y_value);
	xmlSetProp(node, y_attr, (xmlChar *) new_y_val);
	free(new_y_val);
}

/* prepends a transform to the transform attribute of the element - the effect
 * is to do compose the new tranform to the existing ones (think matrix
 * multiplication) */

static void prepend_transform(xmlNodePtr node, char *transform)
{
	const xmlChar *attr = (xmlChar *) "transform";
	xmlChar *value = xmlGetProp(node, attr);
	if (NULL != value) {
		/* prepend translate to existing transform(s) */
		// fprintf (stderr, "Transform: %s, applying %s\n", value, transform);
		char * new_value = masprintf("%s %s",
			transform, (char *) value);
		xmlSetProp(node, attr, (xmlChar *) new_value);
		free(value);
		free(new_value);
	}	
	else {
		/* set transform to translate */
		// fprintf (stderr, "No transform yet. Applying %s\n", transform);
		xmlSetProp(node, attr, (xmlChar *) transform); 
	}
}

/* adds a translation(x,y) to the element */

static void translate(xmlNodePtr node, double x, double y)
{
	char *translation = masprintf("translate(%g,%g)", x, y);
	prepend_transform(node, translation);
	free(translation);
}

/* adds a rotation(angle_deg) to the element */

static void rotate(xmlNodePtr node, double angle_deg)
{
	/* We normally use %g as a number format, because it allows both
	 * maximum precision when possible, yet doesn't add meaningless decimal
	 * places if it can be avoided. The problem here is that in the "orn_r"
	 * test case of test_nw_display.sh, the third decimal is slightly
	 * different on different machines. I therefore round to two decimal
	 * places so the tests pass. I have checked (see src/chord.R) that this
	 * rounding off of 1/1000 degree should not affect trees unless their
	 * radius is over 5'000 pixels long. */
	char *rotation = masprintf("rotate(%g)", angle_deg);
	// char *rotation = masprintf("rotate(%.2f)", angle_deg);
	prepend_transform(node, rotation);
	free(rotation);
}

/* special transforms for <text> elements */

static void text_transforms(xmlNodePtr node, double angle_deg,
		double x, double y)
{
	nudge_baseline(node); /* so text can be read */
	if (angle_deg <= 90 || angle_deg >= 270) {
		// right side (cos >= 0) 
		/* ensure end-anchoring */
		xmlChar *style_attr = (xmlChar*) "style";
		xmlChar *style = xmlGetProp(node, style_attr);
		if (NULL != style) {
			char * new_style = masprintf("%s;%s",
					(char *) style, "text-anchor:end");
			xmlSetProp(node, style_attr, (xmlChar *) new_style);
			free(style);
			free(new_style);
		}
		else {
			xmlSetProp(node, style_attr,
					(xmlChar*) "text-anchor:end");
		}
	} else {
		// left side (cos < 0)
		char * half_turn = masprintf("rotate(180,%g,%g)", x, y);
		prepend_transform(node, half_turn);
		free(half_turn);
		/* the rotation causes any x value to have wrong sign, so: */
		change_x_sign(node); 
	}
}

/* special transforms for <image> elements */

/* centers an image vertically around the node position ("vertically" is to be
 * understood _before_ rotation (i.e., as for a rotation of 0) */

static void center_vertically(xmlNodePtr node)
{
	xmlChar * height_attr = (xmlChar *) "height";
	xmlChar * height_value = xmlGetProp(node, height_attr);
	if (NULL == height_value) {
		fprintf (stderr, "WARNING: <image> has no height.\n");
		return;
	}
	double height = atof((char *) height_value);
	xmlChar * y_attr = (xmlChar *) "y";
	xmlChar * y_value = xmlGetProp(node, y_attr);
	double y;
	if (NULL != y_value)
		y = atof((char *) y_value);
	else
		y = 0;
	y -= height / 2;
	char *new_y_value = masprintf("%g", y);
	xmlSetProp(node, y_attr, (xmlChar *) new_y_value);
	free(new_y_value);
}

/* shifts an image one image width leafwards */

static void shift_one_width_leafwards(xmlNodePtr node)
{
	xmlChar *width_attr = (xmlChar *) "width";
	xmlChar *width_value = xmlGetProp(node, width_attr);
	if (NULL == width_value) {
		fprintf (stderr, "WARNING: <image> has no width.\n");
		return;
	}
	double width = atof((char *) width_value);

	/* could also use a translate() transform, but this would have to be
	prepended _before_ the other transforms. */
	double x = 0.0;
	xmlChar *x_attr = (xmlChar *) "x";
	xmlChar *x_value = xmlGetProp(node, x_attr);
	if (NULL != x_value) 
		x = atof((char *) x_value);
	x -= width;
	char *new_value = masprintf("%g", x);
	xmlSetProp(node, x_attr, (xmlChar *) new_value);
	free(new_value);
	xmlFree(x_value);
	xmlFree(width_value);
}	

static void image_transforms(xmlNodePtr node, double angle_deg,
		double x, double y)
{
	center_vertically(node);
	/* if the image is on the left side, we i) rotate it 180° around the
	 * node (tip of the parent edge)(so that it is not upside-down), and
	 * ii) shift it one image length leafwards (to correct for the
	 * rootwards shift caused by rotation) */
	if (angle_deg > 90 && angle_deg < 270) {
		// left side (cos < 0)
		char * half_turn = masprintf("rotate(180,%g,%g)", x, y);
		prepend_transform(node, half_turn);
		free(half_turn);
		shift_one_width_leafwards(node);
	}
}

void apply_transforms(xmlDocPtr doc, double angle_deg, double x, double y)
{
	xmlNodePtr cur = xmlDocGetRootElement(doc)->xmlChildrenNode;
	while (NULL != cur) {
		/* apply the transforms, in this order */
		rotate(cur, angle_deg);
		translate(cur, x, y);
		if (strcmp("text", (char *) cur->name) == 0)
			text_transforms(cur, angle_deg, x, y);
		else if (strcmp("image", (char *) cur->name) == 0)
			image_transforms(cur, angle_deg, x, y);

		cur = cur->next;	/* sibling */
	}
}

static char *unwrap_snippet(xmlDocPtr doc)
{
	/* this will give a size that is certain to be enough */
	xmlChar *xml_buf;
	int buf_length;
	xmlDocDumpFormatMemory(doc, &xml_buf, &buf_length, 0); 
	xmlFree(xml_buf);	/* only need buf_length */
	/* so, allocate that much (cleared) */
	char *tweaked_svg = calloc(buf_length, sizeof(char));
	if (NULL == tweaked_svg) return NULL;
	// TODO: the following 2 lines can be merged
	xmlNodePtr cur = xmlDocGetRootElement(doc);
	cur = cur->xmlChildrenNode;
	while (NULL != cur) {
		xmlBufferPtr buf = xmlBufferCreate ();
		xmlNodeDump (buf, doc, cur, 0, 0);
		const xmlChar * contents = xmlBufferContent(buf);
		int cur_len = strlen(tweaked_svg);
		/* appends to tweaked_svg - cur_len must initially be 0, which
		 * is why we use calloc() */
		strcpy(tweaked_svg + cur_len, (char *) contents);
		xmlBufferFree(buf);
		cur = cur->next;	/* sibling */
	}
	return tweaked_svg;
}

/* Outputs an SVG <g> element with all the tree branches, radial. In this
 * context, a node's 'top' and 'bottom' are angles, not vertical positions */

/* Transforms SVG elements. Argument is the ornaments string as in the ornament
 * file (i.e., an SVG snippet). Returns (and allocates - you must free it)
 * another SVG snippet in which the elements have been transformed.
 * Transformations * include:
 *	o translation to node position (all elements)
 *	o rotation (the same as node edge and node labels) (all elements)
 *	o 180° rotation and/or alignment (text - so that it always reads
 *	  left-to-right)
 *	o further rotation and translation (images - so that they are oriented
 *	  right)
 *
 * This f() is not static because it needs to be tested directly (in
 * test_svg_graph_radial); but it is not in any header file either, because it
 * is not meant to be used outside this module.
 * Returns NULL in case of failure. */

char *transform_ornaments(const char *ornaments, double angle_deg, double x,
		double y)
{

	xmlDocPtr doc;

	char *wrapped_orn = wrap_in_dummy_doc(ornaments);
	if (NULL == wrapped_orn) return NULL;

	/* parse SVG from string */
	doc = xmlParseMemory(wrapped_orn, strlen(wrapped_orn));
	if (NULL == doc) {
		fprintf(stderr, "Failed to parse document\n");
		return NULL;
	}
	free(wrapped_orn);

	/* tweak according to element type */
	//fprintf(stderr, "%s: translating to (%g,%g)\n", __func__, x, y);
	apply_transforms(doc, angle_deg, x, y);

	/* now print out the altered snipped, unwrapped.  */
	char *tweaked_svg = unwrap_snippet(doc);
	xmlFreeDoc(doc);

	return tweaked_svg;
}

/* Draws the arc for inner nodes, including root */

static void draw_inner_node_arc(double svg_top_angle,
		double svg_bottom_angle, double svg_radius,
		int group_nb, int large_arc_flag)
{
	double svg_top_x_pos = svg_radius * cos(svg_top_angle);
	double svg_top_y_pos = svg_radius * sin(svg_top_angle);
	double svg_bot_x_pos = svg_radius * cos(svg_bottom_angle);
	double svg_bot_y_pos = svg_radius * sin(svg_bottom_angle);
	printf("<path class='clade_%d'"
	       " d='M%.4f,%.4f A%4f,%4f 0 %d 1 %.4f %.4f'/>",
		group_nb,
		svg_top_x_pos, svg_top_y_pos,
		svg_radius, svg_radius,
		large_arc_flag,
		svg_bot_x_pos, svg_bot_y_pos);
}

// TODO: should we really pass the node? Why can't we just pass the parent
// data,or actually, the parent's depth since this is the only use of 'node' ? 

static void draw_radial_line(struct rnode *node, const double r_scale,
		double svg_mid_angle, double svg_mid_x_pos,
		double svg_mid_y_pos, int group_nb)
{
	struct svg_data *parent_data = node->parent->data;
	double svg_parent_radius = svg_root_length + (
		r_scale * parent_data->depth);
	double svg_par_x_pos = svg_parent_radius * cos(svg_mid_angle);
	double svg_par_y_pos = svg_parent_radius * sin(svg_mid_angle);
	//fprintf(stderr, "node pos: (%g,%g)\n", svg_mid_x_pos, svg_mid_y_pos);
	printf ("<line class='clade_%d' "
		"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
		group_nb,
		svg_mid_x_pos, svg_mid_y_pos,
		svg_par_x_pos, svg_par_y_pos);
}

/* Draws the ornament associated with node_data. Does NOT check for NULL --
 * this should be done by the caller. */

static void draw_ornament (struct svg_data *node_data,
		double svg_mid_angle, double svg_mid_x_pos,
		double svg_mid_y_pos)
{
	/* this styling is for text, so that users can omit styles in the map
	 * file and still see the text. */
	//fprintf(stderr, "%s: translating to (%g,%g)\n", __func__, svg_mid_x_pos, svg_mid_y_pos);
	// fprintf(stderr, "angle = %g, Pi = %g\n", svg_mid_angle, PI);
	// fprintf(stderr, "angle = %g°\n", svg_mid_angle / (2*PI) * 360);
	printf("<g style='stroke:none;fill:black'>");
	char *transformed_ornaments = transform_ornaments(
			node_data->ornament,
			svg_mid_angle / (2*PI) * 360,
			svg_mid_x_pos, svg_mid_y_pos);
	printf("%s", transformed_ornaments);
	// fprintf(stderr, "%s\n", transformed_ornaments);
	free(transformed_ornaments);
	printf("</g>");
	/*
	if (cos(svg_mid_angle) >= 0) {
		// right side
		// TODO: apply transform
		printf ("<g style='text-anchor:end;vertical-align:super'"
			" transform='rotate(%g,%g,%g)"
			" translate(%.4f,%.4f)'>%s</g>",
			svg_mid_angle / (2*PI) * 360,
			svg_mid_x_pos, svg_mid_y_pos,
			svg_mid_x_pos, svg_mid_y_pos,
			node_data->ornament);
	} else {
		// left side
		// TODO: apply transform
		// char *orn_chs_x = change_svg_x_attr_sign(node_data->ornament);
		printf ("<g transform='"
			"rotate(180,%g,%g) "
			"rotate(%g,%g,%g) "
			"translate(%.4f,%.4f)'>%s</g>",
			svg_mid_x_pos, svg_mid_y_pos,
			svg_mid_angle / (2*PI) * 360,
			svg_mid_x_pos, svg_mid_y_pos,
			svg_mid_x_pos, svg_mid_y_pos,
			node_data->ornament);
	} */
}

static void draw_branches_radial (struct rooted_tree *tree, const double r_scale,
		const double a_scale, int align_leaves, double dmax)
{
	printf( "<g"
	       	" style='stroke:black;fill:none;stroke-width:1;"
		"stroke-linecap:round'>"
	    	);

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		int group_nb = node_data->group_nb;
		double svg_radius = svg_root_length + (r_scale * node_data->depth);
		double svg_top_angle = a_scale * node_data->top; 
		double svg_bottom_angle = a_scale * node_data->bottom; 
		double svg_mid_angle =
			0.5 * a_scale * (node_data->top+node_data->bottom);
		double svg_mid_x_pos = svg_radius * cos(svg_mid_angle);
		double svg_mid_y_pos = svg_radius * sin(svg_mid_angle);
		int large_arc_flag;
		if (svg_bottom_angle - svg_top_angle > PI)
			large_arc_flag = 1; /* keep 1 and 0: literal SVG flags */
		else
			large_arc_flag = 0;

		/* draw node (arc), except for leaves */
		if (! is_leaf(node)) {
			draw_inner_node_arc(svg_top_angle, svg_bottom_angle,
					svg_radius, group_nb, large_arc_flag);
		}
		/* draw radial line */
		if (is_root(node)) {
			printf("<line x1='0' y1='0' x2='%.4f' y2='%.4f'/>",
				svg_mid_x_pos, svg_mid_y_pos);
		} else {
			draw_radial_line(node, r_scale, svg_mid_angle,
					svg_mid_x_pos, svg_mid_y_pos,
					group_nb);
		}
		/* draw ornament, if any */ 
		if (NULL != node_data->ornament)
			draw_ornament(node_data, svg_mid_angle, svg_mid_x_pos,
				svg_mid_y_pos);
	}
	printf("</g>");
}

/* lower-level label drawing */

static void place_label(const char *label, const double radius, double
		mid_angle, const bool nudge, const char *class)
{
	double x_pos;
	double y_pos;

	if (cos(mid_angle) >= 0)  {
		x_pos = radius * cos(mid_angle);
		y_pos = radius * sin(mid_angle);
		if (nudge) {
			x_pos -= (NUDGE_DISTANCE * cos(mid_angle + PI / 2));
			y_pos -= (NUDGE_DISTANCE * sin(mid_angle + PI / 2));
		}
		printf("<text class='%s' "
		       "transform='rotate(%g,%g,%g)' "
		       "x='%.4f' y='%.4f'>%s</text>",
			class,
			mid_angle / (2*PI) * 360,
			x_pos, y_pos,
			x_pos, y_pos, label);
	}
	else {
		mid_angle += svg_left_label_angle_correction;
		x_pos = radius * cos(mid_angle);
		y_pos = radius * sin(mid_angle);
		if (nudge) {
			x_pos += (NUDGE_DISTANCE * cos(mid_angle + PI / 2));
			y_pos += (NUDGE_DISTANCE * sin(mid_angle + PI / 2));
		}
		fprintf(stderr, "angle = %g (%g°), Pi = %g\n",
				mid_angle, mid_angle / (2*PI) * 360, PI);
		printf(	"<text class='%s' "
			"style='text-anchor:end;' "
			"transform='rotate(%g,%g,%g) rotate(180,%g,%g)' "
		       "x='%.4f' y='%.4f'>%s</text>",
			class,
			mid_angle / (2*PI) * 360,
			x_pos, y_pos,
			x_pos, y_pos,
			x_pos, y_pos, label);
	}
}

/* Draws a node label */

static void draw_label(struct rnode *node, double radius,
		double mid_angle, const double r_scale,
		const char *class, const char *url)
{
	/* Will set this to true when the label must be drawn parallel to the
	 * branch, rather than on the exact same line */
	bool nudge = false;

	if (url) printf ("<a %s>", url);

	if (is_inner_node(node)) {
		struct svg_data *parent_data = NULL;
		double parent_radius = -1;
		switch (inner_label_pos) {
			case INNER_LBL_LEAVES:
				break;
			case INNER_LBL_MIDDLE:
				parent_data = node->parent->data;
				parent_radius = svg_root_length + (
					r_scale * parent_data->depth);
				radius = 0.5 * (radius + parent_radius);
				nudge = true;
				break;
			case INNER_LBL_ROOT:
				parent_data = node->parent->data;
				parent_radius = svg_root_length + (
					r_scale * parent_data->depth);
				radius = parent_radius;
				nudge = true;
				break;
			default:
				assert(0);
		}
	}

	place_label(node->label, radius, mid_angle, nudge, class);

	if (url) printf("</a>");
}

/* Prints the node text (labels and lengths) in a <g> element, radial */

static void draw_text_radial (struct rooted_tree *tree, const double r_scale,
		const double a_scale, int align_leaves, double dmax)
{
	printf( "<g style='stroke:none'>");

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		double radius = svg_root_length + (r_scale * node_data->depth);
		double mid_angle =
			0.5 * a_scale * (node_data->top+node_data->bottom);

		mid_angle += svg_label_angle_correction;

		if (is_leaf(node))
			radius += label_space;
		else
			radius += INNER_LBL_SPACE;

		char *url = NULL;
		if (url_map) url = hash_get(url_map, node->label);

		char *class;
		if (is_leaf(node))
			class = leaf_label_class;
		else
			class = inner_label_class;

		/* draw label IFF it is nonempty */
		if (0 != strcmp(node->label, ""))
			draw_label(node, radius, mid_angle, r_scale,
					class, url);
		/* TODO: add this when node labels work */
		/*
		if (! is_root(node)) {
			struct rnode *parent = node->parent_edge->parent_node;
			struct svg_data *parent_data = parent->data;
			double svg_parent_h_pos = svg_root_length + (
				h_scale * parent_data->depth);
				*/
			/* Print branch length IFF it is nonempty AND 
			 * requested size is not 0 */
			/*
			if (0 != strcmp(branch_length_font_size, "0") &&
			    0 != strcmp(node->parent_edge->length_as_string, "")) {
				printf("<text style='stroke:none;font-size:%s' "
					"x='%4f' y='%4f'>%s</text>",
					branch_length_font_size,
					(svg_h_pos + svg_parent_h_pos) / 2.0,
					edge_length_v_offset + svg_mid_pos,
					node->parent_edge->length_as_string);
			}
		}
		*/
	}
	printf("</g>");
}

/* Prints an SVG comment block with some run parameters. Useful for debugging */

static void params_as_svg_comment (struct h_data hd, double node_area_width,
		double r_scale)
{
	printf ("<!-- SVG parameters:\n"
		"    primary params:\n"
		"    ..............\n"
		"    graph width [g]:\t\t%4d px\n"
		"    label char width [c]:\t%4g px\n"
		"    max label length [l]:\t%4d characters\n"
		"    root length [r]:\t\t%4d px\n"
		"    label space [s]:\t\t%4d px\n"
		"    tree depth [d]:\t\t%4g arbitrary units\n\n"
		"    derived params:\n"
		"    ..............\n"
		"    label area [cl]:\t\t\t%4g px\n"
		"    node area [n = g/2 - cl - r - s]:\t%4g px\n"
		"    radial scale [S = n / d]:\t\t%4g px / arbitrary units\n"
		"  -->\n",
		graph_width,
	       	label_char_width,
	       	hd.l_max,
		svg_root_length,
	       	label_space,
		hd.d_max,
	       	label_char_width * hd.l_max,
		node_area_width,
		r_scale);
}

void display_svg_tree_radial(struct rooted_tree *tree,
		struct h_data hd, int align_leaves, int with_scale_bar,
		char *branch_length_unit)
{
	double r_scale = -1;
	/* By using 1.9 PI instead of 2 PI, we leave a wedge that shows the
	 * tree's bounds */
	double a_scale = 1.9 * PI / leaf_count(tree); /* radians */

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */

	/* TODO: why twice svg_root_length? */
	double node_area_width = 0.5 * graph_width
			- label_char_width * hd.l_max
			- svg_root_length - label_space;
	r_scale = node_area_width / hd.d_max;

	// params_as_svg_comment(hd, node_area_width, r_scale);

	/* Given that we only 'use' 1.9 PI instead of 2 PI (see above), we can
	 * rotate the whole graph by 0.05 PI so that we can draw the scale bar
	 * on the horizontal axis. */
	/* NOTE: I've tried drawing the scale bar from the tree's center. It's
	 * not as good as it seems, so for now I shall draw it in the lower
	 * left corner, just like in an orthogonal tree. If someone really
	 * insists on this, it can be reenabled by commenting out the following
	 * (and commenting the appropriate parts), and drwaing the scale bar at
	 * (0, 0) (actually, needs a bit of vertical nudging, see
	 * draw_scale_bar() */
	
	// double rotation = 0.1 * PI / (2 * PI) * 360;	/* degrees */
	/*
	printf( "<g transform='translate(%g,%g) rotate(%g)'>",
		 	graph_width / 2.0, graph_width / 2.0, rotation);
	*/

	printf( "<g transform='translate(%g,%g)'>", graph_width / 2.0,
			graph_width / 2.0); 
	/* We draw all the tree's branches in an SVG group of their own, to
	 * facilitate editing. */
	draw_branches_radial(tree, r_scale, a_scale, align_leaves, hd.d_max);
	/* likewise for text */
	draw_text_radial(tree, r_scale, a_scale, align_leaves, hd.d_max);
	printf ("</g>");
	if (with_scale_bar)
		draw_scale_bar(Scale_bar_left_space, (double) graph_width,
				r_scale, hd.d_max, branch_length_unit);
}
