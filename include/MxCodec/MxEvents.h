/*****************************************************************************
 * vlc_events.h: events definitions
 * Interface used to send events.
 *****************************************************************************
 * Copyright (C) 2007 VLC authors and VideoLAN
 * $Id: 229f4f677029763a837558932dad3e47b283e61c $
 *
 * Authors: Pierre d'Herbemont
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

#ifndef MXEVENTS_H
#define MXEVENTS_H

#include "CMxMeta.h"
#include "../MxSystem/MxThread.h"
#include <vector>

/**
 * \file
 * This file is the interface definition for events
 * (implementation in src/misc/events.c)
 */

/*****************************************************************************
 * Documentation
 *****************************************************************************/
/*
 **** Background
 *
 * This implements a way to send and receive event for an object (which can be
 * a simple C struct or less).
 *
 * This is in direct concurrency with the Variable based Callback
 * (see src/misc/variables.c).
 *
 * It has the following advantages over Variable based Callback:
 * - No need to implement the whole VLC_COMMON_MEMBERS in the object,
 * thus it reduce it size. This is especially true for input_item_t which
 * doesn't have VLC_COMMON_MEMBERS. This is the first reason of existence of
 * this implementation.
 * - Libvlc can easily be based upon that.
 * - Existing event are clearly declared (in include/vlc_events.h)
 *
 *
 **** Example usage
 *
 * (vlc_cool_object_t doesn't need to have the VLC_COMMON_MEMBERS.)
 *
 * struct vlc_cool_object_t
 * {
 *        ...
 *        vlc_event_manager_t p_event_manager;
 *        ...
 * }
 *
 * vlc_my_cool_object_new()
 * {
 *        ...
 *        vlc_event_manager_init( &p_self->p_event_manager, p_self, p_a_libvlc_object );
 *        ...
 * }
 *
 * vlc_my_cool_object_release()
 * {
 *         ...
 *         vlc_event_manager_fini( &p_self->p_event_manager );
 *         ...
 * }
 *
 * vlc_my_cool_object_do_something()
 * {
 *        ...
 *        vlc_event_t event;
 *        event.type = vlc_MyCoolObjectDidSomething;
 *        event.u.my_cool_object_did_something.what_it_did = kSomething;
 *        vlc_event_send( &p_self->p_event_manager, &event );
 * }
 * */

  /*****************************************************************************
 * Event Type
 *****************************************************************************/

/* List of event */
enum MxEventType {
    /* Input item events */
    MxEventType_InputItemMetaChanged,
    MxEventType_InputItemSubItemTreeAdded,
    MxEventType_InputItemDurationChanged,
    MxEventType_InputItemPreparsedChanged,
    MxEventType_InputItemNameChanged,
    MxEventType_InputItemInfoChanged,
    MxEventType_InputItemErrorWhenReadingChanged,
    MxEventType_InputItemPreparseEnded,
};

struct MxEvent;
/* Event callback type */
typedef void ( *mx_event_callback_t )( const MxEvent *, void * );

struct mx_event_listener_t
{
    void *               p_user_data;
    mx_event_callback_t pf_callback;
};

struct mx_event_listeners_group_t
{
    //DECL_ARRAY(struct mx_event_listener_t *) listeners;
    std::vector<mx_event_listener_t*> listeners;
};

/* Event manager type */
struct MxEventManager
{
    void * p_obj;
    MxMutex lock;
    mx_event_listeners_group_t events[MxEventType_InputItemPreparseEnded + 1];
};

/* Event definition */
struct MxEvent
{
    MxEventType type;
    void * p_obj; /* Sender object, automatically filled by vlc_event_send() */
    union mx_event_type_specific
    {
        /* Input item events */
        struct mx_input_item_meta_changed
        {
            MxMetaType meta_type;
        } input_item_meta_changed;
        struct mx_input_item_subitem_added
        {
            //input_item_t * p_new_child;
        } input_item_subitem_added;
        struct mx_input_item_subitem_tree_added
        {
            //input_item_node_t * p_root;
        } input_item_subitem_tree_added;
        struct mx_input_item_duration_changed
        {
            mtime_t new_duration;
        } input_item_duration_changed;
        struct mx_input_item_preparsed_changed
        {
            int new_status;
        } input_item_preparsed_changed;
        struct mx_input_item_name_changed
        {
            const char * new_name;
        } input_item_name_changed;
        struct mx_input_item_info_changed
        {
            void * unused;
        } input_item_info_changed;
        struct input_item_error_when_reading_changed
        {
            bool new_value;
        } input_item_error_when_reading_changed;
        struct input_item_preparse_ended
        {
            int new_status;
        } input_item_preparse_ended;
    } u;
};

 /*****************************************************************************
 * Event manager
 *****************************************************************************/

/*
 * p_obj points to the object that owns the event manager, and from
 * which events are sent
 */
void mxEventManagerInit( MxEventManager * p_em, void * p_obj );

/*
 * Destroy
 */
void mxEventManagerFini( MxEventManager * p_em );

/*
 * Send an event to the listener attached to this p_em.
 */
void mxEventSend( MxEventManager * p_em, MxEvent * ); 

/*
 * Add a callback for an event.
 */
MXSYSTEM_API int mxEventAttach( MxEventManager * p_event_manager,
                              MxEventType event_type,
                              mx_event_callback_t pf_callback,
                              void *p_user_data );

/*
 * Remove a callback for an event.
 */
MXSYSTEM_API void mxEventDetach( MxEventManager *p_event_manager,
                               MxEventType event_type,
                               mx_event_callback_t pf_callback,
                               void *p_user_data );

#endif /* MXEVENTS_H */
