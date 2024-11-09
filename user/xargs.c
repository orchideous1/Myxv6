# include "kernel/types.h"
# include "kernel/param.h"
# include "user.h"


char*
mygets(char *buf, int * pstart, int max, char * ans)
{
    int i, j = 0;
    char c;
    if((*pstart) >= max){
        return 0;
    }
    for(i=(*pstart); i < max; ++i, ++j){
        c = buf[i];
        if(c == ' ' || c == '\n' || c == '\r'){
            for(int o = 0; o < j; ++o){
                ans[o] = buf[(*pstart) + o];
            }
            ans[j] = '\0';
            (*pstart) = i + 1;
            break;
        }
    }
    
    return ans;
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}

void
panic(char *s)
{
  fprintf(2, "%s\n", s);
  exit(1);
}
int main(int argc, char* argv[]){
    char buffer[100];
    int byte = 0;
    while((byte = strlen(gets(buffer, 100))) > 0){  
        char * argu[MAXARG];
        memset(argu, 0, sizeof(argu));
        for(int i = 1; i < argc; ++i){
            argu[i - 1] = argv[i];
        }
        int argn = argc - 1;
        int start = 0;
        int * pstart = &start;
        for(int i = argn; i < MAXARG; ++i){
            argu[i] = malloc(sizeof(buffer));
            mygets(buffer, pstart, byte, argu[i]);
            argn++;
            if((*pstart) >= byte){
                break;
            }
        }
        int child;
        if((child = fork1()) == 0){
            exec(argu[0], argu);
            exit(0);
        }else if(child == -1){
            printf("fork error!");
        }
        int status;
        int childpid;
        childpid = wait(&status);
       // printf("child pid from father: %d\n", childpid);
        
    }
    exit(0);

}

