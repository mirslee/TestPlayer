/*****************************************************************************
 * variables.h: object variables typedefs
 *****************************************************************************
 * Copyright (C) 1999-2012 VLC authors and VideoLAN
 *
 * Authors: Samuel Hocevar <sam@zoy.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef LIBVLC_VARIABLES_H
#define LIBVLC_VARIABLES_H 1

//#include <stdalign.h>
#include "MxSystem/MxAtomic.h"
#include "MxSystem/MxThread.h"

struct mx_res;

/**
 * Private LibVLC data for each object.
 */
typedef struct mx_object_internals mx_object_internals_t;

struct mx_object_internals
{
    alignas (max_align_t) /* ensure mx_externals() is maximally aligned */
    char           *psz_name; /* given name */

    /* Object variables */
    void           *var_root;
    MxMutex     var_lock;
    MxCond      var_wait;

    /* Objects management */
    atomic_uint     refs;
    mx_destructor_t pf_destructor;

    /* Objects tree structure */
    mx_object_internals_t *next;  /* next sibling */
    mx_object_internals_t *prev;  /* previous sibling */
    mx_object_internals_t *first; /* first child */
	MxMutex tree_lock;

    /* Object resources */
    struct mx_res *resources;
};

# define mx_internals( obj ) (((mx_object_internals_t*)(MX_OBJECT(obj)))-1)
# define mx_externals( priv ) ((CMxObject *)((priv) + 1))

void DumpVariables(CMxObject *obj);

extern void var_DestroyAll(CMxObject * );

/**
 * Return a list of all variable names
 *
 * There is no warranty that the returned variables will be still alive after
 * the return of this function.
 *
 * @return a NULL terminated list of char *, each elements and the return value
 * must be freed by the caller
 */
char **var_GetAllNames(CMxObject *);

#endif
