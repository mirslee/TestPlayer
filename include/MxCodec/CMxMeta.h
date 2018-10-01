/*****************************************************************************
 * vlc_meta.h: Stream meta-data
 *****************************************************************************
 *
 *****************************************************************************/

#ifndef CMXMETA_H
#define CMXMETA_H 1

#include <map>
#include <vector>
#include "CMxObject.h"
//#include "vlc_input_item.h"

/**
 * \file
 * This file defines functions and structures for stream meta-data in MX
 *
 */

enum MxMetaType
{
    MxMetaType_Title = 0,
    MxMetaType_Artist,
    MxMetaType_Genre,
    MxMetaType_Copyright,
    MxMetaType_Album,
    MxMetaType_TrackNumber,
    MxMetaType_Description,
    MxMetaType_Rating,
    MxMetaType_Date,
    MxMetaType_Setting,
    MxMetaType_URL,
    MxMetaType_Language,
    MxMetaType_NowPlaying,
    MxMetaType_ESNowPlaying,
    MxMetaType_Publisher,
    MxMetaType_EncodedBy,
    MxMetaType_ArtworkURL,
    MxMetaType_TrackID,
    MxMetaType_TrackTotal,
    MxMetaType_Director,
    MxMetaType_Season,
    MxMetaType_Episode,
    MxMetaType_ShowName,
    MxMetaType_Actors,
    MxMetaType_AlbumArtist,
    MxMetaType_DiscNumber,
    MxMetaType_DiscTotal
};

#define MX_META_TYPE_COUNT 27

#define ITEM_PREPARSED       1
#define ITEM_ART_FETCHED     2
#define ITEM_ART_NOTFOUND    4

/**
 * Basic function to deal with meta
 */
class MXCODEC_API CMxMeta
{
private:
    std::string ppsz_meta[MX_META_TYPE_COUNT];
    std::map<std::string, std::string> extra_tags;
    int i_status;
    
public:
    CMxMeta();
    
    void set(MxMetaType meta_type, const char *psz_val );
    std::string get(MxMetaType meta_type);

    void addExtra(const char *psz_name, const char *psz_value);
    std::string getExtra(const char *psz_name);
    unsigned int extraCount();
    
    /**
     * Allocate a copy of all extra meta names and a table with it.
     * Be sure to free both the returned pointers and its name.
     */
    std::vector<std::string> copyExtraNames();
    void merge(const CMxMeta * pOther);
    int status();
    void setStatus(int status);
};

/**
 * Returns a localizes string describing the meta
 */
std::string mxMetaTypeToLocalizedString( MxMetaType meta_type );



typedef struct meta_export_t
{
    MX_COMMON_MEMBERS
    //input_item_t *p_item;
    const char *psz_file;
} meta_export_t;

//MXCORE_API int input_item_WriteMeta(CMxObject *, CMxInputItem *);

/* Setters for meta.
 * Warning: Make sure to use the input_item meta setters (defined in vlc_input_item.h)
 * instead of those one. */
#define mx_meta_SetTitle( meta, b )       meta->( MxMetaType_Title, b )
#define mx_meta_SetArtist( meta, b )      meta->( MxMetaType_Artist, b )
#define mx_meta_SetGenre( meta, b )       meta->( MxMetaType_Genre, b )
#define mx_meta_SetCopyright( meta, b )   meta->( MxMetaType_Copyright, b )
#define mx_meta_SetAlbum( meta, b )       meta->( MxMetaType_Album, b )
#define mx_meta_SetTrackNum( meta, b )    meta->( MxMetaType_TrackNumber, b )
#define mx_meta_SetDescription( meta, b ) meta->( MxMetaType_Description, b )
#define mx_meta_SetRating( meta, b )      meta->( MxMetaType_Rating, b )
#define mx_meta_SetDate( meta, b )        meta->( MxMetaType_Date, b )
#define mx_meta_SetSetting( meta, b )     meta->( MxMetaType_Setting, b )
#define mx_meta_SetURL( meta, b )         meta->( MxMetaType_URL, b )
#define mx_meta_SetLanguage( meta, b )    meta->( MxMetaType_Language, b )
#define mx_meta_SetNowPlaying( meta, b )  meta->( MxMetaType_NowPlaying, b )
#define mx_meta_SetPublisher( meta, b )   meta->( MxMetaType_Publisher, b )
#define mx_meta_SetEncodedBy( meta, b )   meta->( MxMetaType_EncodedBy, b )
#define mx_meta_SetArtURL( meta, b )      meta->( MxMetaType_ArtworkURL, b )
#define mx_meta_SetTrackID( meta, b )     meta->( MxMetaType_TrackID, b )
#define mx_meta_SetTrackTotal( meta, b )  meta->( MxMetaType_TrackTotal, b )
#define mx_meta_SetDirector( meta, b )    meta->( MxMetaType_Director, b )
#define mx_meta_SetSeason( meta, b )      meta->( MxMetaType_Season, b )
#define mx_meta_SetEpisode( meta, b )     meta->( MxMetaType_Episode, b )
#define mx_meta_SetShowName( meta, b )    meta->( MxMetaType_ShowName, b )
#define mx_meta_SetActors( meta, b )      meta->( MxMetaType_Actors, b )
#define mx_meta_SetAlbumArtist( meta, b ) meta->( MxMetaType_AlbumArtist, b )
#define mx_meta_SetDiscNumber( meta, b )  meta->( MxMetaType_DiscNumber, b )

#define MX_META_TITLE              mxMetaTypeToLocalizedString( MxMetaType_Title )
#define MX_META_ARTIST             mxMetaTypeToLocalizedString( MxMetaType_Artist )
#define MX_META_GENRE              mxMetaTypeToLocalizedString( MxMetaType_Genre )
#define MX_META_COPYRIGHT          mxMetaTypeToLocalizedString( MxMetaType_Copyright )
#define MX_META_ALBUM              mxMetaTypeToLocalizedString( MxMetaType_Album )
#define MX_META_TRACK_NUMBER       mxMetaTypeToLocalizedString( MxMetaType_TrackNumber )
#define MX_META_DESCRIPTION        mxMetaTypeToLocalizedString( MxMetaType_Description )
#define MX_META_RATING             mxMetaTypeToLocalizedString( MxMetaType_Rating )
#define MX_META_DATE               mxMetaTypeToLocalizedString( MxMetaType_Date )
#define MX_META_SETTING            mxMetaTypeToLocalizedString( MxMetaType_Setting )
#define MX_META_URL                mxMetaTypeToLocalizedString( MxMetaType_URL )
#define MX_META_LANGUAGE           mxMetaTypeToLocalizedString( MxMetaType_Language )
#define MX_META_NOW_PLAYING        mxMetaTypeToLocalizedString( MxMetaType_NowPlaying )
#define MX_META_PUBLISHER          mxMetaTypeToLocalizedString( MxMetaType_Publisher )
#define MX_META_ENCODED_BY         mxMetaTypeToLocalizedString( MxMetaType_EncodedBy )
#define MX_META_ART_URL            mxMetaTypeToLocalizedString( MxMetaType_ArtworkURL )
#define MX_META_TRACKID            mxMetaTypeToLocalizedString( MxMetaType_TrackID )
#define MX_META_DIRECTOR           mxMetaTypeToLocalizedString( MxMetaType_Director )
#define MX_META_SEASON             mxMetaTypeToLocalizedString( MxMetaType_Season )
#define MX_META_EPISODE            mxMetaTypeToLocalizedString( MxMetaType_Episode )
#define MX_META_SHOW_NAME          mxMetaTypeToLocalizedString( MxMetaType_ShowName )
#define MX_META_ACTORS             mxMetaTypeToLocalizedString( MxMetaType_Actors )
#define MX_META_ALBUMARTIST        mxMetaTypeToLocalizedString( MxMetaType_AlbumArtist )
#define MX_META_DISCNUMBER         mxMetaTypeToLocalizedString( MxMetaType_DiscNumber )

#define MX_META_EXTRA_MB_ALBUMID   "MB_ALBUMID"

#endif
