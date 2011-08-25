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

#include "config.h"

#ifdef USE_LIBXML2
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#endif

#include "list.h"
#include "rnode.h"
#include "tree.h"
#include "hash.h"
#include "node_pos_alloc.h"
#include "graph_common.h"
#include "svg_graph_common.h"
#include "math.h"
#include "masprintf.h"
#include "common.h"

/* If USE_LIBXML2 is not defined, we explicitly define it to false. */
#ifndef USE_LIBXML2	
#define USE_LIBXML2 0
#endif

extern enum inner_lbl_pos inner_label_pos;

const double PI = 3.1415926535;
const int Scale_bar_left_space = 10;
const int NUDGE_DISTANCE = 3;	/* px */
const int TEXT_ORNAMENTS_BASELINE_NUDGE = 3; /* px */

/* If this is zero, the label's baseline is aligned on the branch. Use it to
 * nudge the labels a small angle. Unfortunately the correct amount will depend
 * on the graph's diameter and the label font size. */
static double label_angle_correction = 0.0;

/* The following applies to labels on the left side of the tree (because they
 * are subject to a 180° rotation, see draw_text_radial(). The default value
 * below was determined by trial and error. */
static double left_label_angle_correction = -0.0349; /* -2°, in radians */ 

static int root_length = ROOT_SPACE;

void set_label_angle_correction(double corr)
{
	label_angle_correction = corr;
}

void set_left_label_angle_correction(double corr)
{
	left_label_angle_correction = corr;
}

void set_root_length(int length)
{
	root_length = length;
}

/****************************************************************

  LibXML functions - only compiled if libxml was requested and found. Some do
  not actually use libxml, but none are called if libxml is not used.

****************************************************************/

#if USE_LIBXML2

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

static int change_x_sign(xmlNodePtr node)
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
	else
		return FAILURE;
	xmlFree(x_value);
	return SUCCESS;
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
	xmlFree(y);
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
		char * new_value = masprintf("%s %s",
			transform, (char *) value);
		xmlSetProp(node, attr, (xmlChar *) new_value);
		free(value);
		free(new_value);
	}	
	else {
		/* set transform to translate */
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
	// NOTE: if tests fail due to rounding errors in rotation angles,
	// change the %g below into %f.
	// char *rotation = masprintf("rotate(%.2f)", angle_deg);
	char *rotation = masprintf("rotate(%g)", angle_deg);
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
	free(height_value);
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

/* unwraps the snippet out of its dummy doc, ready for embedding in the final
 * SVG. Returns NULL in case of memory problem. */ 

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

/* Transforms SVG elements by parsing XML. Argument is the ornaments string as
 * in the ornament file (i.e., an SVG snippet). Returns (and allocates - you
 * must free it) another SVG snippet in which the elements have been
 * transformed.
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

char *xml_transform_ornaments(const char *ornaments, double angle_deg, double x,
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
	apply_transforms(doc, angle_deg, x, y);

	/* now get the altered snipped, unwrapped (will be NULL if there was a
	 * problem).  */
	char *tweaked_svg = unwrap_snippet(doc);
	xmlFreeDoc(doc);

	return tweaked_svg;
}

#endif	/* USE_LIBXML2 */

/* Embeds the ornaments in an SVG <g> tag, performing some crude transforms
 * (rotation, etc). Returns an SVG snippet, or NULL in case of error. */

static char *embed_transform_ornaments(const char *svg_ornaments, double angle_deg,
		double x, double y)
{
	char *result;
	if (angle_deg <= 90 || angle_deg >= 270) {
		// right side
		result = masprintf (
			"<g style='text-anchor:end;vertical-align:super'"
			" transform='rotate(%g,%g,%g)"
			" translate(%.4f,%.4f)'>%s</g>",
			angle_deg, x, y, x, y, svg_ornaments);
	} else {
		// left side
		result = masprintf ("<g transform='rotate(180,%g,%g) "
			"rotate(%g,%g,%g) "
			"translate(%.4f,%.4f)'>%s</g>",
			x, y, angle_deg, x, y, x, y, svg_ornaments);
	}

	return result;
}

/* A dispatcher function, will call the XML-parsing transform function if
 * libXml is available, or the simpler string-embedding function if not.
 * Returns NULL in case of problem. */

static char *transform_ornaments(const char *svg_ornaments, double angle_deg,
		double x, double y)
{
#if USE_LIBXML2
	return xml_transform_ornaments(svg_ornaments, angle_deg, x, y);
#else
	return embed_transform_ornaments(svg_ornaments, angle_deg, x, y);
#endif
}

/* Draws the arc for inner nodes, including root */

/* NOTE: we keep 'large_arc_flag' as an int (even though it is semantically a
 * boolean) because it gets printed as SVG and must be '0' or '1'. */

static void draw_inner_node_arc(double top_angle,
		double bottom_angle, double radius,
		int group_nb, int large_arc_flag)
{
	double top_x_pos = radius * cos(top_angle);
	double top_y_pos = radius * sin(top_angle);
	double bot_x_pos = radius * cos(bottom_angle);
	double bot_y_pos = radius * sin(bottom_angle);
	printf("<path class='clade_%d'"
	       " d='M%.4f,%.4f A%4f,%4f 0 %d 1 %.4f %.4f'/>",
		group_nb,
		top_x_pos, top_y_pos,
		radius, radius,
		large_arc_flag,
		bot_x_pos, bot_y_pos);
}

// Should we really pass the node? Why not just pass the parent data, or
// actually, the parent's depth since this is the only use of 'node' ? OTOH
// it doesn't do any harm (we're actually passing a pointer, so it doesn't use
// up that much space), and if later we need other fields from the node, we
// don't need to alter the function's signature.

static void draw_radial_line(struct rnode *node, const double r_scale,
		double mid_angle, double mid_x_pos,
		double mid_y_pos, int group_nb)
{
	struct svg_data *parent_data = node->parent->data;
	double parent_radius = root_length + (
		r_scale * parent_data->depth);
	double par_x_pos = parent_radius * cos(mid_angle);
	double par_y_pos = parent_radius * sin(mid_angle);
	printf ("<line class='clade_%d' "
		"x1='%.4f' y1='%.4f' x2='%.4f' y2='%.4f'/>",
		group_nb,
		mid_x_pos, mid_y_pos,
		par_x_pos, par_y_pos);
}

/* Draws the ornament associated with node_data. Does NOT check for NULL --
 * this should be done by the caller. */

static int draw_ornament (struct svg_data *node_data,
		double mid_angle, double mid_x_pos,
		double mid_y_pos)
{
	/* this styling is for text, so that users can omit styles in the map
	 * file and still see the text. */
	printf("<g style='stroke:none;fill:black'>");

	char *transformed_ornaments = transform_ornaments(
			node_data->ornament,
			mid_angle / (2*PI) * 360,
			mid_x_pos, mid_y_pos);
	if (NULL != transformed_ornaments)
		printf("%s", transformed_ornaments);
	else 
		return FAILURE;
	free(transformed_ornaments);

	printf("</g>");

	return SUCCESS;
}

static int draw_branches_radial (
		struct rooted_tree *tree, const double r_scale,
		const double a_scale, bool align_leaves, double dmax)
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
		double radius = root_length + (r_scale * node_data->depth);
		double top_angle = a_scale * node_data->top; 
		double bottom_angle = a_scale * node_data->bottom; 
		double mid_angle =
			0.5 * a_scale * (node_data->top+node_data->bottom);
		double mid_x_pos = radius * cos(mid_angle);
		double mid_y_pos = radius * sin(mid_angle);
		int large_arc_flag;
		if (bottom_angle - top_angle > PI)
			large_arc_flag = 1; /* keep 1 and 0: literal SVG flags */
		else
			large_arc_flag = 0;

		/* draw node (arc), except for leaves */
		if (! is_leaf(node)) {
			draw_inner_node_arc(top_angle, bottom_angle,
					radius, group_nb, large_arc_flag);
		}
		/* draw radial line */
		if (is_root(node)) {
			printf("<line x1='0' y1='0' x2='%.4f' y2='%.4f'/>",
				mid_x_pos, mid_y_pos);
		} else {
			draw_radial_line(node, r_scale, mid_angle,
					mid_x_pos, mid_y_pos,
					group_nb);
		}
		/* draw ornament, if any */ 
		if (NULL != node_data->ornament)
			if (!draw_ornament(node_data, mid_angle, mid_x_pos,
				mid_y_pos))
				return FAILURE;
	}
	printf("</g>");

	return SUCCESS;
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
		mid_angle += left_label_angle_correction;
		x_pos = radius * cos(mid_angle);
		y_pos = radius * sin(mid_angle);
		if (nudge) {
			x_pos += (NUDGE_DISTANCE * cos(mid_angle + PI / 2));
			y_pos += (NUDGE_DISTANCE * sin(mid_angle + PI / 2));
		}
		printf(	"<text class='%s' "
			"style='text-anchor:end;' "
			"transform='rotate(%f,%g,%g) rotate(180,%g,%g)' "
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
		double mid_angle, const double r_scale)
{
	char *class;
	if (is_leaf(node)) {
		radius += label_space;
		class = leaf_label_class;
	} else {
		radius += INNER_LBL_SPACE;
		class = inner_label_class;
	}

	char *url = NULL;
	if (url_map) url = hash_get(url_map, node->label);

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
				parent_radius = root_length + (
					r_scale * parent_data->depth);
				radius = 0.5 * (radius + parent_radius);
				nudge = true;
				break;
			case INNER_LBL_ROOT:
				parent_data = node->parent->data;
				parent_radius = root_length + (
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
		const double a_scale, bool align_leaves, double dmax)
{
	printf( "<g style='stroke:none'>");

	struct list_elem *elem;
	for (elem=tree->nodes_in_order->head; NULL!=elem; elem=elem->next) {
		struct rnode *node = elem->data;
		struct svg_data *node_data = (struct svg_data *) node->data;

		/* For cladograms */
		if (align_leaves && is_leaf(node))
			node_data->depth = dmax;

		double radius = root_length + (r_scale * node_data->depth);
		double mid_angle =
			0.5 * a_scale * (node_data->top+node_data->bottom);

		mid_angle += label_angle_correction;

		/* draw label IFF it is nonempty */
		if (0 != strcmp(node->label, ""))
			draw_label(node, radius, mid_angle, r_scale);

		/* draw edge length (except for root) */
		if ((! is_root(node)) &&
			(0 != strcmp("", node->edge_length_as_string))) {
			struct svg_data *parent_data = node->parent->data;
			double parent_radius = root_length + (
				r_scale * parent_data->depth);
			radius = 0.5 * (radius + parent_radius);
			place_label(node->edge_length_as_string, radius,
					mid_angle, true, "edge-label");
		}
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
		root_length,
	       	label_space,
		hd.d_max,
	       	label_char_width * hd.l_max,
		node_area_width,
		r_scale);
}

/* Draws a radial SVG tree. Returns SUCCESS iff there was no problem. */

int display_svg_tree_radial(struct rooted_tree *tree,
		struct h_data hd, bool align_leaves, int with_scale_bar,
		char *branch_length_unit)
{
	double r_scale = -1;
	/* By using 1.9 PI instead of 2 PI, we leave a wedge that shows the
	 * tree's bounds */
	double a_scale = 1.9 * PI / leaf_count(tree); /* radians */

	if (0.0 == hd.d_max ) { hd.d_max = 1; } 	/* one-node trees */

	double node_area_width = 0.5 * graph_width
			- label_char_width * hd.l_max
			- root_length - label_space;
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
	draw_text_radial( tree, r_scale, a_scale, align_leaves, hd.d_max);
	printf ("</g>");
	if (with_scale_bar)
		draw_scale_bar(Scale_bar_left_space, (double) graph_width,
			r_scale, hd.d_max, branch_length_unit);

	return SUCCESS;
}
