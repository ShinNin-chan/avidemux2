/**/
/***************************************************************************
                          DIA_hue
                             -------------------

			   Ui for hue & sat

    begin                : 08 Apr 2005
    copyright            : (C) 2004/7 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "DIA_flyDialogQt4.h"
#include "ADM_default.h"
#include "ADM_image.h"
#include "DIA_flyHue.h"
#include "../ADM_vidHue.h"

/************* COMMON PART *********************/
uint8_t  flyHue::update(void)
{
    return 1;
}        
/**
    \fn processYuv
*/
uint8_t   flyHue::processYuv(ADMImage *in,ADMImage *out )
{
    uint8_t *src,*dst;
    uint32_t stride;
    int isin,icos;

    ADMVideoHue::update(&param,&isin,&icos);
    out->copyPlane(in,out,PLANAR_Y);
    // Do it!
    ADMVideoHue::HueProcess_C(out->GetWritePtr(PLANAR_V), out->GetWritePtr(PLANAR_U),
                 in->GetReadPtr(PLANAR_V), in->GetReadPtr(PLANAR_U),
                 out->GetPitch(PLANAR_U),in->GetPitch(PLANAR_U), // assume u&v pitches are =
                 in->GetWidth(PLANAR_U),in->GetHeight(PLANAR_U),
                 isin, icos);
    // Copy half source to display
    in->copyLeftSideTo(out);
    out->printString(1,1,"Original"); // printString can't handle non-ascii input, do not translate this!
    out->printString(in->GetWidth(PLANAR_Y)/24+1,1,"Processed"); // as above, don't try to translate

    return 1;
}
//EOF
