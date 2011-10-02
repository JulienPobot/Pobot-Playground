void put_image_jpeg (char *image, int width, int height, int quality, int frame);
void put_image_png (char * image, int width, int height, int frame);
int put_image_ppm (char * image, int width, int height, int binary, int frame);
int put_hsv_image_ppm (float *image, int width, int height, int binary, int frame);
void GetImageFromFile (char *p, char *path, int width, int height);
/* TC : get a filename, open the file and read it into a char *, size of file should be "height*width*3"  */
char * getImageFromFile (const char * fileName, int fileLength);
