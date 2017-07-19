//
//  wisard.hpp
//  
//
//  Created by Maurizio Giordano on 18/01/13.
//
//

#ifndef _wisard_h
#define _wisard_h

#define PI 3.1415926535
// magic cache size is 10
#define MAXCACHESIZE 1024
#define MAXQUEUESIZE 1024
#define setblack(x) { x = (char)0; }
#define setwhite(x) { x = (char)255; }

#include "wnet_lib.hpp"
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

typedef unsigned char uchar;

typedef struct pixel pix_t;
struct pixel {
    long int x;
    long int y;
};

typedef struct wisard wisard_t;
typedef struct cache_entry cache_entry_t;
struct cache_entry {
    int *tuple;
    int cr;
    int cg;
    int cb;
    cache_entry_t *next;
    cache_entry_t *prev;
};

typedef struct queue_entry queue_entry_t;
struct queue_entry {
    int *tuple;
    int index;
    queue_entry_t *next;
    queue_entry_t *prev;
};

struct wisard {
    wentry_t ***net;
    cache_entry_t **cachearray;
    queue_entry_t **queuearray;
    pix_t **neigh_map;
    char **neigh_img;
    int n_ram;
    int n_bit;
    int nt;
    int neigh_w;
    int neigh_h;
} ;

wisard_t *net_create(int, int, int, string, int, int, int);
pix_t **neigh_mapping(int, int, int, int);
cache_entry_t *cacheinit(int, int);
queue_entry_t *queueinit(int, int);
void queueempty(queue_entry_t*,int);
void printCache(cache_entry_t *cache);
pix_t newpix(long int,long int);
void pixels_print(pix_t*, int);
#endif
