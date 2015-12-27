//
//  main.cpp
//  png2c
//
//  Created by Magnus Runesson on 01/12/15.
//  Copyright (c) 2015 Pokewhat. All rights reserved.
//

#include <SDL2/SDL.h>
#include <SDL2_Image/SDL_Image.h>

typedef struct
{
	unsigned short w;
	unsigned short h;
	unsigned short* pixels;
	unsigned char* alpha;
} Image;

Image myImage
{
	10, 10, NULL, NULL
};

unsigned short pixels[] = {
	0x1111,
	0x2222,
	0xffff
};

void writeHeader( FILE* f, char* _symbolNameBase, SDL_Surface* image )
{
	fprintf( f, "#include \"Types.h\"\n" );
	fprintf( f, "#include \"Image.h\"\n" );
	fprintf( f, "\n" );
}

void writePixels( FILE* f, char* _symbolNameBase, SDL_Surface* image )
{
	fprintf( f, "uint16 %s_pixels[] =\n{\n", _symbolNameBase );

	unsigned char* pixels = (unsigned char*)image->pixels;
	int x, y;
	for( y=0; y<image->h; y++ )
	{
		fprintf( f, "\t" );
		for( x=0; x<image->w; x++ )
		{
			int rofs = ((y*image->w)+x)*4;
			unsigned char b = pixels[ rofs+0 ];
			unsigned char g = pixels[ rofs+1 ];
			unsigned char r = pixels[ rofs+2 ];
			//unsigned char a = pixels[ rofs+3 ];
			r >>= 3;
			g >>= 2;
			b >>= 3;
			unsigned short outcol = (r<<11) + (g<<5) + b;
			//printf("r=%i, g=%i, b=%i, a=%i\n", r, g, b, a);
			fprintf( f, "0x%04x,", outcol );
		}
		fprintf( f, "\n" );
	}
	
	fprintf( f, "};\n\n" );
}

void writeAlpha( FILE* f, char* _symbolNameBase, SDL_Surface* image )
{
	fprintf( f, "uint8 %s_alpha[] =\n{\n", _symbolNameBase );
	
	unsigned char* pixels = (unsigned char*)image->pixels;
	int x, y;
	for( y=0; y<image->h; y++ )
	{
		fprintf( f, "\t" );
		for( x=0; x<image->w; x++ )
		{
			int rofs = ((y*image->w)+x)*4;
			unsigned char a = pixels[ rofs+3 ];
			fprintf( f, "0x%02x,", a );
		}
		fprintf( f, "\n" );
	}
	
	fprintf( f, "};\n\n" );
}

void writeImage( FILE* f, char* _symbolNameBase, SDL_Surface* _image )
{
	fprintf( f, "Image %s =\n{\n", _symbolNameBase );
	fprintf( f, "\t%i,%i,\n", _image->w, _image->h );
	fprintf( f, "\t(uint16*)&%s_pixels,\n", _symbolNameBase );
	if( SDL_ISPIXELFORMAT_ALPHA( _image->format->format ))
		fprintf( f, "\t(uint8*)&%s_alpha,\n", _symbolNameBase );
	else
		fprintf( f, "\t(uint8*)0,\n" );
	fprintf( f, "\t(uint8*)\"%s\",\n", _symbolNameBase );
	
	/*
	 
	 Don't output the blitting function anymore. Selecting the correct blit function will need to be done by the game code.
	 
	if( SDL_ISPIXELFORMAT_ALPHA( _image->format->format ))
		fprintf( f, "\t&imageBlitAlpha,\n");
	else
	{
		if((_image->w==96) && (_image->h==64))
		{
			fprintf( f, "\t&imageBlitFullScreen,\n" );
		}
		else
		{
			fprintf( f, "\t&imageBlitNoAlpha,\n" );
		}
	}
	 */
		
	fprintf( f, "};\n" );
}

void writeHeaderFile( FILE* f, char* _symbolNameBase, SDL_Surface* _image )
{
	fprintf( f, "#ifndef %s_data_h\n", _symbolNameBase );
	fprintf( f, "#define %s_data_h\n", _symbolNameBase );
	fprintf( f, "\n" );
	fprintf( f, "extern Image %s;\n", _symbolNameBase );
	fprintf( f, "\n" );
	fprintf( f, "#endif // %s_data_h\n", _symbolNameBase );
}

SDL_Surface* LoadImage( char* _fileName )
{
	SDL_Surface* image = IMG_Load( _fileName );
	printf("Image=0x%016llx\n", (long long)image );
	bool isAlpha = SDL_ISPIXELFORMAT_ALPHA( image->format->format );
	bool isIndexed = SDL_ISPIXELFORMAT_INDEXED( image->format->format );

	printf("w=%i, h=%i, bpp=%i, format=%i, isAlpha=%s, isIndexed=%s\n", image->w, image->h, image->format->BitsPerPixel, image->format->format, isAlpha?"Yes":"No", isIndexed?"Yes":"No" );
	
	return image;
}

FILE* openOutfileC( char* _baseOutFileName )
{
	char outname_c[ 2048 ];
	sprintf( outname_c, "%s.cpp", _baseOutFileName );
	FILE* f = fopen( outname_c, "w" );
	
	return f;
}

FILE* openOutfileH( char* _baseOutFileName )
{
	char outname_c[ 2048 ];
	sprintf( outname_c, "%s.h", _baseOutFileName );
	FILE* f = fopen( outname_c, "w" );
	
	return f;
}

int main( int _numargs, char** _apszArgh )
{
	if( _numargs != 4 )
	{
		printf("Usage error: Program need 3 arguments:\n");
		printf("  png2c <in_file.png> <out_file_base> <symbol_name>\n");
		return -1;
	}

	char* pszInFileName = _apszArgh[ 1 ];
	char* pszOutFilenameBase = _apszArgh[ 2 ];
	char* pszSymbolNameBase = _apszArgh[ 3 ];
	
	//
	SDL_Surface* image = LoadImage( pszInFileName );

	//
	// Write cpp file
	//
	FILE* f = openOutfileC( pszOutFilenameBase );

	writeHeader( f, pszSymbolNameBase, image );
	writePixels( f, pszSymbolNameBase, image );
	if( SDL_ISPIXELFORMAT_ALPHA( image->format->format ))
		writeAlpha( f, pszSymbolNameBase, image );
	writeImage( f, pszSymbolNameBase, image );
	
	fclose( f );
	
	
	//
	// Write h file
	//
	f = openOutfileH( pszOutFilenameBase );
	writeHeaderFile( f, pszSymbolNameBase, image );
	fclose( f );
	
	return 0;
}
