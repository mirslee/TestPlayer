/*****************************************************************************
 * modules.c : Builtin and plugin modules management functions
 *****************************************************************************/
#include "stdafx.h"
#include "MxConfig.h"
#include "MxCommon.h"
#include "MxModules.h"
#include "MxError.h"
#include "MxSystem/MxFixups.h"
#include "MxVariables.h"
#include <stdlib.h>
#include <string.h>
#ifdef ENABLE_NLS
# include <libintl.h>
#endif
#include <assert.h>

//#include "libvlc.h"
//#include "config/configuration.h"
//#include "vlc_arrays.h"
#include "modules.h"

/**
 * Checks whether a module implements a capability.
 *
 * \param m the module
 * \param cap the capability to check
 * \return true if the module has the capability
 */
bool module_provides (const module_t *m, const char *cap)
{
    return !strcmp (module_get_capability (m), cap);
}

/**
 * Get the internal name of a module
 *
 * \param m the module
 * \return the module name
 */
const char *module_get_object( const module_t *m )
{
    if (unlikely(m->i_shortcuts == 0))
        return "unnamed";
    return m->pp_shortcuts[0];
}

/**
 * Get the human-friendly name of a module.
 *
 * \param m the module
 * \param long_name TRUE to have the long name of the module
 * \return the short or long name of the module
 */
const char *module_get_name( const module_t *m, bool long_name )
{
    if( long_name && ( m->psz_longname != NULL) )
        return m->psz_longname;

    if (m->psz_shortname != NULL)
        return m->psz_shortname;
    return module_get_object (m);
}

/**
 * Get the help for a module
 *
 * \param m the module
 * \return the help
 */
const char *module_get_help( const module_t *m )
{
    return m->psz_help;
}

/**
 * Gets the capability of a module
 *
 * \param m the module
 * \return the capability, or "none" if unspecified
 */
const char *module_get_capability (const module_t *m)
{
    return (m->psz_capability != NULL) ? m->psz_capability : "none";
}

/**
 * Get the score for a module
 *
 * \param m the module
 * return the score for the capability
 */
int module_get_score( const module_t *m )
{
    return m->i_score;
}

/**
 * Translate a string using the module's text domain
 *
 * \param m the module
 * \param str the American English ASCII string to localize
 * \return the gettext-translated string
 */
const char *module_gettext (const module_t *m, const char *str)
{
    if (unlikely(str == NULL || *str == '\0'))
        return "";
#ifdef ENABLE_NLS
    const char *domain = m->plugin->textdomain;
    return dgettext ((domain != NULL) ? domain : PACKAGE_NAME, str);
#else
    (void)m;
    return str;
#endif
}

#undef module_start
typedef int (*activateFun) (CMxObject *);
int module_start (CMxObject *obj, const module_t *m)
{
   int (*activate) (CMxObject *) = (activateFun)m->pf_activate;

   return (activate != NULL) ? activate (obj) : MX_SUCCESS;
}

#undef module_stop
typedef void (*deactivateFun) (CMxObject *);
void module_stop (CMxObject *obj, const module_t *m)
{
   void (*deactivate) (CMxObject *) = (deactivateFun)m->pf_deactivate;

    if (deactivate != NULL)
        deactivate (obj);
}

static bool module_match_name (const module_t *m, const char *name)
{
     /* Plugins with zero score must be matched explicitly. */
     if (!strcasecmp ("any", name))
         return m->i_score > 0;

     for (unsigned i = 0; i < m->i_shortcuts; i++)
          if (!strcasecmp (m->pp_shortcuts[i], name))
              return true;
     return false;
}

static int module_load (CMxObject *obj, module_t *m,
                        mx_activate_t init, va_list args)
{
    int ret = MX_SUCCESS;

    if (module_Map(obj, m->plugin))
        return MX_EGENERIC;

    if (m->pf_activate != NULL)
    {
        va_list ap;

        va_copy (ap, args);
        ret = init (m->pf_activate, ap);
        va_end (ap);
    }

    if (ret != MX_SUCCESS)
        mx_objres_clear(obj);

    return ret;
}

#undef mx_module_load
/**
 * Finds and instantiates the best module of a certain type.
 * All candidates modules having the specified capability and name will be
 * sorted in decreasing order of priority. Then the probe callback will be
 * invoked for each module, until it succeeds (returns 0), or all candidate
 * module failed to initialize.
 *
 * The probe callback first parameter is the address of the module entry point.
 * Further parameters are passed as an argument list; it corresponds to the
 * variable arguments passed to this function. This scheme is meant to
 * support arbitrary prototypes for the module entry point.
 *
 * \param obj VLC object
 * \param capability capability, i.e. class of module
 * \param name name of the module asked, if any
 * \param strict if true, do not fallback to plugin with a different name
 *                 but the same capability
 * \param probe module probe callback
 * \return the module or NULL in case of a failure
 */
module_t *mx_module_load(CMxObject *obj, const char *capability,
                          const char *name, bool strict,
                          mx_activate_t probe, ...)
{
    char *var = NULL;

    if (name == NULL || name[0] == '\0')
        name = "any";

    /* Deal with variables */
    if (name[0] == '$')
    {
        var = var_InheritString(obj, name + 1);
        name = (var != NULL) ? var : "any";
    }

    /* Find matching modules */
    module_t **mods;
    ssize_t total = module_list_cap (&mods, capability);

    /*msg_Dbg (obj, "looking for %s module matching \"%s\": %zd candidates",
             capability, name, total);*/
    if (total <= 0)
    {
        //module_list_free (mods);
		free(mods);
        //msg_Dbg (obj, "no %s modules", capability);
        return NULL;
    }

    module_t *module = NULL;
    const bool b_force_backup = obj->obj.force; /* FIXME: remove this */
    va_list args;

    va_start(args, probe);
    while (*name)
    {
        char buf[32];
        size_t slen = strcspn (name, ",");

        if (likely(slen < sizeof (buf)))
        {
            memcpy(buf, name, slen);
            buf[slen] = '\0';
        }
        name += slen;
        name += strspn (name, ",");
        if (unlikely(slen >= sizeof (buf)))
            continue;

        const char *shortcut = buf;
        assert (shortcut != NULL);

        if (!strcasecmp ("none", shortcut))
            goto done;

        obj->obj.force = strict && strcasecmp ("any", shortcut);
        for (ssize_t i = 0; i < total; i++)
        {
            module_t *cand = mods[i];
            if (cand == NULL)
                continue; // module failed in previous iteration
            if (!module_match_name (cand, shortcut))
                continue;
            mods[i] = NULL; // only try each module once at most...

            int ret = module_load (obj, cand, probe, args);
            switch (ret)
            {
                case MX_SUCCESS:
                    module = cand;
                    /* fall through */
                case MX_ETIMEOUT:
                    goto done;
            }
        }
    }

    /* None of the shortcuts matched, fall back to any module */
    if (!strict)
    {
        obj->obj.force = false;
        for (ssize_t i = 0; i < total; i++)
        {
            module_t *cand = mods[i];
            if (cand == NULL || module_get_score (cand) <= 0)
                continue;

            int ret = module_load (obj, cand, probe, args);
            switch (ret)
            {
                case MX_SUCCESS:
                    module = cand;
                    /* fall through */
                case MX_ETIMEOUT:
                    goto done;
            }
        }
    }
done:
    va_end (args);
    obj->obj.force = b_force_backup;
    module_list_free (mods);
    free (var);

    if (module != NULL)
    {
        /*msg_Dbg (obj, "using %s module \"%s\"", capability,
                 module_get_object (module));*/
        mx_object_set_name (obj, module_get_object (module));
    }
    else
        /*msg_Dbg (obj, "no %s modules matched", capability)*/;
    return module;
}

#undef mx_module_unload
/**
 * Deinstantiates a module.
 * \param module the module pointer as returned by mx_module_load()
 * \param deinit deactivation callback
 */
void mx_module_unload(CMxObject *obj, module_t *module,
                       mx_deactivate_t deinit, ...)
{
    if (module->pf_deactivate != NULL)
    {
        va_list ap;

        va_start(ap, deinit);
        deinit(module->pf_deactivate, ap);
        va_end(ap);
    }

    mx_objres_clear(obj);
}

typedef int (*activateFun1)(CMxObject *);
static int generic_start(void *func, va_list ap)
{
    CMxObject *obj = va_arg(ap, CMxObject *);
    int (*activate)(CMxObject *) = (activateFun1)func;

    return activate(obj);
}

typedef void (*deactivateFun1)(CMxObject *);
static void generic_stop(void *func, va_list ap)
{
    CMxObject *obj = va_arg(ap, CMxObject *);
    void (*deactivate)(CMxObject *) = (deactivateFun1)func;

    deactivate(obj);
}

#undef module_need
module_t *module_need(CMxObject *obj, const char *cap, const char *name,
                      bool strict)
{
    return mx_module_load(obj, cap, name, strict, generic_start, obj);
}

#undef module_unneed
void module_unneed(CMxObject *obj, module_t *module)
{
    //msg_Dbg(obj, "removing module \"%s\"", module_get_object(module));
    mx_module_unload(obj, module, generic_stop, obj);
}

/**
 * Get a pointer to a module_t given it's name.
 *
 * \param name the name of the module
 * \return a pointer to the module or NULL in case of a failure
 */
module_t *module_find (const char *name)
{
    size_t count;
    module_t **list = module_list_get (&count);

    assert (name != NULL);

    for (size_t i = 0; i < count; i++)
    {
        module_t *module = list[i];

        if (unlikely(module->i_shortcuts == 0))
            continue;
        if (!strcmp (module->pp_shortcuts[0], name))
        {
            module_list_free (list);
            return module;
        }
    }
    module_list_free (list);
    return NULL;
}

/**
 * Tell if a module exists
 *
 * \param psz_name th name of the module
 * \return TRUE if the module exists
 */
bool module_exists (const char * psz_name)
{
    return module_find (psz_name) != NULL;
}

/**
 * Get the configuration of a module
 *
 * \param module the module
 * \param psize the size of the configuration returned
 * \return the configuration as an array
 */
//module_config_t *module_config_get( const module_t *module, unsigned *psize )
//{
//    const mx_plugin_t *plugin = module->plugin;
//
//    if (plugin->module != module)
//    {   /* For backward compatibility, pretend non-first modules have no
//         * configuration items. */
//        *psize = 0;
//        return NULL;
//    }
//
//    unsigned i,j;
//    size_t size = plugin->conf.size;
//    module_config_t *config = vlc_alloc( size, sizeof( *config ) );
//
//    assert( psize != NULL );
//    *psize = 0;
//
//    if( !config )
//        return NULL;
//
//    for( i = 0, j = 0; i < size; i++ )
//    {
//        const module_config_t *item = plugin->conf.items + i;
//        if( item->b_internal /* internal option */
//         || item->b_removed /* removed option */ )
//            continue;
//
//        memcpy( config + j, item, sizeof( *config ) );
//        j++;
//    }
//    *psize = j;
//
//    return config;
//}
//
///**
// * Release the configuration
// *
// * \param the configuration
// * \return nothing
// */
//void module_config_free( module_config_t *config )
//{
//    free( config );
//}
