
#include <stdio.h>
#include <unistd.h>
#include <winix/rex.h>

int main(int argc, char** argv){
    if(argc <= 2){
        while(!(RexSp1->Stat & 2));
        RexSp1->Tx = '!';
    }
    return 0;
}

int foo(){

    return 100;
}
