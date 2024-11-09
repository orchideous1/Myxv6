# include "kernel/types.h"
# include "user.h"
// p = get a number from left neighbor
// print p
// loop:
//     n = get a number from left neighbor
//     if (p does not divide n)
//         send n to right neighbor
int main(int argc, char* argv[]){
    if (argc >= 2) {
        printf("primes don't need arguments!\n");
        exit(-1);
    }

    int p1[2];
    if (pipe(p1) < 0) {
        printf("pipe error");
        exit(-1);
    }
    int child = fork();
    if(child == -1){
        printf("fork error");
        exit(-1);
    }


    if(child == 0){
        prime_sieve(p1);
        exit(0);
    }else{
        close(p1[0]);//关闭p1读端
        for (int i = 2; i <= 35; ++i){       
            write(p1[1], &i, sizeof(i));    
        }
        close(p1[1]);
        wait(0);
        exit(0);
    }   
}

void prime_sieve(int p1[]){
    close(p1[1]);
    int in_num;
    if(read(p1[0], &in_num, sizeof(in_num)) == 0){
        return;
    }
    printf("prime %d\n", in_num);

    int p2[2];
    if (pipe(p2) < 0) {
        printf("pipe error");
        exit(-1);
    }
    int child = fork();
    if(child == -1){
        printf("fork error");
        exit(-1);
    }
    if(child == 0){
        prime_sieve(p2);
        exit(0);
    }else{
        close(p2[0]);
        int in_num2;
        while(read(p1[0], &in_num2, sizeof(in_num2))){
            
            if(in_num2 % in_num){
                int flag = write(p2[1], &in_num2, sizeof(in_num2));   
            }
        }
        close(p1[0]);
        close(p2[1]);
        //printf("close 1\n");
        wait(0);
    }
}


