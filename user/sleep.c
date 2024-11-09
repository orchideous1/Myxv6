#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    if(argc == 1){
        printf("error:sleep take at least one argument!");
    }else{
        int time = atoi(argv[1]);
        sleep(time);
    }  
    exit(0);
}
