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
	const int boxSamples = 3; //side-length, 2 = 4xAA, 3=9xAA, etc
    for( unsigned int i = 0; i < panoWidth; ++i)
    {
		
		for( unsigned int j = 0; j < panoHeight; ++j )
        {
			int accumColors[3] = { 0, 0, 0 }; //note: if using more than 2^(32-4) samples, this may overflow on white images

			for( int sampleX = 0; sampleX < boxSamples; sampleX++ )
			{
				for( int sampleY = 0; sampleY < boxSamples; sampleY++ )
				{
					int xx = 0;
					int yy = 0;
					RGBQUAD sample;

					double ii = i;
					double jj = j;
					ii += (double) sampleX / (double) boxSamples;
					jj += (double) sampleY / (double) boxSamples;

					algo.calXY( ii, jj, xx, yy );

					if( !FreeImage_GetPixelColor( bmpCube[algo.cubeFaceId], xx, yy, &sample ) )
					{
						printf( "Failed to get pixel color\n" );
						return 1;
					}
					accumColors[0] += sample.rgbRed;
					accumColors[1] += sample.rgbGreen;
					accumColors[2] += sample.rgbBlue;
				}
			}
			RGBQUAD generated;
			generated.rgbRed = accumColors[0] / ( boxSamples * boxSamples );
			generated.rgbGreen = accumColors[1] / ( boxSamples * boxSamples );
			generated.rgbBlue = accumColors[2] / ( boxSamples * boxSamples );

			if( !FreeImage_SetPixelColor( output, i, j, &generated ) )
			{
				printf( "Failed to set pixel color 2\n" );
				return 1;
			}
        }

    }

	FreeImage_Save( FIF_BMP, output,"pano.bmp" );

	FreeImage_DeInitialise();

    return 0;
}
