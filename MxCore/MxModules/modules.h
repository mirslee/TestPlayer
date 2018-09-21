/*****************************************************************************
 * modules.h : Module management functions.
 *****************************************************************************
 * Copyright (C) 2001-2016 VLC authors and VideoLAN
 * $Id: 60a36e7d4909412e41cda62952283663d3331fed $
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

#ifndef MODULES_H
#define MODULES_H 1

# include "MxAtomic.h"
#include "CMxBlock.h"

/** The plugin handle type */
typedef void *module_handle_t;

/** VLC plugin */
typedef struct mx_plugin_t
{
    struct mx_plugin_t *next;
    module_t *module;
    unsigned modules_count;

    const char *textdomain; /**< gettext domain (or NULL) */

    /**
     * Variables set by the module to store its config options
     */
    struct
    {
        module_config_t *items; /**< Table of configuration parameters */
        size_t size; /**< Size of items table */
        size_t count; /**< Number of configuration items */
        size_t booleans; /**< Number of booleal config items */
    } conf;

#ifdef HAVE_DYNAMIC_PLUGINS
    atomic_bool loaded; /**< Whether the plug-in is mapped in memory */
    bool unloadable; /**< Whether the plug-in can be unloaded safely */
    module_handle_t handle; /**< Run-time linker handle (if loaded) */
    char *abspath; /**< Absolute path */

    char *path; /**< Relative path (within plug-in directory) */
    int64_t mtime; /**< Last modification time */
    uint64_t size; /**< File size */
#endif
} mx_plugin_t;

/**
 * List of all plug-ins.
 */
extern struct mx_plugin_t *mx_plugins;

#define MODULE_SHORTCUT_MAX 20

/** Plugin entry point prototype */
typedef int (*mx_plugin_cb) (int (*)(void *, void *, int, ...), void *);

/** Core module */
int mx_entry__core (int (*)(void *, void *, int, ...), void *);

/**
 * Internal module descriptor
 */
struct module_t
{
    mx_plugin_t *plugin; /**< Plug-in/library containing the module */
    module_t   *next;

    /** Shortcuts to the module */
    unsigned    i_shortcuts;
    const char **pp_shortcuts;

    /*
     * Variables set by the module to identify itself
     */
    const char *psz_shortname;                              /**< Module name */
    const char *psz_longname;                   /**< Module descriptive name */
    const char *psz_help;        /**< Long help string for "special" modules */

    const char *psz_capability;                              /**< Capability */
    int      i_score;                          /**< Score for the capability */

    /* Callbacks */
    const char *activate_name;
    const char *deactivate_name;
    void *pf_activate;
    void *pf_deactivate;
};

mx_plugin_t *mx_plugin_create(void);
void mx_plugin_destroy(mx_plugin_t *);
module_t *mx_module_create(mx_plugin_t *);
void mx_module_destroy (module_t *);

mx_plugin_t *mx_plugin_describe(mx_plugin_cb);
int mx_plugin_resolve(mx_plugin_t *, mx_plugin_cb);

void module_InitBank (void);
size_t module_LoadPlugins( CMxObject * );
#define module_LoadPlugins(a) module_LoadPlugins(MX_OBJECT(a))
void module_EndBank (bool);
int module_Map(CMxObject *, mx_plugin_t *);

ssize_t module_list_cap (module_t ***, const char *);

int mx_bindtextdomain (const char *);

/* Low-level OS-dependent handler */
int module_Load (CMxObject *, const char *, module_handle_t *, bool);
void *module_Lookup (module_handle_t, const char *);
void module_Unload (module_handle_t);

/* Plugins cache */
mx_plugin_t *mx_cache_load(CMxObject *, const char *, CMxBlock **);
mx_plugin_t *mx_cache_lookup(mx_plugin_t **, const char *relpath);

void CacheSave(CMxObject *, const char *, mx_plugin_t *const *, size_t);

#endif /* !MODULES_H */
