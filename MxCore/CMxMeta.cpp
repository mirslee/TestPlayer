/*****************************************************************************
 * meta.c : Metadata handling
 *****************************************************************************
 * Copyright (C) 1998-2004 VLC authors and VideoLAN
 * $Id: 241b926079ff186952b18c3ff7dedab377ea6705 $
 *
 * Authors: Antoine Cellerier <dionoea@videolan.org>
 *          Clément Stenac <zorglub@videolan.org
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
#include "CMxMeta.h"
#include "MxConfig.h"
#include "MxCommon.h"
#include "MxError.h"

#include "../MxSystem/MxFixups.h"
#include <assert.h>

//#include <vlc_playlist.h>
#include "../MxSystem/MxUrl.h"
//#include <vlc_arrays.h>
//#include <vlc_modules.h>
#include "../MxSystem/MxCharSet.h"

//#include "input_internal.h"
//#include "../playlist/art.h"

#define gettext_noop(str) (str)
#define N_(str)           gettext_noop (str)

/* FIXME bad name convention */
std::string mxMetaTypeToLocalizedString( MxMetaType meta_type )
{
	switch (meta_type)
	{
	case MxMetaType_Title:
		return N_("Title");
	case MxMetaType_Artist:
		return N_("Artist");
	case MxMetaType_Genre:
		return N_("Genre");
	case MxMetaType_Copyright:
		return N_("Copyright");
	case MxMetaType_Album:
		return N_("Album");
	case MxMetaType_TrackNumber:
		return N_("Track number");
	case MxMetaType_Description:
		return N_("Description");
	case MxMetaType_Rating:
		return N_("Rating");
	case MxMetaType_Date:
		return N_("Date");
	case MxMetaType_Setting:
		return N_("Setting");
	case MxMetaType_URL:
		return N_("URL");
	case MxMetaType_Language:
		return N_("Language");
	case MxMetaType_NowPlaying:
		return N_("Now Playing");
	case MxMetaType_ESNowPlaying:
		return N_("Now Playing");
	case MxMetaType_Publisher:
		return N_("Publisher");
	case MxMetaType_EncodedBy:
		return N_("Encoded by");
	case MxMetaType_ArtworkURL:
		return N_("Artwork URL");
	case MxMetaType_TrackID:
		return N_("Track ID");
	case MxMetaType_TrackTotal:
		return N_("Number of Tracks");
	case MxMetaType_Director:
		return N_("Director");
	case MxMetaType_Season:
		return N_("Season");
	case MxMetaType_Episode:
		return N_("Episode");
	case MxMetaType_ShowName:
		return N_("Show Name");
	case MxMetaType_Actors:
		return N_("");
	case MxMetaType_AlbumArtist:
		return N_("Actors");
	case MxMetaType_DiscNumber:
		return N_("Album Artist");
	case MxMetaType_DiscTotal:
		return N_("Disc number");
	default:
		return N_("");
	}
};


CMxMeta::CMxMeta() {
    i_status = 0;
}

/**
 * vlc_meta has two kinds of meta, the one in a table, and the one in a
 * dictionary.
 * FIXME - Why don't we merge those two?
 */

void CMxMeta::set(MxMetaType meta_type, const char *psz_val )
{
    assert( psz_val == NULL || IsUTF8( psz_val ) );
    ppsz_meta[meta_type] = psz_val ? strdup( psz_val ) : "";
}

string CMxMeta::get(MxMetaType meta_type )
{
    return ppsz_meta[meta_type];
}

void CMxMeta::addExtra(const char *psz_name, const char *psz_value)
{
    extra_tags.insert(std::pair<string, string>(psz_name, strdup(psz_value)));
}

string CMxMeta::getExtra(const char *psz_name )
{
    std::map<string, string>::iterator iter = extra_tags.find(psz_name);
    if(iter != extra_tags.end())
        return iter->second;
    
    return "";
}

unsigned int CMxMeta::extraCount()
{
    return extra_tags.size();
}

std::vector<std::string> CMxMeta::copyExtraNames()
{
    std::vector<std::string> vec;
    std::map<std::string, std::string>::iterator iter = extra_tags.begin();
    while (iter != extra_tags.end()) {
        vec.push_back(iter->first);
        iter++;
    }
    return vec;
}

int CMxMeta::status()
{
    return i_status;
}

void CMxMeta::setStatus(int status )
{
    this->i_status = status;
}

void CMxMeta::merge(const CMxMeta * pOther)
{
    if (this == pOther)
        return;

    for( int i = 0; i < MX_META_TYPE_COUNT; i++ )
        this->ppsz_meta[i] = pOther->ppsz_meta[i];

    std::map<std::string, std::string>::iterator iter = extra_tags.begin();
    while (iter != extra_tags.end()) {
       this->extra_tags.insert(std::pair<string, string>(iter->first, iter->second));
        iter++;
    }
}


//void input_ExtractAttachmentAndCacheArt( input_thread_t *p_input, const char *name )
//{
//    input_item_t *p_item = input_priv(p_input)->p_item;
//
//    if( input_item_IsArtFetched( p_item ) )
//    {   /* XXX Weird, we should not end up with attachment:// art URL
//         * unless there is a race condition */
//        msg_Warn( p_input, "art already fetched" );
//        if( likely(playlist_FindArtInCache( p_item ) == VLC_SUCCESS) )
//            return;
//    }
//
//    /* */
//    input_attachment_t *p_attachment = NULL;
//
//    vlc_mutex_lock( &p_item->lock );
//    for( int i_idx = 0; i_idx < input_priv(p_input)->i_attachment; i_idx++ )
//    {
//        input_attachment_t *a = input_priv(p_input)->attachment[i_idx];
//
//        if( !strcmp( a->psz_name, name ) )
//        {
//            p_attachment = vlc_input_attachment_Duplicate( a );
//            break;
//        }
//    }
//    vlc_mutex_unlock( &p_item->lock );
//
//    if( p_attachment == NULL )
//    {
//        msg_Warn( p_input, "art attachment %s not found", name );
//        return;
//    }
//
//    /* */
//    const char *psz_type = NULL;
//
//    if( !strcmp( p_attachment->psz_mime, "image/jpeg" ) )
//        psz_type = ".jpg";
//    else if( !strcmp( p_attachment->psz_mime, "image/png" ) )
//        psz_type = ".png";
//    else if( !strcmp( p_attachment->psz_mime, "image/x-pict" ) )
//        psz_type = ".pct";
//
//    playlist_SaveArt( VLC_OBJECT(p_input), p_item,
//                      p_attachment->p_data, p_attachment->i_data, psz_type );
//    vlc_input_attachment_Delete( p_attachment );
//}

//int input_item_WriteMeta( CMxObject *obj, CMxInputItem *p_item )
//{
    /*meta_export_t *p_export = vlc_custom_create( obj, sizeof( *p_export ), "meta writer" );
    if( p_export == NULL )
        return MX_ENOMEM;
    p_export->p_item = p_item;

    int type;
    mxMutexLock( &p_item->lock );
    type = p_item->i_type;
    vlc_mutex_unlock( &p_item->lock );
    if( type != ITEM_TYPE_FILE )
        goto error;

    char *psz_uri = input_item_GetURI( p_item );
    p_export->psz_file = vlc_uri2path( psz_uri );
    if( p_export->psz_file == NULL )
        msg_Err( p_export, "cannot write meta to remote media %s", psz_uri );
    free( psz_uri );
    if( p_export->psz_file == NULL )
        goto error;

    module_t *p_mod = module_need( p_export, "meta writer", NULL, false );
    if( p_mod )
        module_unneed( p_export, p_mod );
    vlc_object_release( p_export );
    return VLC_SUCCESS;

error:
    vlc_object_release( p_export );*/
//    return MX_EGENERIC;
//}

//void vlc_audio_replay_gain_MergeFromMeta( audio_replay_gain_t *p_dst, const vlc_meta_t *p_meta )
//{
//    const char * psz_value;
//
//    if( !p_meta )
//        return;
//
//    if( (psz_value = vlc_meta_GetExtra(p_meta, "REPLAYGAIN_TRACK_GAIN")) ||
//        (psz_value = vlc_meta_GetExtra(p_meta, "RG_RADIO")) )
//    {
//        p_dst->pb_gain[AUDIO_REPLAY_GAIN_TRACK] = true;
//        p_dst->pf_gain[AUDIO_REPLAY_GAIN_TRACK] = us_atof( psz_value );
//    }
//
//    if( (psz_value = vlc_meta_GetExtra(p_meta, "REPLAYGAIN_TRACK_PEAK" )) ||
//             (psz_value = vlc_meta_GetExtra(p_meta, "RG_PEAK" )) )
//    {
//        p_dst->pb_peak[AUDIO_REPLAY_GAIN_TRACK] = true;
//        p_dst->pf_peak[AUDIO_REPLAY_GAIN_TRACK] = us_atof( psz_value );
//    }
//
//    if( (psz_value = vlc_meta_GetExtra(p_meta, "REPLAYGAIN_ALBUM_GAIN" )) ||
//             (psz_value = vlc_meta_GetExtra(p_meta, "RG_AUDIOPHILE" )) )
//    {
//        p_dst->pb_gain[AUDIO_REPLAY_GAIN_ALBUM] = true;
//        p_dst->pf_gain[AUDIO_REPLAY_GAIN_ALBUM] = us_atof( psz_value );
//    }
//
//    if( (psz_value = vlc_meta_GetExtra(p_meta, "REPLAYGAIN_ALBUM_PEAK" )) )
//    {
//        p_dst->pb_peak[AUDIO_REPLAY_GAIN_ALBUM] = true;
//        p_dst->pf_peak[AUDIO_REPLAY_GAIN_ALBUM] = us_atof( psz_value );
//    }
//}

