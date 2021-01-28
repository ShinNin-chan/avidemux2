/***************************************************************************
                          Charcoal filter 
    Algorithm:
        Copyright (C) 2003-2020 Meltytech, LLC
    Ported to Avidemux:
        Copyright 2021 szlldm
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#define _USE_MATH_DEFINES // some compilers do not export M_PI etc.. if GNU_SOURCE or that is defined, let's do that
#include <math.h>
#include "ADM_default.h"
#include "ADM_coreVideoFilter.h"
#include "ADM_coreVideoFilterInternal.h"
#include "DIA_factory.h"
#include "artCharcoal.h"
#include "artCharcoal_desc.cpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

extern uint8_t DIA_getArtCharcoal(artCharcoal *param, ADM_coreVideoFilter *in);
/**
    \class ADMVideoArtCharcoal
*/
class  ADMVideoArtCharcoal:public ADM_coreVideoFilter
{

  protected:
    void            update(void);
    artCharcoal     _param;
    int32_t         _scatterX;
    int32_t         _scatterY;
    float           _intensity;
    float           _color;
    bool            _invert;
    ADMImage        *work;
  public:
    ADMVideoArtCharcoal(ADM_coreVideoFilter *in,CONFcouple *couples);
    ~ADMVideoArtCharcoal();

    virtual const char   *getConfiguration(void);          /// Return  current configuration as a human readable string
    virtual bool         getNextFrame(uint32_t *fn,ADMImage *image);    /// Return the next image
    virtual bool         getCoupledConf(CONFcouple **couples) ;   /// Return the current filter configuration
    virtual void         setCoupledConf(CONFcouple *couples);
    virtual bool         configure(void) ;                 /// Start graphical user interface

  private:
    float   valueLimit(float val, float min, float max);
    int32_t valueLimit(int32_t val, int32_t min, int32_t max);
};



// Add the hook to make it valid plugin
//DECLARE_VIDEO_FILTER(   ADMVideoArtCharcoal,   // Class
DECLARE_VIDEO_FILTER_PARTIALIZABLE(   ADMVideoArtCharcoal,   // Class
                                      1,0,0,              // Version
                                      ADM_UI_TYPE_BUILD,         // UI
                                      VF_ART,            // Category
                                      "artCharcoal",            // internal name (must be uniq!)
                                      QT_TRANSLATE_NOOP("artCharcoal","Charcoal / Chalkboard"),            // Display name
                                      QT_TRANSLATE_NOOP("artCharcoal","Ported from MLT.") // Description
                                  );
/**
    \fn ArtCharcoalProcess_Sqrti
*/
float ArtCharcoalProcess_Sqrti( int n )
{
    int p = 0;
    int q = 1;
    int r = n;
    int h = 0;

    while( q <= n )
        q = q << 2;

    while( q != 1 )
    {
        q = q >> 2;
        h = p + q;
        p = p >> 1;
        if ( r >= h )
        {
            p = p + q;
            r = r - h;
        }
    }

    return (float)p;
}
/**
    \fn ArtCharcoalProcess_C
*/
void ArtCharcoalProcess_C(ADMImage *img, ADMImage *tmp, int32_t scatterX, int32_t scatterY, float intensity, float color, bool invert)
{
    if (!img || !tmp) return;
    int width=img->GetWidth(PLANAR_Y); 
    int height=img->GetHeight(PLANAR_Y);
    int stride, istride;
    uint8_t * ptr, * iptr;
    float pixel;
    uint8_t pixelU8;
    int matrix[ 3 ][ 3 ];
    int sum1;
    int sum2;
    float sum;
    int val;

    if(img->_range != ADM_COL_RANGE_MPEG)
        img->shrinkColorRange();

    tmp->copyPlane(img,tmp,PLANAR_Y);

    // Y plane
    stride=tmp->GetPitch(PLANAR_Y);
    ptr=tmp->GetWritePtr(PLANAR_Y);
    istride=img->GetPitch(PLANAR_Y);
    iptr=img->GetWritePtr(PLANAR_Y);
    for(int y=0;y<height;y++)
    {
        for (int x=0;x<width;x++)
        {
            if (((x - scatterX) >= 0) && ((x + scatterX) < width) && ((y - scatterY) >= 0) && ((y + scatterY) < height))    // safe range for speed-up
            {
  #define get_Y(j,k) (ptr[(k)*stride + (j)])
                matrix[ 0 ][ 0 ] = get_Y(x - scatterX, y - scatterY );
                matrix[ 0 ][ 1 ] = get_Y(x           , y - scatterY );
                matrix[ 0 ][ 2 ] = get_Y(x + scatterX, y - scatterY );
                matrix[ 1 ][ 0 ] = get_Y(x - scatterX, y            );
                matrix[ 1 ][ 2 ] = get_Y(x + scatterX, y            );
                matrix[ 2 ][ 0 ] = get_Y(x - scatterX, y + scatterY );
                matrix[ 2 ][ 1 ] = get_Y(x           , y + scatterY );
                matrix[ 2 ][ 2 ] = get_Y(x + scatterX, y + scatterY );
  #undef get_Y
            } else {
  #define get_Y(j,k) ( (((j)<0) || ((j)>=width) || ((k)<0) || ((k)>=height)) ? 235 : (ptr[(k)*stride + (j)]) )
                matrix[ 0 ][ 0 ] = get_Y(x - scatterX, y - scatterY );
                matrix[ 0 ][ 1 ] = get_Y(x           , y - scatterY );
                matrix[ 0 ][ 2 ] = get_Y(x + scatterX, y - scatterY );
                matrix[ 1 ][ 0 ] = get_Y(x - scatterX, y            );
                matrix[ 1 ][ 2 ] = get_Y(x + scatterX, y            );
                matrix[ 2 ][ 0 ] = get_Y(x - scatterX, y + scatterY );
                matrix[ 2 ][ 1 ] = get_Y(x           , y + scatterY );
                matrix[ 2 ][ 2 ] = get_Y(x + scatterX, y + scatterY );
  #undef get_Y
            }

            sum1 = (matrix[2][0] - matrix[0][0]) + ( (matrix[2][1] - matrix[0][1]) << 1 ) + (matrix[2][2] - matrix[2][0]);
            sum2 = (matrix[0][2] - matrix[0][0]) + ( (matrix[1][2] - matrix[1][0]) << 1 ) + (matrix[2][2] - matrix[2][0]);
            sum = intensity * ArtCharcoalProcess_Sqrti( sum1 * sum1 + sum2 * sum2 );
            if (sum < 16.0) sum = 16.0;
            if (sum > 235.0) sum = 235.0;
            pixelU8 = (uint8_t)std::round(sum);
            if (!invert) pixelU8 = 251 - pixelU8;
            iptr[x] = pixelU8;
        }
        iptr+=istride;
    }

    // UV planes
    for (int p=1; p<3; p++)
    {
        stride=img->GetPitch((ADM_PLANE)p);
        ptr=img->GetWritePtr((ADM_PLANE)p);
        for(int y=0;y<height/2;y++)	// 4:2:0
        {
            for (int x=0;x<width/2;x++)
            {
                pixel = ptr[x];
                pixel -= 128;
                ptr[x] = (uint8_t)std::round( (pixel * color) + 128);
            }
            ptr+=stride;
        }
    }

}

/**
    \fn configure
*/
bool ADMVideoArtCharcoal::configure()
{
    uint8_t r=0;

    r=  DIA_getArtCharcoal(&_param, previousFilter);
    if(r) update();
    return r;
}
/**
    \fn getConfiguration
*/
const char   *ADMVideoArtCharcoal::getConfiguration(void)
{
    static char s[256];
    snprintf(s,255,"%s, Scatter X:%d Y:%d, Intensity:%.2f, Color: %.2f",(_param.invert ? "Chalkboard" : "Charcoal"), _param.scatterX, _param.scatterY, _param.intensity, _param.color);
    return s;
}
/**
    \fn ctor
*/
ADMVideoArtCharcoal::ADMVideoArtCharcoal(  ADM_coreVideoFilter *in,CONFcouple *couples)  :ADM_coreVideoFilter(in,couples)
{
    if(!couples || !ADM_paramLoad(couples,artCharcoal_param,&_param))
    {
        _param.scatterX = 2;
        _param.scatterY = 2;
        _param.intensity = 1.0;
        _param.color = 0.0;
        _param.invert = false;
    }
    work=new ADMImageDefault(info.width,info.height);
    update();
}
/**
    \fn valueLimit
*/
float ADMVideoArtCharcoal::valueLimit(float val, float min, float max)
{
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}
/**
    \fn valueLimit
*/
int32_t ADMVideoArtCharcoal::valueLimit(int32_t val, int32_t min, int32_t max)
{
    if (val < min) val = min;
    if (val > max) val = max;
    return val;
}
/**
    \fn update
*/
void ADMVideoArtCharcoal::update(void)
{
    _scatterX=valueLimit(_param.scatterX,0,10);
    _scatterY=valueLimit(_param.scatterY,0,10);
    _intensity=valueLimit(_param.intensity,0.0,1.0);
    _color=valueLimit(_param.color,0.0,1.0);
    _invert=_param.invert;
}
/**
    \fn dtor
*/
ADMVideoArtCharcoal::~ADMVideoArtCharcoal()
{
    if(work) delete work;
    work=NULL;
}
/**
    \fn getCoupledConf
*/
bool ADMVideoArtCharcoal::getCoupledConf(CONFcouple **couples)
{
    return ADM_paramSave(couples, artCharcoal_param,&_param);
}

void ADMVideoArtCharcoal::setCoupledConf(CONFcouple *couples)
{
    ADM_paramLoad(couples, artCharcoal_param, &_param);
}

/**
    
*/

/**
    \fn getNextFrame
    \brief
*/
bool ADMVideoArtCharcoal::getNextFrame(uint32_t *fn,ADMImage *image)
{
    /*
    ADMImage *src;
    src=vidCache->getImage(nextFrame);
    if(!src)
        return false; // EOF
    *fn=nextFrame++;
    image->copyInfo(src);
    image->copyPlane(src,image,PLANAR_Y); // Luma is untouched
    src = image;

    DoFilter(...);

    vidCache->unlockAll();
    */
    if(!previousFilter->getNextFrame(fn,image)) return false;

    ArtCharcoalProcess_C(image, work,_scatterX, _scatterY, _intensity, _color, _invert);

    return 1;
}

