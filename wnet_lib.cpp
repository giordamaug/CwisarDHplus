//
//  wnet_lib.c
//
//
//  Created by Maurizio Giordano on 02/02/13.
//
// a library for ram creation, initialization and print
// rams are circular lists of pairs key value
//

#include <string>
#include "wnet_lib.hpp"



// create (and init) a ram
wentry_t *wram_create() {
    wentry_t *m;
    m = (wentry_t *)malloc(sizeof(wentry_t));
    m->key = -1;
    m->value = (wvalue_t)0;
    m->next = m;
    m->prev = m;
    return m;
}
    
// set ram entry to "value"
// if ram is empty, initalize it
void wram_set(wentry_t *m, wkey_t key,wvalue_t value) {
        wentry_t *p, *newp;
    
        // circulate in list for insertion
        p = m;
        for (;;) {
            p = p->next;
            if (p ==m || key <= p->key) break;
        }
        // the key already exists (update)
        if (key == p->key) {
            //printf("key matched\n");
            // delete item if value is null
            if (value==0) {
                (p->prev)->next = p->next;
                (p->next)->prev = p->prev;
                wcounter--;
                m->value--;
                free(p);
            } else {
                p->value = value;
            }
        // the key does not exist (insertion)
        } else {
            wcounter++;
            //printf("new key\n");
            newp = (wentry_t *)malloc(sizeof(wentry_t));            
            newp->key = key;
            newp->value = value;
            newp->next = p;
            newp->prev = p->prev;
            (p->prev)->next = newp;
            p->prev = newp;
            m->value++;
        }
        return;
}

// put ram entry to zero
void wram_del(wentry_t *m, wkey_t key) {
    wentry_t *p;
    
    // circulate in list for insertion
    p = m;
    for (;;) {
        p = p->next;
        if (key == p->key) {
            printf("delete key\n");
            (p->prev)->next = p->next;
            (p->next)->prev = p->prev;
            wcounter--;
            m->value--;
            free(p);
            break;
        }
        if (p ==m) break;
    }
}

// get ram entry
wvalue_t wram_get(wentry_t *m, wkey_t key) {
    wentry_t *p;
    
    // circulate in list for insertion
    p = m;
    for (;;) {
        p = p->next;
        if (key == p->key) return p->value;
        if (p ==m) return (wvalue_t)0;
    }
}

// if ram entry exists increment its value by "incr"
// otherwise ram entry is inserted and set to "value"
void wram_set_or_incr(wentry_t *m, wkey_t key, wvalue_t value, wvalue_t incr) {
    wentry_t *p, *newp;
    
    // circulate in list for insertion
    p = m;
    for (;;) {
        p = p->next;
        if (p ==m || key <= p->key) break;
        //if (key >= p->key) break;
    }
    // the key already exists (update)
    if (key == p->key) {
        //printf("key matched\n");
        p->value += incr;
        // the key does not exist (insertion)
    } else {
        wcounter++;
        //printf("new key\n");
        newp = (wentry_t *)malloc(sizeof(wentry_t));
        newp->key = key;
        newp->value = value;
        newp->next = p;
        newp->prev = p->prev;
        (p->prev)->next = newp;
        p->prev = newp;
        m->value++;
    }
    return;
}

// if ram entry exists increment its value by "incr"
// otherwise ram entry is inserted and set to "value"
void wram_incr(wentry_t *m, wkey_t key) {
    wentry_t *p, *newp;
    
    // circulate in list for insertion
    p = m;
    for (;;) {
        p = p->next;
        if (p ==m || key <= p->key) break;
        //if (key >= p->key) break;
    }
    // the key already exists (update)
    if (key == p->key) {
        //printf("key matched\n");
        p->value += (wvalue_t)1;
        // the key does not exist (insertion)
    } else {
        wcounter++;
        //printf("new key\n");
        newp = (wentry_t *)malloc(sizeof(wentry_t));
        newp->key = key;
        newp->value = (wvalue_t)1;
        newp->next = p;
        newp->prev = p->prev;
        (p->prev)->next = newp;
        p->prev = newp;
        m->value++;
    }
    return;
}

void wram_decr(wentry_t *m,wkey_t key) {
    wentry_t *p;
    
    // circulate in list for insertion
    p = m;
    for (;;) {
        p = p->next;
        if (p ==m) break;
        if (key == p->key) {
            if ((p->value - (wvalue_t)1) <= 0) {
                // remove entry
                //printf("delete key\n");
                (p->prev)->next = p->next;
                (p->next)->prev = p->prev;
                wcounter--;
                m->value--;
                free(p);
            } else {
                // decrease the entry
                //printf("decr 1 key\n");
                p->value -= (wvalue_t)1;
            }
            break;
        }
    }
    return;
}

void wram_decr_all_but_key(wentry_t *m,wkey_t key) {
    wentry_t *p, *newp;
    bool notfound = true;
    // circulate in list for insertion
    p = m;
    for (;;) {
        p = p->next;
        if (p ==m) break;
        if (key == p->key) {
            notfound = false;
            //printf("key matched\n");
            p->value += (wvalue_t)1;
            // the key does not exist (insertion)
        } else {
            if ((p->value - (wvalue_t)1) <= 0) {
                // remove entry
                //printf("delete key\n");
                (p->prev)->next = p->next;
                (p->next)->prev = p->prev;
                wcounter--;
                m->value--;
                free(p);
            } else {
                // decrease the entry
                //printf("decr 1 key\n");
                p->value -= (wvalue_t)1;
            }
        }
    }
    if (notfound) {
        wcounter++;
        //printf("new key\n");
        newp = (wentry_t *)malloc(sizeof(wentry_t));
        newp->key = key;
        newp->value = (wvalue_t)1;
        newp->next = m;
        newp->prev = m->prev;
        (m->prev)->next = newp;
        m->prev = newp;
        m->value++;
    }
    return;
}

void wram_decr_or_del(wentry_t *m,wkey_t key,wvalue_t decr) {
    wentry_t *p;
    
    // circulate in list for insertion
    p = m;
    for (;;) {
        p = p->next;
        if (p ==m) break;
        if (key == p->key) {
            if ((p->value - decr) <= 0) {
                // remove entry
                printf("delete key\n");
                (p->prev)->next = p->next;
                (p->next)->prev = p->prev;
                wcounter--;
                m->value--;
                free(p);
            } else {
                // decrease the entry
                printf("decr 1 key\n");
                p->value -= decr;
            }
            break;
        }
    }
    return;
}

// count non-zero ram entries
unsigned long wram_len(wentry_t *m) {
    wentry_t *p;
    int cnt=0;
    
    // circulate in list for insertion
    p = m;
    for (;;) {
        p = p->next;
        if (p ==m) break;
        else cnt++;
    }
    return cnt;
}

wentry_t *wram_copy(wentry_t *m) {
    wentry_t *p, *cp;
    
    cp = wram_create();
    cp->value = m->value;
    for(p=m;;p=p->next) {
        if (p->next==m) break;
        wram_set(cp, p->next->key, p->next->value);
    }
    return cp;
}

// decrease all ram entries at once
void wram_sink(wentry_t *m) {
    wentry_t *p;
    
    p = m;
    for (;;) {
        if (p->next==m) break;
        p = p->next;
        if (p->value > 1) p->value -= 1;
        else {
            (p->prev)->next = p->next;
            (p->next)->prev = p->prev;
            wcounter--;
            m->value--;
            free(p);
        }
    }
}

// discriminator print utility functions
void wram_print(wentry_t *m) {
    wentry_t *p;
    
    fprintf(stdout,"{");
    for(p=m;;p=p->next) {
        if (p->next==m) break;
        if (p->next->next==m) fprintf(stdout,"%lu:%f", p->next->key, p->next->value);
        else fprintf(stdout,"%lu:%f,", p->next->key, p->next->value);
    }
    fprintf(stdout,"}");
}

// free ram
void wram_free(wentry_t *m) {
    free(m);
    m=(wentry_t *)NULL;
}

void intuple_print(wkey_t *vector, long size) {
    long i;
    printf("[");
    for (i = 0; i < size; i++) {
        if (vector[i]) printf("%li", vector[i]);
        if (i != size -1) printf(".");
    }
    printf("]\n");
}


