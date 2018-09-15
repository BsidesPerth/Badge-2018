This is a variation of the Adafruit_GFX library. It has been modified to don't broken compatibility with regular one
and uses an experimental text method, faster and support multiple fonts!
 - Faster Circle rendering
 - Faster Font Rendering
 - Multiple Font support
 - Expanded Bitmap support
 - Added ellipse rendering (with fast sin e cos tables)
 - Slight faster drawLine 

The compatibility with the original Adafruit library it's 100% so this one can be used without drawback.
The reason I have modified? Adafruit kindly provide this useful libraries to developers but the rarely update and even less fix errors! I currently use this one in many libraries so I try to maintain better I can to avoid multiple errors in different libraries.
I always update when Adafruit add features so it's always sincronized with the latest adafruit_GFX and you'll never miss the original.
 
If uou are using a library that include this one you can get an error during compilation about glcdfont!!!
Solution it's simple, if you want to use this library you just comment out the include of glcdfont in the library that uses
this version of Adafruit_GFX as:

//#include "glcdfont.c" //comment out if you are using the custom version of Adafruit_GFX!!!!!!!!!
You can also try to use this:

#if !defined(_ADAFRUIT_GFX_VARIANT)
#include "glcdfont.c"
#endif

This library has _ADAFRUIT_GFX_VARIANT defined so you can trigger your code as needed.


That's it.
Many libraries do not need any mod so probably you will not need this.


Below the original text from Adafruit as requested by licence.
--------------------------------------------------------------------------------------------------------------------------
This is the core graphics library for all our displays, providing a common set of graphics primitives (points, lines, circles, etc.).  It needs to be paired with a hardware-specific library for each display device we carry (to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, check license.txt for more information.
All text above must be included in any redistribution.

To download, click the DOWNLOAD ZIP button, uncompress and rename the uncompressed folder Adafruit_GFX. Confirm that the Adafruit_GFX folder contains Adafruit_GFX.cpp and Adafruit_GFX.h

Place the Adafruit_GFX library folder your <arduinosketchfolder>/Libraries/ folder. You may need to create the Libraries subfolder if its your first library. Restart the IDE.
