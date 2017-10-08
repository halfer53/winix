/**
 * 
 * Winix utiles
 *
 * @author Bruce Tan
 * @email brucetansh@gmail.com
 * 
 * @author Paul Monigatti
 * @email paulmoni@waikato.ac.nz
 * 
 * @create date 2017-08-23 06:22:47
 * 
*/
#ifndef _W_UTIL_H_
#define _W_UTIL_H_

#include <curses.h>
#include <stdbool.h>

#define ASSERT(expression)    _assert(expression, __LINE__ , __FILE__)
#define ASSERT_NOT_NULL(exp)    ASSERT(exp != NULL)
#define ASSERT_NULL(exp)        ASSERT(exp == NULL)
#define ASSERT_TRUE(exp)        ASSERT(exp == true)
#define ASSERT_FALSE(exp)       ASSERT(exp == false)
#define ASSERT_OK(exp)          ASSERT(exp == OK)
#define PANIC(expression)     _panic(expression, __FILE__)


#endif
