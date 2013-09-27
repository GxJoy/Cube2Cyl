#include "Cube2Cyl.h"
#include "Freeimage.h"
#include <stdio.h>

using namespace std;

char* cubeNames[CUBE_FACE_NUM] =
{
    "top.bmp",
    "left.bmp",
    "front.bmp",
    "right.bmp",
    "back.bmp",
    "down.bmp"
};

int main()
{
	FreeImage_Initialise();

	FIBITMAP *bmpCube[CUBE_FACE_NUM];

	// read the 6 images
    for (int i = 0; i < CUBE_FACE_NUM; ++i)
    {
		bmpCube[i] = FreeImage_Load( FIF_BMP, cubeNames[i] );

		if( !bmpCube[i] )
        {
			printf( "Failed to load image %d\n", i );
            return 1;
        }		
    }

	/* Get image's dimensions */
	int width = FreeImage_GetWidth( bmpCube[0] );
	int height = FreeImage_GetHeight( bmpCube[0] );

    // the input images must be square
    if (width != height)
    {
		printf( "Error: unsopported dimensions.\n" );
        return 1;
    }

    // map the 6 images to 3D space
    Cube2Cyl algo;

    algo.init(width, M_PI, 2.0*M_PI);

    unsigned int panoWidth  = algo.pxPanoSizeH;
    unsigned int panoHeight = algo.pxPanoSizeV;

    // create panorama image
	FIBITMAP* output = FreeImage_AllocateT( FIT_BITMAP, panoWidth, panoHeight, 24 );

    // process
    for (int i = 0; i < panoWidth; ++i)
    {
        for (int j = 0; j < panoHeight; ++j)
        {
			int xx = 0;
			int yy = 0;

            algo.calXY(i, j, xx, yy);

			RGBQUAD sample;
			BOOL flag1 = FreeImage_GetPixelColor( bmpCube[algo.cubeFaceId], xx, yy, &sample );

			BOOL flag2 = FreeImage_SetPixelColor( output, i, j, &sample );

			if( flag1 == FALSE || flag2 == FALSE )
			{
				printf( "Failed to set pixel color\n" );
				return 1;
			}
        }
    }

	FreeImage_Save( FIF_BMP, output,"pano.bmp" );

	FreeImage_DeInitialise();

    return 0;
}
