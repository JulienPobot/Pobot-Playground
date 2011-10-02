/* ATTENTION : le pont en position 1 est un cas particulier !!! Il n'y a alors pas de foss� � gauche, mais un petit � droite de l'image ! */


/* valeurs de retour, les positions sont num�rot�es de gauche � droite � partir du point de vue du pont */
#define NOT_PONT 0
#define PONT_1 1
#define PONT_2 2
#define PONT_3 3
#define PONT_4 4

/* caract�ristiques g�n�rales du foss� entre le bord du terrain et le pont (positionn� al�atoirement) */
/* surface min avec pont en position 1 (ou 2) */
#define FOSSE_SURFACE_MIN 200
/* surface max avec pont en position 4 */
#define FOSSE_SURFACE_MAX 3000

/* position min max en X du DEBUT du foss� */
/* en position 2, 3, 4 */
#define FOSSE_X_MIN 115
#define FOSSE_X_MAX 125 
/* en position 1 */
#define FOSSE_X_POS_1_MIN 300 
#define FOSSE_X_POS_1_MAX 310 

/* position min max en Y du HAUT du foss� */
#define FOSSE_Y_MIN 50
#define FOSSE_Y_MAX 100

/* la taille minimale d'un foss� : c'est l'espace minimum si on a le pont en position 2 */
/* IMPORTANT : cette constante sert � d�finir la position des ponts car elle donne la position du pont 2 */
#define FOSSE_WIDTH_MIN 35
/* la taille maximale du foss� : c'est l'espace maximum quand on a le pont en position 4 */
#define FOSSE_WIDTH_MAX 190

/* idem mais pour le pont 1 */
#define FOSSE_WIDTH_POS_1_MIN 10
#define FOSSE_WIDTH_POS_1_MAX 30

	
/* controles de la taille en Y du foss� (attention, pas de DELTA sur mes Y) valeurs absolues */
#define FOSSE_HEIGHT_MIN 10
#define FOSSE_HEIGHT_MAX 30

/* la largeur d'un espace entre 2 positions possibles de pont */
#define DELTA_PONT 75

/* le nombre de pixels qui autours desquels l'espace de pont est consid�r� comme valide */
/* exemple pour pont 1 : FOSSE_WIDTH_POS_1_MIN - DELTA_ESPACE < ESPACE_VALIDE < FOSSE_WIDTH_POS_1_MIN + DELTA_ESPACE */
/* exemple pour pont 2 : FOSSE_WIDTH_MIN - DELTA_ESPACE < ESPACE_VALIDE < FOSSE_WIDTH_MIN + DELTA_ESPACE */
/* exemple pour pont 3 : FOSSE_WIDTH_MIN + DELTA_PONT - DELTA_ESPACE < ESPACE_VALIDE < FOSSE_WIDTH_MIN + DELTA_PONT + DELTA_ESPACE */
/* exemple pour pont 4 : FOSSE_WIDTH_MIN + (2 * DELTA_PONT) - DELTA_ESPACE < ESPACE_VALIDE < FOSSE_WIDTH_MIN + (2 * DELTA_PONT) + DELTA_ESPACE */
#define DELTA_ESPACE 15

/* returns one of : NOT_PONT, PONT_1, PONT_2, PONT_3, PONT_4 */
int detectPonts(int x1, int x2, int y1, int y2, int surface);
