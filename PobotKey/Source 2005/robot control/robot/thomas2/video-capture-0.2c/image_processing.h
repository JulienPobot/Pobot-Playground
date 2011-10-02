
#define MAX_POS 8000
#define NIL 0
#define NB_OBJECT_MAX 100
typedef
 struct{
     int x,y;
     int surface;
     int x1,y1,x2,y2;
     long x_bary,y_bary;
     int d[MAX_POS] ;
 } freeman_chain;

typedef
 struct{
 	int x1,y1;
	int x2,y2;
	int x_bary,y_bary;
	int surface;
	int nb_pos;
	int color;
	int etat_quille;
 } object;

void mediane_filtering(char *image, int width, int height);

void contour_enhance(char *image, int width, int height);

void contrast_enhance(char *image, int width, int height);

void stretching_histo(char *image, int width, int height,int d);

void shrinking_histo(char *image, int width, int height,int d);


void rgb2hsv(char *image, float *hsv, int width, int height);

int freeman_search(float *hsv, freeman_chain *chain, int y1, int width, int height, int *couleur);

void rgb_write(	char *image, int x, int y, int width, int height,
		unsigned char r, unsigned char g, unsigned char b);

int freeman(char *image, int width, int height, object *a_object, int display);

void display(char *image, object *a_object, int nb_object, int width, int height);


