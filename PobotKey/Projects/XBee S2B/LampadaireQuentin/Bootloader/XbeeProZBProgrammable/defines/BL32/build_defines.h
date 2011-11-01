/*-------------------------------------------------------
* Copyright (C) 2009 Digi International, All Rights Reserved.
*
*
* This software is provided as instructional material without charge 
* by Digi International for use by its employees and customers
* subject to the following terms.
*
* PERMISSION
* Permission is hereby granted, free of charge, to any person obtaining 
* a copy of this software, to deal with it without restriction, 
* including without limitation the rights to use, copy,  modify, merge, publish, 
* distribute, sublicense, and/or sell copies of it, and to permit persons to 
* whom it is furnished to do so, provided the above copyright notice 
* and this permission notice are included in all derived works
* and the use of this software is restricted to Digi products.
*
* WARRANTY
* THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, OR NONINFRINGEMENT.  
*
* LIABILITY
* IN NO EVENT SHALL DIGI INTERNATIONAL BE LIABLE FOR ANY CLAIM, DAMAGES, 
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, 
* ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE, OR THE USE 
* OR OTHER DEALINGS WITH THE SOFTWARE.
*
*-------------------------------------------------------*/

/**********************************************************************
*
*  This file contains #defines that change the compilation of the build
*  There should be a different build_defines.h for each build type
*  Cannot be located in a sub-folder in the source directory.
*
***********************************************************************
*
*
*                      MC9S08_BL32.mcp
*
*/

/* defines here are for changing compile options for different builds */


/*BL = Bootloader type; this is based on the processor size of flash*/
//#define BL128
//#define BL96
//#define BL64
#define BL32
//#define BL16
