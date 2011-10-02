/*
 * ccvt.h
 * This file is part of PhilipsWebCamLib
 *
 * PhilipsWebCamLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * PhilipsWebCamLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PhilipsWebCamLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * PhilipsWebCamLib - Version 1.0.0
 * Release Date: 9/02
 * Author : Derrick Parkhurst
 * Copyright (c) 2002 Derrick Parkhurst <derrick.parkhurst@jhu.edu>
 * All Rights Reserved.
 *
 *
 * This code was derived from the GNU GPL project 
 * Colour ConVerT (C) 2001-2002 Nemosoft Unv.  nemosoft@smcc.demon.nl
 *
 */

/*   Format descriptions:
   420i = "4:2:0 interlaced"
           YYYY UU YYYY UU   even lines
           YYYY VV YYYY VV   odd lines
           U/V data is subsampled by 2 both in horizontal 
           and vertical directions, and intermixed with the Y values.
   
   420p = "4:2:0 planar"
           YYYYYYYY      N lines
           UUUU          N/2 lines
           VVVV          N/2 lines
           U/V is again subsampled, but all the Ys, Us and Vs are placed 
           together, but in separate buffers. The buffers may be placed in
           one piece of contiguous memory though, with Y buffer first,
           followed by U, folloed by V.

   yuyv = "4:2:2 interlaced"
           YUYV YUYV YUYV ...   N lines
           The U/V data is subsampled by 2 in horizontal direction only.

   bgr24 = 3 bytes per pixel, in the order Blue Green Red (whoever came up
           with that idea...)
   rgb24 = 3 bytes per pixel, in the order Red Green Blue (which is sensible)
   rgb32 = 4 bytes per pixel, in the order Red Green Blue Alpha, with 
           Alpha really being a filler byte (0)
   bgr32 = last but not least, 4 bytes per pixel, in the order Blue Green Red
           Alpha, Alpha again a filler byte (0)
 */

/* Functions in ccvt.S */
/* 4:2:0 YUV interlaced to RGB/BGR */
void ccvt_420i_bgr24(int width, int height, void *src, void *dst);
void ccvt_420i_rgb24(int width, int height, void *src, void *dst);
void ccvt_420i_bgr32(int width, int height, void *src, void *dst);
void ccvt_420i_rgb32(int width, int height, void *src, void *dst);

/* 4:2:0 YUYV interlaced to RGB/BGR */
void ccvt_yuyv_rgb32(int width, int height, void *src, void *dst);
void ccvt_yuyv_bgr32(int width, int height, void *src, void *dst);

/* 4:2:0 YUV planar to RGB/BGR     */
void ccvt_420p_rgb32(int width, int height, void *srcy, void *srcu, void *srcv, void *dst);
void ccvt_420p_bgr32(int width, int height, void *srcy, void *srcu, void *srcv, void *dst);
void ccvt_420p_bgr24(int width, int height, void *srcy, void *srcu, void *srcv, void *dst);
void ccvt_420p_rgb24(int width, int height, void *srcy, void *srcu, void *srcv, void *dst);

/* RGB/BGR to 4:2:0 YUV interlaced */

/* RGB/BGR to 4:2:0 YUV planar     */
void ccvt_rgb24_420p(int width, int height, void *src, void *dsty, void *dstu, void *dstv);
void ccvt_bgr24_420p(int width, int height, void *src, void *dsty, void *dstu, void *dstv);

/* Go from 420i to other yuv formats */
void ccvt_420i_420p(int width, int height, void *src, void *dsty, void *dstu, void *dstv);
void ccvt_420i_yuyv(int width, int height, void *src, void *dst);

