#include <stdio.h>
#include <stdlib.h>
#include <cc65.h>
#include <conio.h>
#include <ctype.h>
#include <modload.h>
#include <tgi.h>



#ifndef DYN_DRV
#  define DYN_DRV       1
#endif

#define COLOR_BACK      TGI_COLOR_BLACK
#define COLOR_FORE      TGI_COLOR_WHITE


/*****************************************************************************/
/*                                   Data                                    */
/*****************************************************************************/



/* Driver stuff */
static unsigned MaxX;
static unsigned MaxY;
static unsigned AspectRatio;



/*****************************************************************************/
/*                                   Code                                    */
/*****************************************************************************/

typedef struct point point;

struct point {
  unsigned int x;
  unsigned int y;
};

point vertex[3];



static void CheckError (const char* S)
{
    unsigned char Error = tgi_geterror ();

    if (Error != TGI_ERR_OK) {
        printf ("%s: %u\n", S, Error);
        if (doesclrscrafterexit ()) {
            cgetc ();
        }
        exit (EXIT_FAILURE);
    }
}



#if DYN_DRV
static void DoWarning (void)
/* Warn the user that the dynamic TGI driver is needed for this program */
{
    printf ("Warning: This program needs the TGI\n"
            "driver on disk! Press 'y' if you have\n"
            "it - any other key exits.\n");
    if (tolower (cgetc ()) != 'y') {
        exit (EXIT_SUCCESS);
    }
    printf ("OK. Please wait patiently...\n");
}
#endif


static void sierpinski(void)
{
    static const unsigned char Palette[2] = { TGI_COLOR_WHITE, TGI_COLOR_BLACK };
    int random_index;
    point current, next, midpoint;
    const unsigned Min = (MaxX < MaxY) ? MaxX : MaxY;

    vertex[0].x = MaxX / 2;
    vertex[0].y = 0;

    vertex[1].x = 0;
    vertex[1].y = MaxY;

    vertex[2].x = MaxX;
    vertex[2].y = MaxY;

    tgi_setpalette (Palette);
    tgi_setcolor (COLOR_FORE);
    tgi_clear ();

    current.x = MaxX/2;
    current.y = MaxY/2;

    while (1) {

      random_index = rand() % 3;

      next.x = vertex[random_index].x;
      next.y = vertex[random_index].y;
      
      midpoint.x = ( current.x + next.x ) / 2;
      midpoint.y = ( current.y + next.y ) / 2;

      tgi_setpixel( midpoint.x, midpoint.y );

      current.x = midpoint.x;
      current.y = midpoint.y;
    }

    // cgetc ();
}



int main (void)
{
    unsigned char Border;

#if DYN_DRV
    /* Warn the user that the tgi driver is needed */
    DoWarning ();

    /* Load and initialize the driver */
    tgi_load_driver (tgi_stddrv);
    CheckError ("tgi_load_driver");
#else
    /* Install the driver */
    tgi_install (tgi_static_stddrv);
    CheckError ("tgi_install");
#endif

    tgi_init ();
    CheckError ("tgi_init");

    /* Get stuff from the driver */
    MaxX = tgi_getmaxx ();
    MaxY = tgi_getmaxy ();
    AspectRatio = tgi_getaspectratio ();

    /* Set the palette, set the border color */
    Border = bordercolor(COLOR_BLACK);

    /* Do graphics stuff */
    sierpinski();

#if DYN_DRV
    /* Unload the driver */
    tgi_unload ();
#else
    /* Uninstall the driver */
    tgi_uninstall ();
#endif

    /* Reset the border */
    (void) bordercolor (Border);

    /* Done */
    printf ("Done\n");
    return EXIT_SUCCESS;
}
