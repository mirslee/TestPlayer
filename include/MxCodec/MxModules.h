/*****************************************************************************
 * mx_modules.h : Module descriptor and load functions
 *****************************************************************************
 * Copyright (C) 2001-2011 VLC authors and VideoLAN
 * $Id: c2d3c26d20c3c45529bc01afb40377560cdf7306 $
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

#ifndef MXMODULES_H
#define MXMODULES_H 1

#include "CMxObject.h"
/**
 * \file
 * This file defines functions for modules in vlc
 */

typedef int (*mx_activate_t)(void *func, va_list args);
typedef void (*mx_deactivate_t)(void *func, va_list args);

/*****************************************************************************
 * Exported functions.
 *****************************************************************************/
struct module_t;
struct module_config_t;
MXCORE_API module_t * mx_module_load( CMxObject *obj, const char *cap, const char *name, bool strict, mx_activate_t probe, ... );
#define mx_module_load(o,c,n,s,...) \
        mx_module_load(MX_OBJECT(o),c,n,s,__VA_ARGS__)
MXCORE_API void mx_module_unload( CMxObject *obj, module_t *,
                                mx_deactivate_t deinit, ... );
#define mx_module_unload(o,m,d,...) \
        mx_module_unload(MX_OBJECT(o),m,d,__VA_ARGS__)

MXCORE_API module_t * module_need( CMxObject *, const char *, const char *, bool ) ;
#define module_need(a,b,c,d) module_need(MX_OBJECT(a),b,c,d)
MXCORE_API void module_unneed( CMxObject *, module_t * );
#define module_unneed(a,b) module_unneed(MX_OBJECT(a),b)
MXCORE_API bool module_exists(const char *) ;
MXCORE_API module_t * module_find(const char *) ;

int module_start(CMxObject *, const module_t *);
#define module_start(o, m) module_start(MX_OBJECT(o),m)
void module_stop(CMxObject *, const module_t *);
#define module_stop(o, m) module_stop(MX_OBJECT(o),m)

/*MXCORE_API module_config_t * module_config_get( const module_t *, unsigned * ) MX_USED;
MXCORE_API void module_config_free( module_config_t * );*/

MXCORE_API void module_list_free(module_t **);
MXCORE_API module_t ** module_list_get(size_t *n) ;

MXCORE_API bool module_provides( const module_t *m, const char *cap );
MXCORE_API const char * module_get_object( const module_t *m ) ;
MXCORE_API const char * module_get_name( const module_t *m, bool long_name ) ;
#define module_GetLongName( m ) module_get_name( m, true )
MXCORE_API const char * module_get_help( const module_t *m ) ;
MXCORE_API const char * module_get_capability( const module_t *m ) ;
MXCORE_API int module_get_score( const module_t *m ) ;
MXCORE_API const char * module_gettext( const module_t *, const char * ) ;

 static inline module_t *module_get_main (void)
{
    return module_find ("core");
}
#define module_get_main(a) module_get_main()

 static inline bool module_is_main( const module_t * p_module )
{
    return !strcmp( module_get_object( p_module ), "core" );
}

#endif /* MXMODULES_H */
