//
//  colorproc.cpp
//  
//
//  Created by Maurizio Giordano on 20/02/14.
//
//

#include "colorproc.hpp"


cache_entry_t *makeTupleRGB(cache_entry_t *cache, unsigned char R, unsigned char G, unsigned char B, int den, int nt, int nram, pix_t **neigh_map) {
    int cr = (int)(R / den);
    int cg = (int)(G / den);
    int cb = (int)(B / den);
    register int neuron;
    
    // initialize intuple
    for (neuron=0;neuron<nram;neuron++) cache->tuple[neuron] = 0;
    for (int k=0;k<nt;k++) {
        if (k<cr) cache->tuple[neigh_map[0][k].x] |= neigh_map[0][k].y;
        if (k<cg) cache->tuple[neigh_map[1][k].x] |= neigh_map[1][k].y;
        if (k<cb) cache->tuple[neigh_map[2][k].x] |= neigh_map[2][k].y;
    }
}

// Make tuple with Cache support
cache_entry_t *makeTupleCachedRGB(cache_entry_t *cache, unsigned char R, unsigned char G, unsigned char B, int den, int nt, int nram, pix_t **neigh_map) {
    cache_entry_t *p;
    int cr = (int)(R / den);
    int cg = (int)(G / den);
    int cb = (int)(B / den);
    register int neuron, k;
    int tmp, *ptr;
    // scan cache for hit or miss
    p = cache;
    try {
    for (;;) {
        if (p->cr == cr && p->cg == cg && p->cb == cb) {  // cache hit (move found in front)
            //printf("HIT:  ");
            hits++;
            if (p == cache) return cache;
            cache = cache->prev;
            tmp = cache->cr;
            cache->cr = cr;
            p->cr = tmp;
            tmp = cache->cg;
            cache->cg = cg;
            p->cg = tmp;
            tmp = cache->cb;
            cache->cb = cb;
            p->cb = tmp;
            ptr = cache->tuple;
            cache->tuple = p->tuple;
            p->tuple = ptr;
            return cache;
        }
        if (p->next == cache) {
            // move top on first non-empty
            //printf("MISS: ");
            misses++;
            cache = cache->prev;
            cache->cr = cr;
            cache->cg = cg;
            cache->cb = cb;
            // initialize tuple
            for (neuron=0;neuron<nram;neuron++) cache->tuple[neuron] = 0;
            // compute tuple
            for (k=0;k<nt;k++) {
                if (k<cr) cache->tuple[neigh_map[0][k].x] |= neigh_map[0][k].y;
                if (k<cg) cache->tuple[neigh_map[1][k].x] |= neigh_map[1][k].y;
                if (k<cb) cache->tuple[neigh_map[2][k].x] |= neigh_map[2][k].y;
            }
            return cache;
        }
        p = p->next;
    }
    } catch (const std::exception &e) {
        cout << e.what() << endl;
    }
}

cache_entry_t *makeTupleCachedGRAY(cache_entry_t *cache, unsigned char val, int den, int nt, int nram, pix_t **neigh_map) {
    cache_entry_t *p;
    int cval = (int)(val / den);
    register int neuron, k;
    int tmp, *ptr;
    // scan cache for hit or miss
    p = cache;
    try {
        for (;;) {
            if (p->cr == cval) {  // cache hit (move found in front)
                //printf("HIT:  ");
                hits++;
                if (p == cache) return cache;
                cache = cache->prev;
                tmp = cache->cr;
                cache->cr = cval;
                p->cr = tmp;
                ptr = cache->tuple;
                cache->tuple = p->tuple;
                p->tuple = ptr;
                return cache;
            }
            if (p->next == cache) {
                // move top on first non-empty
                //printf("MISS: ");
                misses++;
                cache = cache->prev;
                cache->cr = cval;
                // initialize tuple
                for (neuron=0;neuron<nram;neuron++) cache->tuple[neuron] = 0;
                // compute tuple
                for (k=0;k<nt;k++) {
                    if (k<cval) cache->tuple[neigh_map[0][k].x] |= neigh_map[0][k].y;
                }
                return cache;
            }
            p = p->next;
        }
    } catch (const std::exception &e) {
        cout << e.what() << endl;
    }
}
