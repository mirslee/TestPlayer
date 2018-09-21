/*****************************************************************************
 * events.c: events interface
 * This library provides an interface to the send and receive events.
 * It is more lightweight than variable based callback.
 *****************************************************************************
 * Copyright (C) 1998-2005 VLC authors and VideoLAN
 * $Id: 6bf55268ed99f9ec02289e8e850c6d59851710b6 $
 *
 * Authors: Pierre d'Herbemont <pdherbemont # videolan.org >
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
#include "MxConfig.h"
#include "MxCommon.h"
#include "MxEvents.h"
#include "MxError.h"
#include <assert.h>

/*****************************************************************************
 * Documentation : Read vlc_events.h
 *****************************************************************************/

/*****************************************************************************
 *  Private types.
 *****************************************************************************/

/*****************************************************************************
 *
 *****************************************************************************/

#undef mxEventManagerInit
/**
 * Initialize event manager object
 * p_obj is the object that contains the event manager. But not
 * necessarily a vlc_object_t (an input_item_t is not a vlc_object_t
 * for instance).
 */
void mxEventManagerInit( MxEventManager * p_em, void * p_obj )
{
    p_em->p_obj = p_obj;
    /* This is an unsafe work-around for a long-standing playlist bug.
     * Do not rely on this. */
    mxMutexInitRecursive(&p_em->lock);

    /*for( size_t i = 0; i < ARRAY_SIZE(p_em->events); i++ )
       ARRAY_INIT( p_em->events[i].listeners );*/
}

/**
 * Destroy the event manager
 */
void mxEventManagerFini( MxEventManager * p_em ) 
{
    struct mx_event_listener_t * listener;

    mxMutexDestroy( &p_em->lock );

    for( size_t i = 0; i < MxEventType_InputItemPreparseEnded + 1; i++ )
    {
        struct mx_event_listeners_group_t *slot = p_em->events + i;

        /*FOREACH_ARRAY( listener, slot->listeners )
            free( listener );
        FOREACH_END()
        ARRAY_RESET( slot->listeners );*/
        
        for (int i = 0; i < slot->listeners.size(); i++) {
            free(slot->listeners[i]);
        }
        slot->listeners.clear();
    }
    
}

/**
 * Send an event to the listener attached to this p_em.
 */
void mxEventSend( MxEventManager * p_em, MxEvent * p_event )
{
    mx_event_listeners_group_t *slot = &p_em->events[p_event->type];
    mx_event_listener_t * listener;

    /* Fill event with the sending object now */
    p_event->p_obj = p_em->p_obj;

    mxMutexLock( &p_em->lock ) ;

    /*FOREACH_ARRAY( listener, slot->listeners )
        listener->pf_callback( p_event, listener->p_user_data );
    FOREACH_END()*/
    
    for (int i = 0; i < slot->listeners.size(); i++) {
        slot->listeners[i]->pf_callback( p_event, listener->p_user_data );
    }

    mxMutexUnlock( &p_em->lock );
}

#undef vlc_event_attach
/**
 * Add a callback for an event.
 */
int mxEventAttach( MxEventManager * p_em,
                      MxEventType event_type,
                      mx_event_callback_t pf_callback,
                      void *p_user_data )
{
    mx_event_listener_t * listener;
    mx_event_listeners_group_t *slot = &p_em->events[event_type];

    listener = (mx_event_listener_t*)malloc(sizeof(mx_event_listener_t));
    if( !listener )
        return MX_ENOMEM;

    listener->p_user_data = p_user_data;
    listener->pf_callback = pf_callback;

    mxMutexLock( &p_em->lock );
    //ARRAY_APPEND( slot->listeners, listener );
    slot->listeners.push_back(listener);
    mxMutexUnlock( &p_em->lock );
    return MX_SUCCESS;
}

/**
 * Remove a callback for an event.
 */

void mxEventDetach( MxEventManager *p_em,
                       MxEventType event_type,
                       mx_event_callback_t pf_callback,
                       void *p_user_data )
{
    mx_event_listeners_group_t *slot = &p_em->events[event_type];
    struct mx_event_listener_t * listener;

    mxMutexLock( &p_em->lock );

//    FOREACH_ARRAY( listener, slot->listeners )
//        if( listener->pf_callback == pf_callback &&
//            listener->p_user_data == p_user_data )
//        {
//            /* that's our listener */
//            ARRAY_REMOVE( slot->listeners,
//                          fe_idx /* This comes from the macro (and that's why
//                                    I hate macro) */ );
//            vlc_mutex_unlock( &p_em->lock );
//            free( listener );
//            return;
//        }
//    FOREACH_END()
    
    for (int i = 0; i < slot->listeners.size(); i++) {
        if (slot->listeners[i]->pf_callback == pf_callback && slot->listeners[i]->p_user_data == p_user_data) {
            /* that's our listener */
            free(slot->listeners[i]);
            slot->listeners.erase(slot->listeners.begin()+i);
            mxMutexUnlock( &p_em->lock );
            break;
        }
    }

    MX_assert_unreachable();
}
