/*****************************************************************************
 * vlc_variables.h: variables handling
 *****************************************************************************
 * Copyright (C) 2002-2004 VLC authors and VideoLAN
 * $Id: 83752b171f82c86164142a6254f513fc9cb7a324 $
 *
 * Authors: Samuel Hocevar <sam@zoy.org>
 *          Gildas Bazin <gbazin@netcourrier.com>
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

#ifndef MX_VARIABLES_H
#define MX_VARIABLES_H 1

/**
 * \defgroup variables Variables
 * \ingroup vlc_object
 *
 * VLC object variables and callbacks
 *
 * @{
 * \file
 * VLC object variables and callbacks interface
 */

#define MX_VAR_TYPE      0x00ff
#define MX_VAR_CLASS     0x00f0
#define MX_VAR_FLAGS     0xff00

/**
 * \defgroup var_type Variable types
 * These are the different types a vlc variable can have.
 * @{
 */
#define MX_VAR_VOID      0x0010
#define MX_VAR_BOOL      0x0020
#define MX_VAR_INTEGER   0x0030
#define MX_VAR_STRING    0x0040
#define MX_VAR_FLOAT     0x0050
#define MX_VAR_ADDRESS   0x0070
#define MX_VAR_COORDS    0x00A0
/**@}*/

/** \defgroup var_flags Additive flags
 * These flags are added to the type field of the variable. Most as a result of
 * a var_Change() call, but some may be added at creation time
 * @{
 */
#define MX_VAR_HASCHOICE 0x0100

#define MX_VAR_ISCOMMAND 0x2000

/** Creation flag */
/* If the variable is not found on the current module
   search all parents and finally module config until found */
#define MX_VAR_DOINHERIT 0x8000
/**@}*/

/**
 * \defgroup var_action Variable actions
 * These are the different actions that can be used with var_Change().
 * The parameters given are the meaning of the two last parameters of
 * var_Change() when this action is being used.
 * @{
 */

#define MX_VAR_SETSTEP             0x0012

/**
 * Set the value of this variable without triggering any callbacks
 * \param p_val The new value
 * \param p_val2 Unused
 */
#define MX_VAR_SETVALUE            0x0013

#define MX_VAR_SETTEXT             0x0014
#define MX_VAR_GETTEXT             0x0015

#define MX_VAR_GETMIN              0x0016
#define MX_VAR_GETMAX              0x0017
#define MX_VAR_GETSTEP             0x0018

#define MX_VAR_ADDCHOICE           0x0020
#define MX_VAR_DELCHOICE           0x0021
#define MX_VAR_CLEARCHOICES        0x0022
#define MX_VAR_GETCHOICES          0x0024

#define MX_VAR_CHOICESCOUNT        0x0026
#define MX_VAR_SETMINMAX           0x0027

/**@}*/

/** \defgroup var_GetAndSet Variable actions
 * These are the different actions that can be used with var_GetAndSet()
 * @{
 */
enum {
    MX_VAR_BOOL_TOGGLE, /**< Invert a boolean value (param ignored) */
    MX_VAR_INTEGER_ADD, /**< Add parameter to an integer value */
    MX_VAR_INTEGER_OR,  /**< Binary OR over an integer bits field */
    MX_VAR_INTEGER_NAND,/**< Binary NAND over an integer bits field */
};
/**@}*/

/*****************************************************************************
 * Prototypes
 *****************************************************************************/
MXCODEC_API int var_Create( CMxObject *, const char *, int );
#define var_Create(a,b,c) var_Create( MX_OBJECT(a), b, c )

MXCODEC_API void var_Destroy( CMxObject *, const char * );
#define var_Destroy(a,b) var_Destroy( MX_OBJECT(a), b )

MXCODEC_API int var_Change( CMxObject *, const char *, int, mx_value_t *, mx_value_t * );
#define var_Change(a,b,c,d,e) var_Change( MX_OBJECT(a), b, c, d, e )

MXCODEC_API int var_Type( CMxObject *, const char * ) MX_USED;
#define var_Type(a,b) var_Type( MX_OBJECT(a), b )

MXCODEC_API int var_Set( CMxObject *, const char *, mx_value_t );
#define var_Set(a,b,c) var_Set( MX_OBJECT(a), b, c )

MXCODEC_API int var_Get( CMxObject *, const char *, mx_value_t * );
#define var_Get(a,b,c) var_Get( MX_OBJECT(a), b, c )

MXCODEC_API int var_SetChecked( CMxObject *, const char *, int, mx_value_t );
#define var_SetChecked(o,n,t,v) var_SetChecked(MX_OBJECT(o),n,t,v)
MXCODEC_API int var_GetChecked( CMxObject *, const char *, int, mx_value_t * );
#define var_GetChecked(o,n,t,v) var_GetChecked(MX_OBJECT(o),n,t,v)
MXCODEC_API int var_GetAndSet( CMxObject *, const char *, int, mx_value_t * );

MXCODEC_API int var_Inherit( CMxObject *, const char *, int, mx_value_t * );

MXCODEC_API void var_FreeList( mx_value_t *, mx_value_t * );


/*****************************************************************************
 * Variable callbacks
 *****************************************************************************
 * int MyCallback( CMxObject *p_this,
 *                 char const *psz_variable,
 *                 mx_value_t oldvalue,
 *                 mx_value_t newvalue,
 *                 void *p_data);
 *****************************************************************************/
MXCODEC_API void var_AddCallback( CMxObject *, const char *, mx_callback_t, void * );
MXCODEC_API void var_DelCallback( CMxObject *, const char *, mx_callback_t, void * );
MXCODEC_API void var_TriggerCallback( CMxObject *, const char * );

MXCODEC_API void var_AddListCallback( CMxObject *, const char *, mx_list_callback_t, void * );
MXCODEC_API void var_DelListCallback( CMxObject *, const char *, mx_list_callback_t, void * );

#define var_AddCallback(a,b,c,d) var_AddCallback( MX_OBJECT(a), b, c, d )
#define var_DelCallback(a,b,c,d) var_DelCallback( MX_OBJECT(a), b, c, d )
#define var_TriggerCallback(a,b) var_TriggerCallback( MX_OBJECT(a), b )

#define var_AddListCallback(a,b,c,d) var_AddListCallback( MX_OBJECT(a), b, c, d )
#define var_DelListCallback(a,b,c,d) var_DelListCallback( MX_OBJECT(a), b, c, d )

/*****************************************************************************
 * helpers functions
 *****************************************************************************/

/**
 * Set the value of an integer variable
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 * \param i The new integer value of this variable
 */
static inline int var_SetInteger( CMxObject *p_obj, const char *psz_name,
                                  int64_t i )
{
    mx_value_t val;
    val.i_int = i;
    return var_SetChecked( p_obj, psz_name, MX_VAR_INTEGER, val );
}

/**
 * Set the value of an boolean variable
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 * \param b The new boolean value of this variable
 */
static inline int var_SetBool( CMxObject *p_obj, const char *psz_name, bool b )
{
    mx_value_t val;
    val.b_bool = b;
    return var_SetChecked( p_obj, psz_name, MX_VAR_BOOL, val );
}

static inline int var_SetCoords( CMxObject *obj, const char *name,
                                 int32_t x, int32_t y )
{
    mx_value_t val;
    val.coords.x = x;
    val.coords.y = y;
    return var_SetChecked (obj, name, MX_VAR_COORDS, val);
}
#define var_SetCoords(o,n,x,y) var_SetCoords(MX_OBJECT(o),n,x,y)

/**
 * Set the value of a float variable
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 * \param f The new float value of this variable
 */
static inline int var_SetFloat( CMxObject *p_obj, const char *psz_name, float f )
{
    mx_value_t val;
    val.f_float = f;
    return var_SetChecked( p_obj, psz_name, MX_VAR_FLOAT, val );
}

/**
 * Set the value of a string variable
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 * \param psz_string The new string value of this variable
 */
static inline int var_SetString( CMxObject *p_obj, const char *psz_name, const char *psz_string )
{
    mx_value_t val;
    val.psz_string = (char *)psz_string;
    return var_SetChecked( p_obj, psz_name, MX_VAR_STRING, val );
}

/**
 * Set the value of a pointer variable
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 * \param ptr The new pointer value of this variable
 */
static inline
int var_SetAddress( CMxObject *p_obj, const char *psz_name, void *ptr )
{
    mx_value_t val;
    val.p_address = ptr;
    return var_SetChecked( p_obj, psz_name, MX_VAR_ADDRESS, val );
}

#define var_SetInteger(a,b,c)   var_SetInteger( MX_OBJECT(a),b,c)
#define var_SetBool(a,b,c)      var_SetBool( MX_OBJECT(a),b,c)
#define var_SetFloat(a,b,c)     var_SetFloat( MX_OBJECT(a),b,c)
#define var_SetString(a,b,c)    var_SetString( MX_OBJECT(a),b,c)
#define var_SetAddress(o, n, p) var_SetAddress(MX_OBJECT(o), n, p)


/**
 * Get an integer value
*
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline int64_t var_GetInteger( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t val;
    if( !var_GetChecked( p_obj, psz_name, MX_VAR_INTEGER, &val ) )
        return val.i_int;
    else
        return 0;
}

/**
 * Get a boolean value
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline bool var_GetBool( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t val; val.b_bool = false;

    if( !var_GetChecked( p_obj, psz_name, MX_VAR_BOOL, &val ) )
        return val.b_bool;
    else
        return false;
}

static inline void var_GetCoords( CMxObject *obj, const char *name,
                                  int32_t *px, int32_t *py )
{
    mx_value_t val;

    if (likely(!var_GetChecked (obj, name, MX_VAR_COORDS, &val)))
    {
        *px = val.coords.x;
        *py = val.coords.y;
    }
    else
        *px = *py = 0;
}
#define var_GetCoords(o,n,x,y) var_GetCoords(MX_OBJECT(o),n,x,y)

/**
 * Get a float value
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline float var_GetFloat( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t val; val.f_float = 0.0;
    if( !var_GetChecked( p_obj, psz_name, MX_VAR_FLOAT, &val ) )
        return val.f_float;
    else
        return 0.0;
}

/**
 * Get a string value
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED MX_MALLOC
static inline char *var_GetString( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t val; val.psz_string = NULL;
    if( var_GetChecked( p_obj, psz_name, MX_VAR_STRING, &val ) )
        return NULL;
    else
        return val.psz_string;
}

MX_USED MX_MALLOC
static inline char *var_GetNonEmptyString( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t val;
    if( var_GetChecked( p_obj, psz_name, MX_VAR_STRING, &val ) )
        return NULL;
    if( val.psz_string && *val.psz_string )
        return val.psz_string;
    free( val.psz_string );
    return NULL;
}

MX_USED
static inline void *var_GetAddress( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t val;
    if( var_GetChecked( p_obj, psz_name, MX_VAR_ADDRESS, &val ) )
        return NULL;
    else
        return val.p_address;
}

/**
 * Increment an integer variable
 * \param p_obj the object that holds the variable
 * \param psz_name the name of the variable
 */
static inline int64_t var_IncInteger( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t val;
    val.i_int = 1;
    if( var_GetAndSet( p_obj, psz_name, MX_VAR_INTEGER_ADD, &val ) )
        return 0;
    return val.i_int;
}
#define var_IncInteger(a,b) var_IncInteger( MX_OBJECT(a), b )

/**
 * Decrement an integer variable
 * \param p_obj the object that holds the variable
 * \param psz_name the name of the variable
 */
static inline int64_t var_DecInteger( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t val;
    val.i_int = -1;
    if( var_GetAndSet( p_obj, psz_name, MX_VAR_INTEGER_ADD, &val ) )
        return 0;
    return val.i_int;
}
#define var_DecInteger(a,b) var_DecInteger( MX_OBJECT(a), b )

static inline uint64_t var_OrInteger( CMxObject *obj, const char *name,
                                      unsigned v )
{
    mx_value_t val;
    val.i_int = v;
    if( var_GetAndSet( obj, name, MX_VAR_INTEGER_OR, &val ) )
        return 0;
    return val.i_int;
}
#define var_OrInteger(a,b,c) var_OrInteger(MX_OBJECT(a),b,c)

static inline uint64_t var_NAndInteger( CMxObject *obj, const char *name,
                                        unsigned v )
{
    mx_value_t val;
    val.i_int = v;
    if( var_GetAndSet( obj, name, MX_VAR_INTEGER_NAND, &val ) )
        return 0;
    return val.i_int;
}
#define var_NAndInteger(a,b,c) var_NAndInteger(MX_OBJECT(a),b,c)

/**
 * Create a integer variable with inherit and get its value.
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline int64_t var_CreateGetInteger( CMxObject *p_obj, const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_INTEGER | MX_VAR_DOINHERIT );
    return var_GetInteger( p_obj, psz_name );
}

/**
 * Create a boolean variable with inherit and get its value.
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline bool var_CreateGetBool( CMxObject *p_obj, const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_BOOL | MX_VAR_DOINHERIT );
    return var_GetBool( p_obj, psz_name );
}

/**
 * Create a float variable with inherit and get its value.
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline float var_CreateGetFloat( CMxObject *p_obj, const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_FLOAT | MX_VAR_DOINHERIT );
    return var_GetFloat( p_obj, psz_name );
}

/**
 * Create a string variable with inherit and get its value.
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED MX_MALLOC
static inline char *var_CreateGetString( CMxObject *p_obj,
                                           const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_STRING | MX_VAR_DOINHERIT );
    return var_GetString( p_obj, psz_name );
}

MX_USED MX_MALLOC
static inline char *var_CreateGetNonEmptyString( CMxObject *p_obj,
                                                   const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_STRING | MX_VAR_DOINHERIT );
    return var_GetNonEmptyString( p_obj, psz_name );
}

/**
 * Create an address variable with inherit and get its value.
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline void *var_CreateGetAddress( CMxObject *p_obj,
                                           const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_ADDRESS | MX_VAR_DOINHERIT );
    return var_GetAddress( p_obj, psz_name );
}

#define var_CreateGetInteger(a,b)   var_CreateGetInteger( MX_OBJECT(a),b)
#define var_CreateGetBool(a,b)   var_CreateGetBool( MX_OBJECT(a),b)
#define var_CreateGetFloat(a,b)   var_CreateGetFloat( MX_OBJECT(a),b)
#define var_CreateGetString(a,b)   var_CreateGetString( MX_OBJECT(a),b)
#define var_CreateGetNonEmptyString(a,b)   var_CreateGetNonEmptyString( MX_OBJECT(a),b)
#define var_CreateGetAddress(a,b)  var_CreateGetAddress( MX_OBJECT(a),b)

/**
 * Create a integer command variable with inherit and get its value.
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline int64_t var_CreateGetIntegerCommand( CMxObject *p_obj, const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_INTEGER | MX_VAR_DOINHERIT
                                   | MX_VAR_ISCOMMAND );
    return var_GetInteger( p_obj, psz_name );
}

/**
 * Create a boolean command variable with inherit and get its value.
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline bool var_CreateGetBoolCommand( CMxObject *p_obj, const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_BOOL | MX_VAR_DOINHERIT
                                   | MX_VAR_ISCOMMAND );
    return var_GetBool( p_obj, psz_name );
}

/**
 * Create a float command variable with inherit and get its value.
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED
static inline float var_CreateGetFloatCommand( CMxObject *p_obj, const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_FLOAT | MX_VAR_DOINHERIT
                                   | MX_VAR_ISCOMMAND );
    return var_GetFloat( p_obj, psz_name );
}

/**
 * Create a string command variable with inherit and get its value.
 *
 * \param p_obj The object that holds the variable
 * \param psz_name The name of the variable
 */
MX_USED MX_MALLOC
static inline char *var_CreateGetStringCommand( CMxObject *p_obj,
                                           const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_STRING | MX_VAR_DOINHERIT
                                   | MX_VAR_ISCOMMAND );
    return var_GetString( p_obj, psz_name );
}

MX_USED MX_MALLOC
static inline char *var_CreateGetNonEmptyStringCommand( CMxObject *p_obj,
                                                   const char *psz_name )
{
    var_Create( p_obj, psz_name, MX_VAR_STRING | MX_VAR_DOINHERIT
                                   | MX_VAR_ISCOMMAND );
    return var_GetNonEmptyString( p_obj, psz_name );
}

#define var_CreateGetIntegerCommand(a,b)   var_CreateGetIntegerCommand( MX_OBJECT(a),b)
#define var_CreateGetBoolCommand(a,b)   var_CreateGetBoolCommand( MX_OBJECT(a),b)
#define var_CreateGetFloatCommand(a,b)   var_CreateGetFloatCommand( MX_OBJECT(a),b)
#define var_CreateGetStringCommand(a,b)   var_CreateGetStringCommand( MX_OBJECT(a),b)
#define var_CreateGetNonEmptyStringCommand(a,b)   var_CreateGetNonEmptyStringCommand( MX_OBJECT(a),b)

MX_USED
static inline int var_CountChoices( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t count;
    if( var_Change( p_obj, psz_name, MX_VAR_CHOICESCOUNT, &count, NULL ) )
        return 0;
    return count.i_int;
}
#define var_CountChoices(a,b) var_CountChoices( MX_OBJECT(a),b)


static inline bool var_ToggleBool( CMxObject *p_obj, const char *psz_name )
{
    mx_value_t val;
    if( var_GetAndSet( p_obj, psz_name, MX_VAR_BOOL_TOGGLE, &val ) )
        return false;
    return val.b_bool;
}
#define var_ToggleBool(a,b) var_ToggleBool( MX_OBJECT(a),b )


MX_USED
static inline bool var_InheritBool( CMxObject *obj, const char *name )
{
    mx_value_t val;

    if( var_Inherit( obj, name, MX_VAR_BOOL, &val ) )
        val.b_bool = false;
    return val.b_bool;
}
#define var_InheritBool(o, n) var_InheritBool(MX_OBJECT(o), n)

MX_USED
static inline int64_t var_InheritInteger( CMxObject *obj, const char *name )
{
    mx_value_t val;

    if( var_Inherit( obj, name, MX_VAR_INTEGER, &val ) )
        val.i_int = 0;
    return val.i_int;
}
#define var_InheritInteger(o, n) var_InheritInteger(MX_OBJECT(o), n)

MX_USED
static inline float var_InheritFloat( CMxObject *obj, const char *name )
{
    mx_value_t val;

    if( var_Inherit( obj, name, MX_VAR_FLOAT, &val ) )
        val.f_float = 0.;
    return val.f_float;
}
#define var_InheritFloat(o, n) var_InheritFloat(MX_OBJECT(o), n)

MX_USED MX_MALLOC
static inline char *var_InheritString( CMxObject *obj, const char *name )
{
    mx_value_t val;

    if( var_Inherit( obj, name, MX_VAR_STRING, &val ) )
        val.psz_string = NULL;
    else if( val.psz_string && !*val.psz_string )
    {
        free( val.psz_string );
        val.psz_string = NULL;
    }
    return val.psz_string;
}
#define var_InheritString(o, n) var_InheritString(MX_OBJECT(o), n)

MX_USED
static inline void *var_InheritAddress( CMxObject *obj, const char *name )
{
    mx_value_t val;

    if( var_Inherit( obj, name, MX_VAR_ADDRESS, &val ) )
        val.p_address = NULL;
    return val.p_address;
}
#define var_InheritAddress(o, n) var_InheritAddress(MX_OBJECT(o), n)

MXCODEC_API int var_InheritURational( CMxObject *, unsigned *num, unsigned *den, const char *var );
#define var_InheritURational(a,b,c,d) var_InheritURational(MX_OBJECT(a), b, c, d)

#define var_GetInteger(a,b)   var_GetInteger( MX_OBJECT(a),b)
#define var_GetBool(a,b)   var_GetBool( MX_OBJECT(a),b)
#define var_GetFloat(a,b)   var_GetFloat( MX_OBJECT(a),b)
#define var_GetString(a,b)   var_GetString( MX_OBJECT(a),b)
#define var_GetNonEmptyString(a,b)   var_GetNonEmptyString( MX_OBJECT(a),b)
#define var_GetAddress(a,b)  var_GetAddress( MX_OBJECT(a),b)

MXCODEC_API int var_LocationParse(CMxObject *, const char *mrl, const char *prefix);
#define var_LocationParse(o, m, p) var_LocationParse(MX_OBJECT(o), m, p)

/*
 * Variables stuff
 */
MXCODEC_API void var_OptionParse (CMxObject *, const char *, bool trusted);

/**
 * @}
 */
#endif /*  _MX_VARIABLES_H */
