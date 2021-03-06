//
//  colorproc.h
//  
//
//  Created by Maurizio Giordano on 20/02/14.
//
//

#ifndef ____colorproc__
#define ____colorproc__

#include <iostream>
#include "wisard.hpp"

extern long int hits;
extern long int misses;

#endif /* defined(____colorproc__) */

cache_entry_t *makeTupleRGB(cache_entry_t *, unsigned char, unsigned char, unsigned char, int, int, int, pix_t **);
cache_entry_t *makeTupleCachedRGB(cache_entry_t *, unsigned char, unsigned char, unsigned char, int, int, int, pix_t **);
cache_entry_t *makeTupleCachedGRAY(cache_entry_t *, unsigned char, int, int, int, pix_t **);

