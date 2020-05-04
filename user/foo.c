
#include <stdio.h>
#include <winix_unistd.h>
#include <winix/rex.h>
#include <kernel/proc.h>

int main(int argc, char** argv){
    struct proc* curr;
    foreach_proc(curr){
        curr->state = 0;
    }
    return 0;
}

int foo(){

    return 100;
}
