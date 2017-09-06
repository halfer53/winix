/**
 * 
 * Globals used in WINIX.
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2016-09-19
 * 
*/

#ifndef _WINIX_H_
#define _WINIX_H_ 1

#include <kernel/kernel.h>
#include <ucontext.h>

#include <kernel/clock.h>
#include <kernel/exception.h>
#include <kernel/system.h>
#include <kernel/idle.h>


struct proc_config{
    char name[PROC_NAME_LEN];
    void (*entry)();
    int pid;
    int quantum;
    bool iskernel_proc;
    unsigned int *image_array;
    int image_len;
};

//Major and minor version numbers for WINIX.
#define MAJOR_VERSION 2
#define MINOR_VERSION 0

#endif

