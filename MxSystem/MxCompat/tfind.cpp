/*****************************************************************************
 * tfind.c : implement every t* fuctions
 *****************************************************************************/
#include "stdafx.h"

/** search.h is not present so every t* functions has to be implemented */
//#ifndef HAVE_SEARCH_H

#include <assert.h>
#include <stdlib.h>
#include "MxFixups.h"

typedef struct node {
    char         *key;
    struct node  *llink, *rlink;
} node_t;

/*	$NetBSD: tdelete.c,v 1.4 2006/03/19 01:12:08 christos Exp $	*/

/*
 * Tree search generalized from Knuth (6.2.2) Algorithm T just like
 * the AT&T man page says.
 *
 * The node_t structure is for internal use only, lint doesn't grok it.
 *
 * Written by reading the System V Interface Definition, not the code.
 *
 * Totally public domain.
 */

/* delete node with given key */
void *tdelete(const void *vkey, void **vrootp, int(*compar) (const void *, const void *))
{
	node_t **rootp = (node_t **)vrootp;
	node_t *p, *q, *r;
	int  cmp;

	assert(vkey != NULL);
	assert(compar != NULL);

	if (rootp == NULL || (p = *rootp) == NULL)
		return NULL;

	while ((cmp = (*compar)(vkey, (*rootp)->key)) != 0) {
		p = *rootp;
		rootp = (cmp < 0) ?
		    &(*rootp)->llink :		/* follow llink branch */
		    &(*rootp)->rlink;		/* follow rlink branch */
		if (*rootp == NULL)
			return NULL;		/* key not found */
	}
	r = (*rootp)->rlink;			/* D1: */
	if ((q = (*rootp)->llink) == NULL)	/* Left NULL? */
		q = r;
	else if (r != NULL) {			/* Right link is NULL? */
		if (r->llink == NULL) {		/* D2: Find successor */
			r->llink = q;
			q = r;
		} else {			/* D3: Find NULL link */
			for (q = r->llink; q->llink != NULL; q = r->llink)
				r = q;
			r->llink = q->rlink;
			q->llink = (*rootp)->llink;
			q->rlink = (*rootp)->rlink;
		}
	}
	if (p != *rootp)
		free(*rootp);			/* D4: Free node */
	*rootp = q;				/* link parent to new node */
	return p;
}


/*	$NetBSD: tdestroy.c,v 1.2 1999/09/16 11:45:37 lukem Exp $	*/

/*
 * Tree search generalized from Knuth (6.2.2) Algorithm T just like
 * the AT&T man page says.
 *
 * The node_t structure is for internal use only, lint doesn't grok it.
 *
 * Written by reading the System V Interface Definition, not the code.
 *
 * Totally public domain.
 */

/* Walk the nodes of a tree */
static void
tdestroy_recurse(node_t* root, void (*free_action)(void *))
{
  if (root->llink != NULL)
    tdestroy_recurse(root->llink, free_action);
  if (root->rlink != NULL)
    tdestroy_recurse(root->rlink, free_action);

  (*free_action) ((void *) root->key);
  free(root);
}

void tdestroy(void *vrootp, void(*freefct)(void *))
{
  node_t *root = (node_t *) vrootp;

  if (root != NULL)
    tdestroy_recurse(root, freefct);
}


/*	$NetBSD: tfind.c,v 1.5 2005/03/23 08:16:53 kleink Exp $	*/

/*
 * Tree search generalized from Knuth (6.2.2) Algorithm T just like
 * the AT&T man page says.
 *
 * The node_t structure is for internal use only, lint doesn't grok it.
 *
 * Written by reading the System V Interface Definition, not the code.
 *
 * Totally public domain.
 */

/* find a node, or return 0 */
void *tfind(const void *vkey, const void **vrootp, int(*compar) (const void *, const void *))
{
	node_t * const *rootp = (node_t * const*)vrootp;

	assert(vkey != NULL);
	assert(compar != NULL);

	if (rootp == NULL)
		return NULL;

	while (*rootp != NULL) {		/* T1: */
		int r;

		if ((r = (*compar)(vkey, (*rootp)->key)) == 0)	/* T2: */
			return *rootp;		/* key found */
		rootp = (r < 0) ?
		    &(*rootp)->llink :		/* T3: follow left branch */
		    &(*rootp)->rlink;		/* T4: follow right branch */
	}
	return NULL;
}


/*	$NetBSD: tsearch.c,v 1.5 2005/11/29 03:12:00 christos Exp $	*/

/*
 * Tree search generalized from Knuth (6.2.2) Algorithm T just like
 * the AT&T man page says.
 *
 * The node_t structure is for internal use only, lint doesn't grok it.
 *
 * Written by reading the System V Interface Definition, not the code.
 *
 * Totally public domain.
 */

/* find or insert datum into search tree */
void *tsearch(const void *vkey, void **vrootp, int(*compar) (const void *, const void *))
{
	node_t *q;
	node_t **rootp = (node_t **)vrootp;

	assert(vkey != NULL);
	assert(compar != NULL);

	if (rootp == NULL)
		return NULL;

	while (*rootp != NULL) {	/* Knuth's T1: */
		int r;

		if ((r = (*compar)(vkey, (*rootp)->key)) == 0)	/* T2: */
			return *rootp;		/* we found it! */

		rootp = (r < 0) ?
		    &(*rootp)->llink :		/* T3: follow left branch */
		    &(*rootp)->rlink;		/* T4: follow right branch */
	}

	q = (node_t *)malloc(sizeof(node_t));		/* T5: key not found */
	if (q != 0) {				/* make new node */
		*rootp = q;			/* link new node to old */
		q->key = (char*)vkey;	/* initialize new node */
		q->llink = q->rlink = NULL;
	}
	return q;
}


/*	$NetBSD: twalk.c,v 1.2 1999/09/16 11:45:37 lukem Exp $	*/

/*
 * Tree search generalized from Knuth (6.2.2) Algorithm T just like
 * the AT&T man page says.
 *
 * The node_t structure is for internal use only, lint doesn't grok it.
 *
 * Written by reading the System V Interface Definition, not the code.
 *
 * Totally public domain.
 */

/* Walk the nodes of a tree */
static void twalk_recurse(const node_t *root, void(*action) (const void *, VISIT, int), int level)
{
	assert(root != NULL);
	assert(action != NULL);

	if (root->llink == NULL && root->rlink == NULL)
		(*action)(root, leaf, level);
	else {
		(*action)(root, preorder, level);
		if (root->llink != NULL)
			twalk_recurse(root->llink, action, level + 1);
		(*action)(root, postorder, level);
		if (root->rlink != NULL)
			twalk_recurse(root->rlink, action, level + 1);
		(*action)(root, endorder, level);
	}
}

/* Walk the nodes of a tree */
void twalk(const void *vroot, void(*action) (const void *, VISIT, int))
{
	if (vroot != NULL && action != NULL)
		twalk_recurse((const node_t *)vroot, action, 0);
}

//#endif // HAVE_SEARCH_H