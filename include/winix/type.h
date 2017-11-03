/**
 * 
 * Winix kernel types
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:44
 * 
*/
#ifndef _W_TYPE_H_
#define _W_TYPE_H_ 1

typedef unsigned int reg_t;
typedef unsigned int ptr_t;  // physical address
typedef unsigned int vptr_t; // virtual address

struct list_head {
    struct list_head *next, *prev;
};

struct hlist_head {
	struct hlist_node *first;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

#endif
