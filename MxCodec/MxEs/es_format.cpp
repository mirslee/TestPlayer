#include "stdafx.h"
#include "MxConfig.h"
#include <assert.h>
#include "MxCommon.h"
#include "MxEs.h"
#include "MxFourcc.h"


void MxEsFormat::init(int i_cat, MxFourcc i_codec )
{
    memset(&this->data, 0, sizeof(Data));
    this->data.i_cat                  = (es_format_category_e)i_cat;
    this->data.i_codec                = i_codec;
    this->data.i_profile              = -1;
    this->data.i_level                = -1;
    this->data.i_id                   = -1;
    this->data.i_priority             = ES_PRIORITY_SELECTABLE_MIN;
    this->data.psz_language           = NULL;
    this->data.psz_description        = NULL;
    this->data.p_extra_languages      = NULL;

    /*if (this->data.i_cat == VIDEO_ES)
        video_format_Init(&this->data.video, 0);*/

    this->data.b_packetized           = true;
    this->data.p_extra                = NULL;
}

void MxEsFormat::initFromVideo(const MxVideoFormat *pVideoFormat )
{
    init(VIDEO_ES, pVideoFormat->data.i_chroma );
    data.video.copy(pVideoFormat);
}

int MxEsFormat::copy(const MxEsFormat *pOther)
{
    int ret = MX_SUCCESS;
    this->data = pOther->data;

    if (pOther->data.psz_language != NULL)
    {
        this->data.psz_language = strdup(pOther->data.psz_language);
        if (unlikely(this->data.psz_language == NULL))
            ret = MX_ENOMEM;
    }
    if (pOther->data.psz_description != NULL)
    {
        this->data.psz_description = strdup(pOther->data.psz_description);
        if (unlikely(this->data.psz_description == NULL))
            ret = MX_ENOMEM;
    }

    if (pOther->data.i_extra > 0)
    {
        assert(pOther->data.p_extra != NULL);
        this->data.p_extra = malloc( pOther->data.i_extra );

        if( likely(this->data.p_extra != NULL) )
            memcpy(this->data.p_extra, pOther->data.p_extra, pOther->data.i_extra);
        else
        {
            this->data.i_extra = 0;
            ret = MX_ENOMEM;
        }
    }
    else
        this->data.p_extra = NULL;

    if (pOther->data.i_cat == VIDEO_ES)
        this->data.video.copy(&pOther->data.video);

    if (pOther->data.i_cat == SPU_ES)
    {
        if (pOther->data.subs.psz_encoding != NULL)
        {
            this->data.subs.psz_encoding = strdup(pOther->data.subs.psz_encoding);
            if (unlikely(this->data.subs.psz_encoding == NULL))
                ret = MX_ENOMEM;
        }
        if (pOther->data.subs.p_style != NULL)
        {
            this->data.subs.p_style = text_style_Duplicate(pOther->data.subs.p_style);
            if (unlikely(this->data.subs.p_style == NULL))
                ret = MX_ENOMEM;
        }
    }

    if (pOther->data.i_extra_languages > 0)
    {
        assert(pOther->data.p_extra_languages != NULL);
        this->data.p_extra_languages = (extra_languages_t*)calloc(this->data.i_extra_languages,
                                        sizeof (*this->data.p_extra_languages));
        if (likely(this->data.p_extra_languages != NULL))
        {
            for (unsigned i = 0; i < this->data.i_extra_languages; i++)
            {
                if (pOther->data.p_extra_languages[i].psz_language != NULL)
                    this->data.p_extra_languages[i].psz_language = strdup(pOther->data.p_extra_languages[i].psz_language);
                if (pOther->data.p_extra_languages[i].psz_description != NULL)
                    this->data.p_extra_languages[i].psz_description = strdup(pOther->data.p_extra_languages[i].psz_description);
            }
            this->data.i_extra_languages = pOther->data.i_extra_languages;
        }
        else
        {
            this->data.i_extra_languages = 0;
            ret = MX_ENOMEM;
        }
    }
    return ret;
}

void MxEsFormat::clean()
{
    free(this->data.psz_language);
    free(this->data.psz_description);
    assert(this->data.i_extra == 0 || this->data.p_extra != NULL);
    free(this->data.p_extra);

    if (this->data.i_cat == VIDEO_ES)
        this->data.video.clean();
    if (this->data.i_cat == SPU_ES)
    {
        free(this->data.subs.psz_encoding);

        if (this->data.subs.p_style != NULL)
            text_style_Delete(this->data.subs.p_style);
    }

    for (unsigned i = 0; i < this->data.i_extra_languages; i++)
    {
        free(this->data.p_extra_languages[i].psz_language);
        free(this->data.p_extra_languages[i].psz_description);
    }
    free(this->data.p_extra_languages);

    /* es_format_Clean can be called multiple times */
    init(UNKNOWN_ES, 0);
}

bool MxEsFormat::isSimilar( const MxEsFormat *pOther)
{
    if( this->data.i_cat != pOther->data.i_cat ||
        mxFourccGetCodec( this->data.i_cat, this->data.i_codec ) !=
        mxFourccGetCodec( pOther->data.i_cat, pOther->data.i_codec ) )
        return false;

    switch( this->data.i_cat )
    {
    case AUDIO_ES:
    {
        MxAudioFormat a1 = this->data.audio;
        MxAudioFormat a2 = pOther->data.audio;

        if( a1.i_format && a2.i_format && a1.i_format != a2.i_format )
            return false;
        if( a1.channel_type != a2.channel_type ||
            a1.i_rate != a2.i_rate ||
            a1.i_channels != a2.i_channels ||
            a1.i_physical_channels != a2.i_physical_channels ||
            a1.i_chan_mode != a2.i_chan_mode )
            return false;
        if( this->data.i_profile != pOther->data.i_profile )
            return false;
        return true;
    }

    case VIDEO_ES:
    {
        MxVideoFormat v1 = this->data.video;
        MxVideoFormat v2 = pOther->data.video;
        if( !v1.data.i_chroma )
            v1.data.i_chroma = mxFourccGetCodec( this->data.i_cat, this->data.i_codec );
        if( !v2.data.i_chroma )
            v2.data.i_chroma = mxFourccGetCodec( pOther->data.i_cat, pOther->data.i_codec );
        return this->data.video.isSimilar(&pOther->data.video);
    }

    case SPU_ES:
    default:
        return true;
    }
}

void MxEsFormat::change( int i_cat,MxFourcc i_codec ) {
    clean();
    init(i_cat, i_codec);
}

