/*
 * elpea
 * -----
 *
 * Copyright (C) 2009 Przemysław Sitek
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string.h>

#include "ooze-stree.h"

#define N_CHILDREN 28
#define INVALID_CHAR 123

typedef struct STreeNode {
	gboolean          success;
	struct STreeNode *children[N_CHILDREN];
} STreeNode;


static int
ord (char c)
{
	if (c == '.')
		return 0;
	c = g_ascii_tolower (c);
	if (c >= 'a' && c <= 'z')
		return 1 + c - 'a';
	else
		return INVALID_CHAR;
}


OozeSTree *
ooze_stree_new (void)
{
	OozeSTree *self = g_new0 (OozeSTree, 1);
	self->root = g_slice_new0 (STreeNode);
	self->success = g_slice_new0 (STreeNode);
	return self;
}


void
ooze_stree_free (OozeSTree *self)
{
	g_free (self);
}


void
ooze_stree_add (OozeSTree   *self,
                const gchar *suffix,
                gint         len)
{
	if (len == -1)
		len = strlen (suffix);

//	g_print ("ooze_stree_add (%s)\n", suffix);
	STreeNode *node = (STreeNode *) self->root;
	const gchar *p = suffix + len - 1;
	while (p >= suffix) {
//		g_print ("'%c' ", *p);

		int c = ord (*p);
		g_return_if_fail (c != INVALID_CHAR);
		STreeNode *next = node->children[c];
		if (next == NULL) {
			next = g_slice_new0 (STreeNode);
			node->children[c] = next;
		}
		node = next;

		p--;
	}
	node->success = TRUE;
//	g_print ("\n");
}


gboolean
ooze_stree_ends_with (OozeSTree   *self,
                      const gchar *subject,
                      gint         len)
{
	if (len == -1)
		len = strlen (subject);

	//g_print ("Matching '%s'... \n", subject);

	STreeNode *node = (STreeNode *) self->root;
	const gchar *p = subject + len - 1;
	while (p >= subject) {
		if (node->success) {
			//g_print ("ok, node is succesful\n");
			return TRUE;
		}

		//g_print ("'%c' ", *p);
		int c = ord (*p);
		if (c == INVALID_CHAR) {
			//g_print ("false, '%c' is invalid\n", *p);
			return FALSE;
		}

		STreeNode *next = node->children[c];
		if (next == NULL) {
			//g_print ("false, '%c' goes to NULL\n", *p);
			return FALSE;
		}

		node = next;


		p--;
	}
	//g_print ("end -> %c\n", *p);
	//g_print ("ok");
	return TRUE;
}



