#include "video_device.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#ifdef HAVE_LIBZ
#include <zlib.h>
#endif
#ifdef HAVE_LIBPNG
#include <png.h>
#endif
#ifdef HAVE_LIBJPEG
#include <jpeglib.h>
#endif
//#include "v4l.h"

void
put_image_jpeg (char *image, int width, int height, int quality, int frame)
{
#ifdef HAVE_LIBJPEG
	int y, x, line_width;
	JSAMPROW row_ptr[1];
	struct jpeg_compress_struct cjpeg;
	struct jpeg_error_mgr jerr;
	char *line;

	line = malloc (width * 3);
	if (!line)
		return;
	cjpeg.err = jpeg_std_error(&jerr);
	jpeg_create_compress (&cjpeg);
	cjpeg.image_width = width;
	cjpeg.image_height= height;
	cjpeg.input_components = 3;
	cjpeg.in_color_space = JCS_RGB;
	jpeg_set_defaults (&cjpeg);

	jpeg_set_quality (&cjpeg, quality, TRUE);
	cjpeg.dct_method = JDCT_FASTEST;
	jpeg_stdio_dest (&cjpeg, stdout);

	jpeg_start_compress (&cjpeg, TRUE);

	row_ptr[0] = line;
	line_width = width * 3;
	for ( y = 0; y < height; y++) {
	for (x = 0; x < line_width; x+=3) {
			line[x]   = image[x+2];
			line[x+1] = image[x+1];
			line[x+2] = image[x];
		}
		jpeg_write_scanlines (&cjpeg, row_ptr, 1);
		image += line_width;
	}
	jpeg_finish_compress (&cjpeg);
	jpeg_destroy_compress (&cjpeg);
	free (line);
#else
	fprintf(stderr, "libjpeg not available - cannot write jpeg!\n");
#endif
}

/*
 * write png image to stdout
 */
void
put_image_png (char *image, int width, int height, int frame)
{
#ifdef HAVE_LIBPNG
	int y;
	char *p;
	png_infop info_ptr;
	png_structp png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING,
						NULL, NULL, NULL);
	if (!png_ptr)
		return;
	info_ptr = png_create_info_struct (png_ptr);
	if (!info_ptr)
		return;

	png_init_io (png_ptr, stdout);
	png_set_IHDR (png_ptr, info_ptr, width, height,
					8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_set_bgr (png_ptr);
	png_write_info (png_ptr, info_ptr);
	p = image;
	for (y = 0; y < height; y++) {
		png_write_row (png_ptr, p);
		p+=width*3;
	}
	png_write_end (png_ptr, info_ptr);
#endif
}

/*
 * write ppm image to stdout
 */
#define WRITE_MODE	"w"

int
put_image_ppm (char *image, int width, int height, int binary, int frame)
{
	int x, y, ls=0;
	unsigned char *p = (unsigned char *)image;
	FILE	*out_fp = stdout;
	int	rc = 0;

	if (frame >= 0) {
		/* Output to a file numbered by the frame number */
		char	fn[32];

		sprintf(fn, "frame%d.ppm", frame);

		out_fp = fopen(fn, WRITE_MODE);
		if (!out_fp) {
			fprintf(stderr, "Unable to open %s for write: ", fn);
			perror("");
			return -1;
		}
	}

	if (!binary) {
	fprintf(out_fp, "P3\n%d %d\n%d\n", width, height, 255);
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			fprintf(out_fp, "%03d %03d %03d ", p[2],p[1],p[0]);
			p += 3;
			if (ls++ > 4) {
				fprintf(out_fp, "\n");
				ls = 0;
			}
		}
	}
	fprintf(out_fp, "\n");
	} else {
		unsigned char	buff[3 * width * height];
		unsigned char	*bp = buff;

		fprintf(out_fp, "P6\n%d %d\n%d\n", width, height, 255);
		for (x = 0; x < width * height; x++) {
			*bp++ = p[2];
			*bp++ = p[1];
			*bp++ = p[0];
			p += 3;
		}
		fwrite(buff, width * height, 3, out_fp);
	}

	if (ferror(out_fp)) {
		rc = -1;
	}

	if (frame >= 0) {
		fclose(out_fp);
	} else {
		fflush(out_fp);
	}

	return rc;
}

int
put_hsv_image_ppm (float *image, int width, int height, int binary, int frame)
{
	int x, y, ls=0;
	float *p = image,f;
	FILE	*out_fp = stdout;
	int	rc = 0;

	if (frame >= 0) {
		/* Output to a file numbered by the frame number */
		char	fn[32];

		sprintf(fn, "frame%d.ppm", frame);

		out_fp = fopen(fn, WRITE_MODE);
		if (!out_fp) {
			fprintf(stderr, "Unable to open %s for write: ", fn);
			perror("");
			return -1;
		}
	}

	if (!binary) {
	fprintf(out_fp, "P3\n%d %d\n%d\n", width, height, 360);
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
		        if ((p[0]>340.0||p[0]<25.0) && p[1]>0.7) f=350.0; else f=0.0;
			fprintf(out_fp, "%03.0f %03.0f %03.0f ",f,f,
			f);
			p += 3;
			if (ls++ > 4) {
				fprintf(out_fp, "\n");
				ls = 0;
			}
		}
	}
	fprintf(out_fp, "\n");
	} else {
		unsigned char	buff[3 * width * height];
		unsigned char	*bp = buff;

		fprintf(out_fp, "P6\n%d %d\n%d\n", width, height, 360);
		for (x = 0; x < width * height; x++) {
			*bp++ = p[0];
			*bp++ = p[0];
			*bp++ = p[0];
			p += 3;
		}
		fwrite(buff, width * height, 3, out_fp);
	}

	if (ferror(out_fp)) {
		rc = -1;
	}

	if (frame >= 0) {
		fclose(out_fp);
	} else {
		fflush(out_fp);
	}

	return rc;
}

void GetImageFromFile (char *p, char *path, int width, int height){
	int i,ls=0,x,y;
	FILE *fp;
	
	fp = fopen(path,"r");
	if (fp<0){
		printf("unable to open image file \n");
		exit (1);
	}
	i=0;
	while (i<3) {
		if (fgetc(fp)=='\n') i++;
	}
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			*(p+2)=fgetc(fp);
			*(p+1)=fgetc(fp);
			*p=fgetc(fp);
			p += 3;
		}
	}
	fclose(fp);
}

/* TC : get a filename, open the file and read it into a char *, size of file should be "height*width*3"  */
char * getImageFromFile (const char * fileName, int fileLength) {

	FILE * fd;
	char * image;
	int i;

	image = (char *) malloc (fileLength);
	
	if (image == NULL) {
		fprintf(stderr, "Unable to allocate memory to store image\n");
		return NULL;
	}

	
	fd = fopen(fileName, "r");

	if (fd == NULL) {
		perror("Unable to open image file");
		return NULL; 
	}
	
	/* skip headers */
	if ( fseek(fd, -fileLength, SEEK_END) ) {
		perror ("Error when fseek to skip ppm input image headers");
		fclose(fd);
		return NULL;
	}

	/* read the entire file, one shot ! */
	if ( fread(image, 1, fileLength, fd) != fileLength) {
		fprintf(stderr, "Warning, not enough characters read from input file. Invalid image dimensions ? Invalid format (not ppm) ? \n");
	}
	
	fclose (fd);	
	
	/* seems to need a BGR image instead of RGB for future execution */
	for (i=0; i < fileLength-2; i+=3) {
		char tmpSwap;
		tmpSwap = image[i];
		image[i] = image[i+2];
		image[i+2] = tmpSwap;
	}
	
	return image;
}

