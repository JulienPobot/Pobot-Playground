/**********************************************************************
*
*  This file contains #defines that change the compilation of the build
*  There should be a different build_defines.h for each build type
*  Cannot be located in a sub-folder in the source directory.
*
***********************************************************************
*
*
*                      App32Transparent.mcp
*
*/

#ifndef BUILD_DEFINES
 #define BUILD_DEFINES

/* the BL## stands for Bootloader 
   the ## stands for the flash size 16k 32k
   Only one should be selected as both RAM and Flash sizes change
*/
// #define BL16
 #define BL32  




 #ifdef BL16
  #define APP_RESERVED_FLASH_START_ADDRESS 0xC000
  #define APP_PROGRAM_FLASH_START_ADDRESS  0xC400
 #endif
 #ifdef BL32
  #define APP_RESERVED_FLASH_START_ADDRESS 0x8000
  #define APP_PROGRAM_FLASH_START_ADDRESS  0x8400
 #endif
 





#endif //BUILD_DEFINES