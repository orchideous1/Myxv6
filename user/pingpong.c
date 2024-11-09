# include "kernel/types.h"
# include "user.h"

int main(int argc, char* argv[]){
    if (argc >= 2) {
        printf("pingpong don't need arguments!\n");
        exit(-1);
    }
    int p1[2];
    int p2[2];
    
    if (pipe(p1) < 0 || pipe(p2) < 0) {
        printf("pipe Error");
        exit(-1);
    }
    int child_pid;
    
    if ((child_pid = fork()) == 0){
        char buffer[1024];
        int bytesRead;
        close(p1[1]);//关闭p1写端
        bytesRead = read(p1[0], buffer, sizeof(buffer) - 1);
        buffer[bytesRead] = '\0';

        // 获取当前进程的ID
        int pid;
        pid = getpid();
        printf("%d: received %s\n", pid, buffer);
        
        close(p2[0]);
        write(p2[1], "pong", 4);
        close(p2[1]);
        exit(0);
    }else {
        char buffer[1024];
        int bytesRead;
        close(p1[0]);//关闭p1读端
        write(p1[1], "ping", 4);
        close(p1[1]);


        close(p2[1]);//关闭p2写端
        bytesRead = read(p2[0], buffer, sizeof(buffer) - 1);
        buffer[bytesRead] = '\0';
        int ppid = getpid();
        printf("%d: received %s\n", ppid, buffer);
        
        close(p2[0]); 

    }

}