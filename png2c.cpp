//
//  main.cpp
//  png2c
//
//  Created by Magnus Runesson on 01/12/15.
//  Copyright (c) 2015 Pokewhat. All rights reserved.
//

#include <SDL2/SDL.h>
#include <SDL_Image/SDL_Image.h>

int main( int _numargs, char** _apszArgh )
{
	if( _numargs != 3 )
	{
		printf("Usage error: Program need 3 arguments:\n");
		printf("  png2c <in_file.png> <out_file_base> <symbol_name>\n");
		return -1;
	}

	/*
	int iarg;
	for( iarg=0; iarg<_numargs; iarg++ )
	{
		printf("Arg %i: %s\n", iarg, _apszArgh[ iarg ]);
	}
	 */
	
	IMG_Load( _apszArgh[ 1 ]);
	return 0;
}
