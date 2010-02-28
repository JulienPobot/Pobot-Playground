 #include <pob-eye.h>

 int main(void)
 {
        UInt32 i;
        UInt8 *rgbFrame;
        UInt8 red, green, blue;

        // Init pobeye2 cpu, i2c bus and camera sensor:
        InitPobeye2();
	   InitUART0(115200);
        InitI2C(I2C_100_KHZ);
        // the pobeye2 camera use the i2c bus for setting, you have to configure i2c bus before!
        InitCameraPobeye2();

        // Get pointer to the pobeye2 RGB memory:
        rgbFrame = GetRGBFrame();

        // Main loop:
        while(1)
        {
                // Capture a camera frame and copy it to the pobeye2 memory:
                GrabRGBFrame();

                // Get pixel color:
                for(i = 0 ; i < WIDTH_FRAME * HEIGHT_FRAME ; i++)
                {
                        // For each byte of rgbFrame, get the color:
                        red   = GetRed(rgbFrame[i]);
                        green = GetGreen(rgbFrame[i]);
                        blue  = GetBlue(rgbFrame[i]);
                        // ... then play with the pixel!

				    SendString("Hello world from POB-EYE2!");
                }
        }
        return 0;
 }