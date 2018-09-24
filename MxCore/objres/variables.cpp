/*****************************************************************************
 * variables.c: routines for object variables handling
 *****************************************************************************
 * Copyright (C) 2002-2009 VLC authors and VideoLAN
 * $Id: 810c8ffc819365cce4c6889885055315d9fcb337 $
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

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#include "stdafx.h"

#ifdef HAVE_SEARCH_H
# include <search.h>
#endif
#include <assert.h>
#include <float.h>
#include <math.h>
#include <limits.h>

#include "MxCommon.h"
#include "MxCharSet.h"
#include "variables.h"
#include "MxFixups.h"
#include "MxVariables.h"
#include "MxError.h"
#include "MxThread.h"
//#include "config/configuration.h"

typedef struct callback_entry_t
{
    union
    {
        mx_callback_t       pf_value_callback;
        mx_list_callback_t  pf_list_callback;
        void *               p_callback;
    };
    void *         p_data;
} callback_entry_t;

typedef struct variable_ops_t
{
    int  (*pf_cmp) ( mx_value_t, mx_value_t );
    void (*pf_dup) ( mx_value_t * );
    void (*pf_free) ( mx_value_t * );
} variable_ops_t;

typedef struct callback_table_t
{
    /*int                i_entries;
    callback_entry_t * p_entries;*/
	std::vector<callback_entry_t> p_entries;
} callback_table_t;

/**
 * The structure describing a variable.
 * \note mx_value_t is the common union for variable values
 */
struct variable_t
{
    char *       psz_name; /**< The variable unique name (must be first) */

    /** The variable's exported value */
    mx_value_t  val;

    /** The variable display name, mainly for use by the interfaces */
    char *       psz_text;

    const variable_ops_t *ops;

    int          i_type;   /**< The type of the variable */
    unsigned     i_usage;  /**< Reference count */

    /** If the variable has min/max/step values */
    mx_value_t  min, max, step;

    /** List of choices */
    mx_list_t   choices;
    /** List of friendly names for the choices */
    mx_list_t   choices_text;

    /** Set to TRUE if the variable is in a callback */
    bool   b_incallback;

    /** Registered value callbacks */
    callback_table_t    value_callbacks;
    /** Registered list callbacks */
    callback_table_t    list_callbacks;
};

static int CmpBool( mx_value_t v, mx_value_t w )
{
    return v.b_bool ? w.b_bool ? 0 : 1 : w.b_bool ? -1 : 0;
}

static int CmpInt( mx_value_t v, mx_value_t w )
{
    return v.i_int == w.i_int ? 0 : v.i_int > w.i_int ? 1 : -1;
}

static int CmpString( mx_value_t v, mx_value_t w )
{
    if( !v.psz_string )
        return !w.psz_string ? 0 : -1;
    else
        return !w.psz_string ? 1 : strcmp( v.psz_string, w.psz_string );
}
static int CmpFloat( mx_value_t v, mx_value_t w ) { return v.f_float == w.f_float ? 0 : v.f_float > w.f_float ? 1 : -1; }
static int CmpAddress( mx_value_t v, mx_value_t w ) { return v.p_address == w.p_address ? 0 : v.p_address > w.p_address ? 1 : -1; }

static void DupDummy( mx_value_t *p_val ) { (void)p_val; /* unused */ }
static void DupString( mx_value_t *p_val )
{
    p_val->psz_string = strdup( p_val->psz_string ? p_val->psz_string :  "" );
}

static void FreeDummy( mx_value_t *p_val ) { (void)p_val; /* unused */ }
static void FreeString( mx_value_t *p_val ) { free( p_val->psz_string ); }

static const struct variable_ops_t
void_ops   = { NULL,       DupDummy,  FreeDummy,  },
addr_ops   = { CmpAddress, DupDummy,  FreeDummy,  },
bool_ops   = { CmpBool,    DupDummy,  FreeDummy,  },
float_ops  = { CmpFloat,   DupDummy,  FreeDummy,  },
int_ops    = { CmpInt,     DupDummy,  FreeDummy,  },
string_ops = { CmpString,  DupString, FreeString, },
coords_ops = { NULL,       DupDummy,  FreeDummy,  };

static int varcmp( const void *a, const void *b )
{
    const variable_t *va = (const variable_t *)a, *vb = (const variable_t *)b;

    /* psz_name must be first */
    assert( va == (const void *)&va->psz_name );
    return strcmp( va->psz_name, vb->psz_name );
}

static variable_t *Lookup( CMxObject *obj, const char *psz_name )
{
    mx_object_internals_t *priv = mx_internals( obj );
    variable_t **pp_var;

    mxMutexLock(&priv->var_lock);
    pp_var = (variable_t **)tfind( &psz_name, (const void **)&priv->var_root, varcmp );
    return (pp_var != NULL) ? *pp_var : NULL;
}

static void Destroy( variable_t *p_var )
{
    p_var->ops->pf_free( &p_var->val );
    if( /*p_var->choices.i_count*/ p_var->choices.p_values.size() > 0 )
    {
        for( int i = 0 ; i < /*p_var->choices.i_count*/p_var->choices.p_values.size(); i++ )
        {
            p_var->ops->pf_free( (mx_value_t *)&p_var->choices.p_values[i] );
            free( p_var->choices_text.p_values[i].psz_string );
        }
        //free( p_var->choices.p_values );
        //free( p_var->choices_text.p_values );
    }

    free( p_var->psz_name );
    free( p_var->psz_text );
    //free( p_var->value_callbacks.p_entries );
    free( p_var );
}

/**
 * Adjusts a value to fit the constraints for a certain variable:
 * - If the value is lower than the minimum, use the minimum.
 * - If the value is higher than the maximum, use the maximum.
 * - If the variable has steps, round the value to the nearest step.
 */
static void CheckValue(variable_t *var, mx_value_t *val)
{
    /* Check that our variable is within the bounds */
    switch (var->i_type & MX_VAR_TYPE)
    {
        case MX_VAR_INTEGER:
            if (val->i_int < var->min.i_int)
               val->i_int = var->min.i_int;
            if (val->i_int > var->max.i_int)
                val->i_int = var->max.i_int;
            if (var->step.i_int != 0 && (val->i_int % var->step.i_int))
            {
                if (val->i_int > 0)
                    val->i_int = (val->i_int + (var->step.i_int / 2))
                                 / var->step.i_int * var->step.i_int;
                else
                    val->i_int = (val->i_int - (var->step.i_int / 2))
                                 / var->step.i_int * var->step.i_int;
            }
            break;

        case MX_VAR_FLOAT:
            if (isless(val->f_float, var->min.f_float))
                val->f_float = var->min.f_float;
            if (isgreater(val->f_float, var->max.f_float))
                val->f_float = var->max.f_float;
            if (var->step.f_float != 0.f)
                val->f_float = var->step.f_float
                              * roundf(val->f_float / var->step.f_float);
            break;
    }
}

/**
 * Waits until the variable is inactive (i.e. not executing a callback)
 */
static void WaitUnused(CMxObject *obj, variable_t *var)
{
    mx_object_internals_t *priv = mx_internals(obj);

    mutex_cleanup_push(&priv->var_lock);
    while (var->b_incallback)
		mxCondWait(&priv->var_wait, &priv->var_lock);
    mxCleanupPop();
}

static void TriggerCallback(CMxObject *obj, variable_t *var,
                            const char *name, mx_value_t prev)
{
    assert(obj != NULL);

    size_t count = /*var->value_callbacks.i_entries*/var->value_callbacks.p_entries.size();
    if (count == 0)
        return;

    /*callback_entry_t *entries*/std::vector<callback_entry_t>&entries = var->value_callbacks.p_entries;
    mx_object_internals_t *priv = mx_internals(obj);

    assert(!var->b_incallback);
    var->b_incallback = true;
    mxMutexUnlock(&priv->var_lock);

    for (size_t i = 0; i < count; i++)
        entries[i].pf_value_callback(obj, name, prev, var->val,
                                     entries[i].p_data);

    mxMutexLock(&priv->var_lock);
    var->b_incallback = false;
	mxCondBroadcast(&priv->var_wait);
}

static void TriggerListCallback(CMxObject *obj, variable_t *var,
                                const char *name, int action, mx_value_t *val)
{
    assert(obj != NULL);

    size_t count = /*var->list_callbacks.i_entries*/var->value_callbacks.p_entries.size();
    if (count == 0)
        return;

    /*callback_entry_t *entries*/std::vector<callback_entry_t>&entries = var->list_callbacks.p_entries;
    mx_object_internals_t *priv = mx_internals(obj);

    assert(!var->b_incallback);
    var->b_incallback = true;
    mxMutexUnlock(&priv->var_lock);

    for (size_t i = 0; i < count; i++)
        entries[i].pf_list_callback(obj, name, action, val,
                                      entries[i].p_data);

    mxMutexLock(&priv->var_lock);
    var->b_incallback = false;
	mxCondBroadcast(&priv->var_wait);
}

#undef var_Create
/**
 * Initialize a vlc variable
 *
 * We hash the given string and insert it into the sorted list. The insertion
 * may require slow memory copies, but think about what we gain in the log(n)
 * lookup phase when setting/getting the variable value!
 *
 * \param p_this The object in which to create the variable
 * \param psz_name The name of the variable
 * \param i_type The variables type. Must be one of \ref var_type combined with
 *               zero or more \ref var_flags
 */
int var_Create( CMxObject *p_this, const char *psz_name, int i_type )
{
    assert( p_this );

    variable_t *p_var = (variable_t *)calloc( 1, sizeof( *p_var ) );
    if( p_var == NULL )
        return MX_ENOMEM;

    p_var->psz_name = strdup( psz_name );
    p_var->psz_text = NULL;

    p_var->i_type = i_type & ~MX_VAR_DOINHERIT;

    p_var->i_usage = 1;

    //p_var->choices.i_count = 0;
    //p_var->choices.p_values = NULL;
    //p_var->choices_text.i_count = 0;
    //p_var->choices_text.p_values = NULL;

    p_var->b_incallback = false;

	//struct callback_table_t aaa = { 0, NULL };
    //p_var->value_callbacks = aaa;

    /* Always initialize the variable, even if it is a list variable; this
     * will lead to errors if the variable is not initialized, but it will
     * not cause crashes in the variable handling. */
    switch( i_type & MX_VAR_CLASS )
    {
        case MX_VAR_BOOL:
            p_var->ops = &bool_ops;
            p_var->val.b_bool = false;
            break;
        case MX_VAR_INTEGER:
            p_var->ops = &int_ops;
            p_var->val.i_int = 0;
            p_var->min.i_int = INT64_MIN;
            p_var->max.i_int = INT64_MAX;
            break;
        case MX_VAR_STRING:
            p_var->ops = &string_ops;
            p_var->val.psz_string = NULL;
            break;
        case MX_VAR_FLOAT:
            p_var->ops = &float_ops;
            p_var->val.f_float = 0.f;
            p_var->min.f_float = -FLT_MAX;
            p_var->max.f_float = FLT_MAX;
            break;
        case MX_VAR_COORDS:
            p_var->ops = &coords_ops;
            p_var->val.coords.x = p_var->val.coords.y = 0;
            break;
        case MX_VAR_ADDRESS:
            p_var->ops = &addr_ops;
            p_var->val.p_address = NULL;
            break;
        case MX_VAR_VOID:
            p_var->ops = &void_ops;
            break;
        default:
			MX_assert_unreachable ();
    }

    if (i_type & MX_VAR_DOINHERIT)
        var_Inherit(p_this, psz_name, i_type, &p_var->val);

    mx_object_internals_t *p_priv = mx_internals( p_this );
    variable_t **pp_var, *p_oldvar;
    int ret = MX_SUCCESS;

	mxMutexLock( &p_priv->var_lock );

    pp_var = (variable_t **)tsearch( p_var, &p_priv->var_root, varcmp );
    if( unlikely(pp_var == NULL) )
        ret = MX_ENOMEM;
    else if( (p_oldvar = *pp_var) == p_var ) /* Variable create */
        p_var = NULL; /* Variable created */
    else /* Variable already exists */
    {
        assert (((i_type ^ p_oldvar->i_type) & MX_VAR_CLASS) == 0);
        p_oldvar->i_usage++;
        p_oldvar->i_type |= i_type & MX_VAR_ISCOMMAND;
    }
	mxMutexUnlock( &p_priv->var_lock );

    /* If we did not need to create a new variable, free everything... */
    if( p_var != NULL )
        Destroy( p_var );
    return ret;
}

/**
 * Destroy a vlc variable
 *
 * Look for the variable and destroy it if it is found. As in var_Create we
 * do a call to memmove() but we have performance counterparts elsewhere.
 *
 * \param p_this The object that holds the variable
 * \param psz_name The name of the variable
 */
void (var_Destroy)(CMxObject *p_this, const char *psz_name)
{
    variable_t *p_var;

    assert( p_this );

    mx_object_internals_t *p_priv = mx_internals( p_this );

    p_var = Lookup( p_this, psz_name );
    if( p_var == NULL )
        /*msg_Dbg( p_this, "attempt to destroy nonexistent variable \"%s\"",
                 psz_name )*/;
    else if( --p_var->i_usage == 0 )
    {
        assert(!p_var->b_incallback);
        tdelete( p_var, &p_priv->var_root, varcmp );
    }
    else
    {
        assert(p_var->i_usage != -1u);
        p_var = NULL;
    }
    mxMutexUnlock( &p_priv->var_lock );

    if( p_var != NULL )
        Destroy( p_var );
}

static void CleanupVar( void *var )
{
    Destroy( (variable_t *)var );
}

void var_DestroyAll( CMxObject *obj )
{
    mx_object_internals_t *priv = mx_internals( obj );

    tdestroy( priv->var_root, CleanupVar );
    priv->var_root = NULL;
}

#undef var_Change
/**
 * Perform an action on a variable
 *
 * \param p_this The object that holds the variable
 * \param psz_name The name of the variable
 * \param i_action The action to perform. Must be one of \ref var_action
 * \param p_val First action parameter
 * \param p_val2 Second action parameter
 */
int var_Change( CMxObject *p_this, const char *psz_name,
                int i_action, mx_value_t *p_val, mx_value_t *p_val2 )
{
    int ret = MX_SUCCESS;
    variable_t *p_var;
    mx_value_t oldval;
    mx_value_t newval;

    assert( p_this );

    mx_object_internals_t *p_priv = mx_internals( p_this );

    p_var = Lookup( p_this, psz_name );
    if( p_var == NULL )
    {
        mxMutexUnlock( &p_priv->var_lock );
        return MX_ENOVAR;
    }

    switch( i_action )
    {
        case MX_VAR_GETMIN:
            *p_val = p_var->min;
            break;
        case MX_VAR_GETMAX:
            *p_val = p_var->max;
            break;
        case MX_VAR_SETMINMAX:
            assert(p_var->ops->pf_free == FreeDummy);
            p_var->min = *p_val;
            p_var->max = *p_val2;
            break;
        case MX_VAR_SETSTEP:
            assert(p_var->ops->pf_free == FreeDummy);
            p_var->step = *p_val;
            CheckValue( p_var, &p_var->val );
            break;
        case MX_VAR_GETSTEP:
            switch (p_var->i_type & MX_VAR_TYPE)
            {
                case MX_VAR_INTEGER:
                    if (p_var->step.i_int == 0)
                        ret = MX_EGENERIC;
                    break;
                case MX_VAR_FLOAT:
                    if (p_var->step.f_float == 0.f)
                        ret = MX_EGENERIC;
                    break;
                default:
                    ret = MX_EGENERIC;
            }
            if (ret == MX_SUCCESS)
                *p_val = p_var->step;
            break;
        case MX_VAR_ADDCHOICE:
        {
            int i = /*p_var->choices.i_count*/p_var->choices.p_values.size();

            /*TAB_APPEND(p_var->choices.i_count,
                       p_var->choices.p_values, *p_val);
            assert(i == p_var->choices_text.i_count);
            TAB_APPEND(p_var->choices_text.i_count,
                       p_var->choices_text.p_values, *p_val);*/
			p_var->choices.p_values.push_back(*p_val);
			assert(i == p_var->choices_text.p_values.size());
			p_var->choices_text.p_values.push_back(*p_val);


            p_var->ops->pf_dup( &p_var->choices.p_values[i] );
            p_var->choices_text.p_values[i].psz_string =
                ( p_val2 && p_val2->psz_string ) ?
                strdup( p_val2->psz_string ) : NULL;

            TriggerListCallback(p_this, p_var, psz_name, MX_VAR_ADDCHOICE, p_val);
            break;
        }
        case MX_VAR_DELCHOICE:
        {
            int i;

            for( i = 0 ; i < /*p_var->choices.i_count*/p_var->choices.p_values.size(); i++ )
                if( p_var->ops->pf_cmp( p_var->choices.p_values[i], *p_val ) == 0 )
                    break;

            if( i == /*p_var->choices.i_count*/p_var->choices.p_values.size())
            {
                /* Not found */
                mxMutexUnlock( &p_priv->var_lock );
                return MX_EGENERIC;
            }

            p_var->ops->pf_free( &p_var->choices.p_values[i] );
            free( p_var->choices_text.p_values[i].psz_string );
            /*TAB_ERASE(p_var->choices.i_count, p_var->choices.p_values, i);
            TAB_ERASE(p_var->choices_text.i_count,
                      p_var->choices_text.p_values, i);*/
			p_var->choices_text.p_values.erase(i+ p_var->choices_text.p_values.begin());
			p_var->choices_text.p_values.erase(i + p_var->choices_text.p_values.begin());

            TriggerListCallback(p_this, p_var, psz_name, MX_VAR_DELCHOICE, p_val);
            break;
        }
        case MX_VAR_CHOICESCOUNT:
            //p_val->i_int = p_var->choices.i_count;
            break;
        case MX_VAR_CLEARCHOICES:
            for( int i = 0 ; i < /*p_var->choices.i_count*/p_var->choices.p_values.size(); i++ )
                p_var->ops->pf_free( &p_var->choices.p_values[i] );
            for( int i = 0 ; i < /*p_var->choices_text.i_count*/p_var->choices_text.p_values.size(); i++ )
                free( p_var->choices_text.p_values[i].psz_string );

            if( /*p_var->choices.i_count*/ p_var->choices.p_values.size() > 0) 
				/*free( p_var->choices.p_values )*/p_var->choices.p_values.clear();
            if( /*p_var->choices_text.i_count*/ p_var->choices_text.p_values.size()) 
				/*free( p_var->choices_text.p_values )*/p_var->choices_text.p_values.clear();

            //p_var->choices.i_count = 0;
            //p_var->choices.p_values = NULL;
            //p_var->choices_text.i_count = 0;
            //p_var->choices_text.p_values = NULL;
            TriggerListCallback(p_this, p_var, psz_name, MX_VAR_CLEARCHOICES, NULL);
            break;
        case MX_VAR_SETVALUE:
            /* Duplicate data if needed */
            newval = *p_val;
            p_var->ops->pf_dup( &newval );
            /* Backup needed stuff */
            oldval = p_var->val;
            /* Check boundaries and list */
            CheckValue( p_var, &newval );
            /* Set the variable */
            p_var->val = newval;
            /* Free data if needed */
            p_var->ops->pf_free( &oldval );
            break;
        case MX_VAR_GETCHOICES:
            p_val->p_list = (mx_list_t *)xmalloc( sizeof(mx_list_t) );
            /*p_val->p_list->p_values =
                xmalloc( p_var->choices.i_count * sizeof(mx_value_t) );*/
			p_val->p_list->p_values = p_var->choices.p_values;
            p_val->p_list->i_type = p_var->i_type;
            //p_val->p_list->i_count = p_var->choices.i_count;
            if( p_val2 )
            {
                p_val2->p_list = (mx_list_t *)xmalloc( sizeof(mx_list_t) );
                /*p_val2->p_list->p_values =
                    xmalloc( p_var->choices.i_count * sizeof(mx_value_t) );*/
				p_val2->p_list->p_values = p_var->choices.p_values;
                p_val2->p_list->i_type = MX_VAR_STRING;
                //p_val2->p_list->i_count = p_var->choices.i_count;
            }
            for( int i = 0 ; i < /*p_var->choices.i_count*/p_var->choices.p_values.size(); i++ )
            {
                p_val->p_list->p_values[i] = p_var->choices.p_values[i];
                p_var->ops->pf_dup( &p_val->p_list->p_values[i] );
                if( p_val2 )
                {
                    p_val2->p_list->p_values[i].psz_string =
                        p_var->choices_text.p_values[i].psz_string ?
                    strdup(p_var->choices_text.p_values[i].psz_string) : NULL;
                }
            }
            break;
        case MX_VAR_SETTEXT:
            free( p_var->psz_text );
            if( p_val && p_val->psz_string )
                p_var->psz_text = strdup( p_val->psz_string );
            else
                p_var->psz_text = NULL;
            break;
        case MX_VAR_GETTEXT:
            p_val->psz_string = p_var->psz_text ? strdup( p_var->psz_text )
                                                : NULL;
            break;
        default:
            break;
    }

    mxMutexUnlock( &p_priv->var_lock );

    return ret;
}

#undef var_GetAndSet
/**
 * Perform a Get and Set on a variable
 *
 * \param p_this: The object that hold the variable
 * \param psz_name: the name of the variable
 * \param i_action: the action to perform
 * \param p_val: The action parameter
 * \return vlc error codes
 */
int var_GetAndSet( CMxObject *p_this, const char *psz_name, int i_action,
                   mx_value_t *p_val )
{
    variable_t *p_var;
    mx_value_t oldval;

    assert( p_this );
    assert( p_val );

    mx_object_internals_t *p_priv = mx_internals( p_this );

    p_var = Lookup( p_this, psz_name );
    if( p_var == NULL )
    {
        mxMutexUnlock( &p_priv->var_lock );
        return MX_ENOVAR;
    }

    WaitUnused( p_this, p_var );

    /* Duplicated data if needed */
    //p_var->ops->pf_dup( &val );

    /* Backup needed stuff */
    oldval = p_var->val;

    /* depending of the action requiered */
    switch( i_action )
    {
    case MX_VAR_BOOL_TOGGLE:
        assert( ( p_var->i_type & MX_VAR_BOOL ) == MX_VAR_BOOL );
        p_var->val.b_bool = !p_var->val.b_bool;
        break;
    case MX_VAR_INTEGER_ADD:
        assert( ( p_var->i_type & MX_VAR_INTEGER ) == MX_VAR_INTEGER );
        p_var->val.i_int += p_val->i_int;
        break;
    case MX_VAR_INTEGER_OR:
        assert( ( p_var->i_type & MX_VAR_INTEGER ) == MX_VAR_INTEGER );
        p_var->val.i_int |= p_val->i_int;
        break;
    case MX_VAR_INTEGER_NAND:
        assert( ( p_var->i_type & MX_VAR_INTEGER ) == MX_VAR_INTEGER );
        p_var->val.i_int &= ~p_val->i_int;
        break;
    default:
        mxMutexUnlock( &p_priv->var_lock );
        return MX_EGENERIC;
    }

    /*  Check boundaries */
    CheckValue( p_var, &p_var->val );
    *p_val = p_var->val;

    /* Deal with callbacks.*/
    TriggerCallback( p_this, p_var, psz_name, oldval );

    mxMutexUnlock( &p_priv->var_lock );
    return MX_SUCCESS;
}

#undef var_Type
/**
 * Request a variable's type
 *
 * \return The variable type if it exists, or 0 if the
 * variable could not be found.
 * \see \ref var_type
 */
int var_Type( CMxObject *p_this, const char *psz_name )
{
    variable_t *p_var;
    int i_type = 0;

    assert( p_this );

    mx_object_internals_t *p_priv = mx_internals( p_this );

    p_var = Lookup( p_this, psz_name );
    if( p_var != NULL )
    {
        i_type = p_var->i_type;
        if( /*p_var->choices.i_count > 0*/p_var->choices.p_values.size() > 0)
            i_type |= MX_VAR_HASCHOICE;
    }
    mxMutexUnlock( &p_priv->var_lock );

    return i_type;
}

#undef var_SetChecked
int var_SetChecked( CMxObject *p_this, const char *psz_name,
                    int expected_type, mx_value_t val )
{
    variable_t *p_var;
    mx_value_t oldval;

    assert( p_this );

    mx_object_internals_t *p_priv = mx_internals( p_this );

    p_var = Lookup( p_this, psz_name );
    if( p_var == NULL )
    {
        mxMutexUnlock( &p_priv->var_lock );
        return MX_ENOVAR;
    }

    assert( expected_type == 0 ||
            (p_var->i_type & MX_VAR_CLASS) == expected_type );
    assert ((p_var->i_type & MX_VAR_CLASS) != MX_VAR_VOID);

    WaitUnused( p_this, p_var );

    /* Duplicate data if needed */
    p_var->ops->pf_dup( &val );

    /* Backup needed stuff */
    oldval = p_var->val;

    /* Check boundaries and list */
    CheckValue( p_var, &val );

    /* Set the variable */
    p_var->val = val;

    /* Deal with callbacks */
    TriggerCallback( p_this, p_var, psz_name, oldval );

    /* Free data if needed */
    p_var->ops->pf_free( &oldval );

    mxMutexUnlock( &p_priv->var_lock );
    return MX_SUCCESS;
}

#undef var_Set
/**
 * Set a variable's value
 *
 * \param p_this The object that hold the variable
 * \param psz_name The name of the variable
 * \param val the value to set
 */
int var_Set( CMxObject *p_this, const char *psz_name, mx_value_t val )
{
    return var_SetChecked( p_this, psz_name, 0, val );
}

#undef var_GetChecked
int var_GetChecked( CMxObject *p_this, const char *psz_name,
                    int expected_type, mx_value_t *p_val )
{
    assert( p_this );

    mx_object_internals_t *p_priv = mx_internals( p_this );
    variable_t *p_var;
    int err = MX_SUCCESS;

    p_var = Lookup( p_this, psz_name );
    if( p_var != NULL )
    {
        assert( expected_type == 0 ||
                (p_var->i_type & MX_VAR_CLASS) == expected_type );
        assert ((p_var->i_type & MX_VAR_CLASS) != MX_VAR_VOID);

        /* Really get the variable */
        *p_val = p_var->val;

        /* Duplicate value if needed */
        p_var->ops->pf_dup( p_val );
    }
    else
        err = MX_ENOVAR;

    mxMutexUnlock( &p_priv->var_lock );
    return err;
}

#undef var_Get
/**
 * Get a variable's value
 *
 * \param p_this The object that holds the variable
 * \param psz_name The name of the variable
 * \param p_val Pointer to a mx_value_t that will hold the variable's value
 *              after the function is finished
 */
int var_Get( CMxObject *p_this, const char *psz_name, mx_value_t *p_val )
{
    return var_GetChecked( p_this, psz_name, 0, p_val );
}

typedef enum
{
    mx_value_callback,
    mx_list_callback
} mx_callback_type_t;

static void AddCallback( CMxObject *p_this, const char *psz_name,
                        callback_entry_t entry, mx_callback_type_t i_type )
{
    variable_t *p_var;

    assert( p_this );

    mx_object_internals_t *p_priv = mx_internals( p_this );

    p_var = Lookup( p_this, psz_name );
    if( p_var == NULL )
    {
        mxMutexUnlock( &p_priv->var_lock );
        /*msg_Err( p_this, "cannot add callback %p to nonexistent variable '%s'",
                 entry.p_callback, psz_name );*/
        return;
    }

    WaitUnused( p_this, p_var );

    callback_table_t *p_table;
    if (i_type == mx_value_callback)
        p_table = &p_var->value_callbacks;
    else
        p_table = &p_var->list_callbacks;
    //TAB_APPEND(p_table->i_entries, p_table->p_entries, entry);
	p_table->p_entries.push_back(entry);

    mxMutexUnlock( &p_priv->var_lock );
}

#undef var_AddCallback
/**
 * Register a callback in a variable
 *
 * We store a function pointer that will be called upon variable
 * modification.
 *
 * \param p_this The object that holds the variable
 * \param psz_name The name of the variable
 * \param pf_callback The function pointer
 * \param p_data A generic pointer that will be passed as the last
 *               argument to the callback function.
 *
 * \warning The callback function is run in the thread that calls var_Set on
 *          the variable. Use proper locking. This thread may not have much
 *          time to spare, so keep callback functions short.
 */
void var_AddCallback( CMxObject *p_this, const char *psz_name,
                      mx_callback_t pf_callback, void *p_data )
{
    callback_entry_t entry;
    entry.pf_value_callback = pf_callback;
    entry.p_data = p_data;

    AddCallback(p_this, psz_name, entry, mx_value_callback);
}

static void DelCallback( CMxObject *p_this, const char *psz_name,
                         callback_entry_t entry, mx_callback_type_t i_type )
{
    int i_entry;
    variable_t *p_var;
#ifndef NDEBUG
    bool b_found_similar = false;
#endif

    assert( p_this );

    mx_object_internals_t *p_priv = mx_internals( p_this );

    p_var = Lookup( p_this, psz_name );
    if( p_var == NULL )
    {
        mxMutexUnlock( &p_priv->var_lock );
        /*msg_Err( p_this, "cannot delete callback %p from nonexistent "
                 "variable '%s'", entry.p_callback, psz_name );*/
        return;
    }

    WaitUnused( p_this, p_var );

    callback_table_t *p_table;
    if (i_type == mx_value_callback)
        p_table = &p_var->value_callbacks;
    else
        p_table = &p_var->list_callbacks;

    for( i_entry = /*p_table->i_entries*/p_table->p_entries.size(); i_entry-- ; )
    {
        if( p_table->p_entries[i_entry].p_callback == entry.p_callback
            && p_table->p_entries[i_entry].p_data == entry.p_data )
        {
            break;
        }
#ifndef NDEBUG
        else if( p_table->p_entries[i_entry].p_callback == entry.p_callback )
            b_found_similar = true;
#endif
    }

    if( i_entry < 0 )
    {
#ifndef NDEBUG
        if( b_found_similar )
            fprintf( stderr, "Calling var_DelCallback for '%s' with the same "
                             "function but not the same data.", psz_name );
        MX_assert_unreachable();
#endif
        mxMutexUnlock( &p_priv->var_lock );
        return;
    }

    //TAB_ERASE(p_table->i_entries, p_table->p_entries, i_entry);
	p_table->p_entries.erase(i_entry+ p_table->p_entries.begin());

    mxMutexUnlock( &p_priv->var_lock );
}

#undef var_DelCallback
/**
 * Remove a callback from a variable
 *
 * pf_callback and p_data have to be given again, because different objects
 * might have registered the same callback function.
 */
void var_DelCallback( CMxObject *p_this, const char *psz_name,
                      mx_callback_t pf_callback, void *p_data )
{
    callback_entry_t entry;
    entry.pf_value_callback = pf_callback;
    entry.p_data = p_data;

    DelCallback(p_this, psz_name, entry, mx_value_callback);
}

#undef var_TriggerCallback
/**
 * Trigger callback on a variable
 *
 * \param p_this The object that hold the variable
 * \param psz_name The name of the variable
 */
void var_TriggerCallback( CMxObject *p_this, const char *psz_name )
{
    mx_object_internals_t *p_priv = mx_internals( p_this );
    variable_t *p_var = Lookup( p_this, psz_name );
    if( p_var != NULL )
    {
        WaitUnused( p_this, p_var );

        /* Deal with callbacks. Tell we're in a callback, release the lock,
         * call stored functions, retake the lock. */
        TriggerCallback( p_this, p_var, psz_name, p_var->val );
    }
    mxMutexUnlock( &p_priv->var_lock );
}

#undef var_AddListCallback
/**
 * Register a callback for a list variable
 *
 * The callback is triggered when an element is added/removed from the
 * list or when the list is cleared.
 *
 * See var_AddCallback().
 */
void var_AddListCallback( CMxObject *p_this, const char *psz_name,
                          mx_list_callback_t pf_callback, void *p_data )
{
    callback_entry_t entry;
    entry.pf_list_callback = pf_callback;
    entry.p_data = p_data;

    AddCallback(p_this, psz_name, entry, mx_list_callback);
}

#undef var_DelListCallback
/**
 * Remove a callback from a list variable
 *
 * See var_DelCallback().
 */
void var_DelListCallback( CMxObject *p_this, const char *psz_name,
                          mx_list_callback_t pf_callback, void *p_data )
{
    callback_entry_t entry;
    entry.pf_list_callback = pf_callback;
    entry.p_data = p_data;

    DelCallback(p_this, psz_name, entry, mx_list_callback);
}

/** Parse a stringified option
 * This function parse a string option and create the associated object
 * variable
 * The option must be of the form "[no[-]]foo[=bar]" where foo is the
 * option name and bar is the value of the option.
 * \param p_obj the object in which the variable must be created
 * \param psz_option the option to parse
 * \param trusted whether the option is set by a trusted input or not
 * \return nothing
 */
void var_OptionParse( CMxObject *p_obj, const char *psz_option,
                      bool trusted )
{
    char *psz_name, *psz_value;
    int  i_type;
    bool b_isno = false;
    mx_value_t val;

    val.psz_string = NULL;

    /* It's too much of a hassle to remove the ':' when we parse
     * the cmd line :) */
    if( psz_option[0] == ':' )
        psz_option++;

    if( !psz_option[0] )
        return;

    psz_name = strdup( psz_option );
    if( psz_name == NULL )
        return;

    psz_value = strchr( psz_name, '=' );
    if( psz_value != NULL )
        *psz_value++ = '\0';

    //i_type = config_GetType( psz_name );
    //if( !i_type && !psz_value )
    //{
    //    /* check for "no-foo" or "nofoo" */
    //    if( !strncmp( psz_name, "no-", 3 ) )
    //    {
    //        memmove( psz_name, psz_name + 3, strlen(psz_name) + 1 - 3 );
    //    }
    //    else if( !strncmp( psz_name, "no", 2 ) )
    //    {
    //        memmove( psz_name, psz_name + 2, strlen(psz_name) + 1 - 2 );
    //    }
    //    else goto cleanup;           /* Option doesn't exist */

    //    b_isno = true;
    //    i_type = config_GetType( psz_name );
    //}
    //if( !i_type ) goto cleanup; /* Option doesn't exist */

    //if( ( i_type != MX_VAR_BOOL ) &&
    //    ( !psz_value || !*psz_value ) ) goto cleanup; /* Invalid value */

    ///* check if option is unsafe */
    //if( !trusted && !config_IsSafe( psz_name ) )
    //{
    //    msg_Err( p_obj, "unsafe option \"%s\" has been ignored for "
    //                    "security reasons", psz_name );
    //    free( psz_name );
    //    return;
    //}

    ///* Create the variable in the input object.
    // * Children of the input object will be able to retrieve this value
    // * thanks to the inheritance property of the object variables. */
    //var_Create( p_obj, psz_name, i_type );

    //switch( i_type )
    //{
    //case MX_VAR_BOOL:
    //    val.b_bool = !b_isno;
    //    break;

    //case MX_VAR_INTEGER:
    //    val.i_int = strtoll( psz_value, NULL, 0 );
    //    break;

    //case MX_VAR_FLOAT:
    //    val.f_float = us_atof( psz_value );
    //    break;

    //case MX_VAR_STRING:
    //    val.psz_string = psz_value;
    //    break;

    //default:
    //    goto cleanup;
    //}

    //var_Set( p_obj, psz_name, val );

cleanup:
    free( psz_name );
}

#undef var_LocationParse
/**
 * Parses a set of colon-separated or semicolon-separated
 * <code>name=value</code> pairs.
 * Some access (or access_demux) plugins uses this scheme
 * in media resource location.
 * @note Only trusted/safe variables are allowed. This is intended.
 *
 * @warning Only use this for plugins implementing VLC-specific resource
 * location schemes. This would not make any sense for standardized ones.
 *
 * @param obj VLC object on which to set variables (and emit error messages)
 * @param mrl string to parse
 * @param pref prefix to prepend to option names in the string
 *
 * @return MX_ENOMEM on error, MX_SUCCESS on success.
 */
int var_LocationParse (CMxObject *obj, const char *mrl, const char *pref)
{
    int ret = MX_SUCCESS;
    size_t preflen = strlen (pref) + 1;

    assert(mrl != NULL);
    while (*mrl != '\0')
    {
        mrl += strspn (mrl, ":;"); /* skip leading colon(s) */

        size_t len = strcspn (mrl, ":;");
        char *buf = (char *)malloc (preflen + len);

        if (likely(buf != NULL))
        {
            /* NOTE: this does not support the "no-<varname>" bool syntax. */
            /* DO NOT use asprintf() here; it won't work! Think again. */
            snprintf (buf, preflen + len, "%s%s", pref, mrl);
            var_OptionParse (obj, buf, false);
            free (buf);
        }
        else
            ret = MX_ENOMEM;
        mrl += len;
    }

    return ret;
}

/**
 * Finds the value of a variable. If the specified object does not hold a
 * variable with the specified name, try the parent object, and iterate until
 * the top of the tree. If no match is found, the value is read from the
 * configuration.
 */
int var_Inherit( CMxObject *p_this, const char *psz_name, int i_type,
                 mx_value_t *p_val )
{
    i_type &= MX_VAR_CLASS;
    for( CMxObject *obj = p_this; obj != NULL; obj = obj->obj.parent )
    {
        if( var_GetChecked( obj, psz_name, i_type, p_val ) == MX_SUCCESS )
            return MX_SUCCESS;
    }

    /* else take value from config */
    /*switch( i_type & MX_VAR_CLASS )
    {
        case MX_VAR_STRING:
            p_val->psz_string = config_GetPsz( p_this, psz_name );
            if( !p_val->psz_string ) p_val->psz_string = strdup("");
            break;
        case MX_VAR_FLOAT:
            p_val->f_float = config_GetFloat( p_this, psz_name );
            break;
        case MX_VAR_INTEGER:
            p_val->i_int = config_GetInt( p_this, psz_name );
            break;
        case MX_VAR_BOOL:
            p_val->b_bool = config_GetInt( p_this, psz_name ) > 0;
            break;
        default:
            MX_assert_unreachable();
        case MX_VAR_ADDRESS:
            return MX_ENOOBJ;
    }*/
    return MX_SUCCESS;
}


/**
 * It inherits a string as an unsigned rational number (it also accepts basic
 * float number).
 *
 * It returns an error if the rational number cannot be parsed (0/0 is valid).
 * The rational is already reduced.
 */
int (var_InheritURational)(CMxObject *object,
                           unsigned *num, unsigned *den,
                           const char *var)
{
    char *str = var_InheritString(object, var);
    if (str == NULL)
        goto error;

    char *sep;
    unsigned n = strtoul(str, &sep, 10);
    unsigned d;

    switch (*sep) {
        case '\0':
            /* Decimal integer */
            d = 1;
            break;

        case ':':
        case '/':
            /* Decimal fraction */
            d = strtoul(sep + 1, &sep, 10);
            if (*sep != '\0')
                goto error;
            break;

        case '.': {
            /* Decimal number */
            unsigned char c;

            d = 1;
            while ((c = *(++sep)) != '\0') {
                c -= '0';

                if (c >= 10)
                    goto error;

                n = n * 10 + c;
                d *= 10;
            }
            break;
        }

        default:
            goto error;
    }

    free(str);

    if (n == 0) {
        *num = 0;
        *den = d ? 1 : 0;
    } else if (d == 0) {
        *num = 1;
        *den = 0;
    } else
        mxUreduce(num, den, n, d, 0);

    return MX_SUCCESS;

error:
    free(str);
    *num = 0;
    *den = 0;
    return MX_EGENERIC;
}

/**
 * Free a list and the associated strings
 * @param p_val: the list variable
 * @param p_val2: the variable associated or NULL
 */
void var_FreeList( mx_value_t *p_val, mx_value_t *p_val2 )
{
    switch( p_val->p_list->i_type & MX_VAR_CLASS )
    {
        case MX_VAR_STRING:
            for( int i = 0; i < /*p_val->p_list->i_count*/p_val->p_list->p_values.size(); i++ )
                free( p_val->p_list->p_values[i].psz_string );
            break;
    }

    //free( p_val->p_list->p_values );
    free( p_val->p_list );

    if( p_val2 != NULL )
    {
        assert( p_val2->p_list != NULL );
        assert( p_val2->p_list->i_type == MX_VAR_STRING );

        for( int i = 0; i < /*p_val2->p_list->i_count*/p_val2->p_list->p_values.size(); i++ )
            free( p_val2->p_list->p_values[i].psz_string );
        //free( p_val2->p_list->p_values );
        free( p_val2->p_list );
    }
}

static void DumpVariable(const void *data, const VISIT which, const int depth)
{
    if (which != postorder && which != leaf)
        return;
    (void) depth;

    const variable_t *var = *(const variable_t **)data;
    const char *type_name = "unknown";

    switch (var->i_type & MX_VAR_TYPE)
    {
        case MX_VAR_VOID:     type_name = "void";        break;
        case MX_VAR_BOOL:     type_name = "bool";        break;
        case MX_VAR_INTEGER:  type_name = "integer";     break;
        case MX_VAR_STRING:   type_name = "string";      break;
        case MX_VAR_FLOAT:    type_name = "float";       break;
        case MX_VAR_COORDS:   type_name = "coordinates"; break;
        case MX_VAR_ADDRESS:  type_name = "address";     break;
        default:               type_name = "unknown";     break;
    }

    printf(" *-o \"%s\" (%s", var->psz_name, type_name);
    if (var->psz_text != NULL)
        printf(", %s", var->psz_text);
    putchar(')');
    if (var->i_type & MX_VAR_HASCHOICE)
        fputs(", has choices", stdout);
    if (var->i_type & MX_VAR_ISCOMMAND)
        fputs(", command", stdout);
    if (/*var->value_callbacks.i_entries*/var->value_callbacks.p_entries.size() > 0)
        printf(", %d callbacks", var->value_callbacks.p_entries);

    switch (var->i_type & MX_VAR_CLASS)
    {
        case MX_VAR_VOID:
            break;
        case MX_VAR_BOOL:
            printf(": %s", var->val.b_bool ? "true" : "false");
            break;
        case MX_VAR_INTEGER:
            /*printf(": %"PRId64, var->val.i_int );*/
            break;
        case MX_VAR_STRING:
            printf(": \"%s\"", var->val.psz_string );
            break;
        case MX_VAR_FLOAT:
            printf(": %f", var->val.f_float );
            break;
        case MX_VAR_COORDS:
            /*printf(": %"PRId32"x%"PRId32,
                   var->val.coords.x, var->val.coords.y);*/
            break;
        case MX_VAR_ADDRESS:
            printf(": %p", var->val.p_address);
            break;
    }
    putchar('\n');
}

void DumpVariables(CMxObject *obj)
{
    mxMutexLock(&mx_internals(obj)->var_lock);
    if (mx_internals(obj)->var_root == NULL)
        puts(" `-o No variables");
    else
        twalk(mx_internals(obj)->var_root, DumpVariable);
    mxMutexUnlock(&mx_internals(obj)->var_lock);
}

static thread_local void *twalk_ctx;

static void TwalkGetNames(const void *data, const VISIT which, const int depth)
{
    if (which != postorder && which != leaf)
        return;
    (void) depth;

    const variable_t *var = *(const variable_t **)data;
    /*DECL_ARRAY(char *) *names = twalk_ctx;
    char *dup = strdup(var->psz_name);
    if (dup != NULL)
        ARRAY_APPEND(*names, dup);*/
}

char **var_GetAllNames(CMxObject *obj)
{
    /*mx_object_internals_t *priv = mx_internals(obj);

    DECL_ARRAY(char *) names;
    ARRAY_INIT(names);

    twalk_ctx = &names;
    mxMutexLock(&priv->var_lock);
    twalk(priv->var_root, TwalkGetNames);
	mxMutexUnlock(&priv->var_lock);

    if (names.i_size == 0)
        return NULL;
    ARRAY_APPEND(names, NULL);
    return names.p_elems;*/

	return NULL;
}
