/*****************************************************************************
 * objres.c: CMxObject resources
 *****************************************************************************
 * Copyright (C) 2017 Rémi Denis-Courmont
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
#include "stdafx.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>

#include "MxCommon.h"

#include "variables.h"

struct mx_res
{
    struct mx_res *prev;
    void (*release)(void *);
    max_align_t payload[];
};

static struct mx_res **mx_obj_res(CMxObject *obj)
{
    return &mx_internals(obj)->resources;
}

void *mx_objres_new(size_t size, void (*release)(void *))
{
    if (unlikely(add_overflow(sizeof (struct mx_res), size, &size)))
    {
        errno = ENOMEM;
        return NULL;
    }

    struct mx_res *res = (mx_res*)malloc(size);
    if (unlikely(res == NULL))
        return NULL;

    res->release = release;
    return res->payload;
}

void mx_objres_push(CMxObject *obj, void *data)
{
    struct mx_res ** pp = (mx_res **)mx_obj_res(obj);
    struct mx_res *res = (mx_res*)container_of(data, struct mx_res, payload);

    res->prev = *pp;
    *pp = res;
}

static void *mx_objres_pop(CMxObject *obj)
{
    struct mx_res ** pp = (mx_res**)mx_obj_res(obj);
    struct mx_res *res = (mx_res*)*pp;

    if (res == NULL)
        return NULL;
    *pp = res->prev;
    return res->payload;
}

void mx_objres_clear(CMxObject *obj)
{
    void *data;

    while ((data = mx_objres_pop(obj)) != NULL)
    {
        struct mx_res *res = (mx_res*)container_of(data, struct mx_res, payload);

        res->release(res->payload);
        free(res);
    }
}

void mx_objres_remove(CMxObject *obj, void *data,
                       bool (*match)(void *, void *))
{
    struct mx_res ** pp = (mx_res **)mx_obj_res(obj);

    /* With a doubly-linked list, this function could have constant complexity.
     * But that would require one more pointer per resource.
     *
     * Any given list should contain a fairly small number of resources,
     * and in most cases, the resources are destroyed implicitly by
     * mx_objres_clear().
     */
    for (;;)
    {
        struct mx_res *res = *pp;

        assert(res != NULL); /* invalid free? */

        if (match(res->payload, data))
        {
            *pp = res->prev;
            res->release(res->payload);
            free(res);
            return;
        }

        pp = &res->prev;
    }
}

static void dummy_release(void *data)
{
    (void) data;
}

static bool ptrcmp(void *a, void *b)
{
    return a == b;
}

void *mx_obj_malloc(CMxObject *obj, size_t size)
{
    void *ptr = mx_objres_new(size, dummy_release);
    if (likely(ptr != NULL))
        mx_objres_push(obj, ptr);
    return ptr;
}

static void *mx_obj_alloc_common(CMxObject *obj, size_t nmemb, size_t size,
                                  bool do_memset)
{
    size_t tabsize;
    if (mul_overflow(nmemb, size, &tabsize))
    {
        errno = ENOMEM;
        return NULL;
    }

    void *ptr = mx_objres_new(tabsize, dummy_release);
    if (likely(ptr != NULL))
    {
        if (do_memset)
            memset(ptr, 0, tabsize);
        mx_objres_push(obj, ptr);
    }
    return ptr;
}

void *mx_obj_calloc(CMxObject *obj, size_t nmemb, size_t size)
{
    return mx_obj_alloc_common(obj, nmemb, size, true);
}

void mx_obj_free(CMxObject *obj, void *ptr)
{
    mx_objres_remove(obj, ptr, ptrcmp);
}
