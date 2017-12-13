#ifndef _SDL_SAVEPNG
#define _SDL_SAVEPNG
/*
 * SDL_SavePNG -- libpng-based SDL_Surface writer.
 *
 * This code is free software, available under zlib/libpng license.
 * http://www.libpng.org/pub/png/src/libpng-LICENSE.txt
 */
#include <SDL_video.h>

/*
 * Save an SDL_Surface as a PNG file.
 *
 * Returns 0 success or -1 on failure, the error message is then retrievable
 * via SDL_GetError().
 */
#define SDL_SavePNG(surface, file) \
	SDL_SavePNG_RW(surface, SDL_RWFromFile(file, "wb"), 1)


int SDL_SavePNG_RW(SDL_Surface *surface, SDL_RWops *rw, int freedst);

SDL_Surface *SDL_PNGFormatAlpha(SDL_Surface *src);

#endif
