#define _POSIX_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<signal.h>

#define BUF_SIZE 100

void error_handling(char* message);
void read_childproc(int sig);

int main(int argc,char *argv[]){
    
    int serv_sock,clnt_sock;
    struct sockaddr_in serv_adr,clnt_adr;
    socklen_t adr_sz;
    char buf[BUF_SIZE];
    int fds[2];

    pid_t pid;
    struct sigaction act;
    int str_len,state;

    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(0);
    }

    act.sa_handler=read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags=0;
    state=sigaction(SIGCHLD,&act,0);

    serv_sock=socket(PF_INET,SOCK_STREAM,0);
    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    serv_adr.sin_port=htons(atoi(argv[1]));

    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1){
        error_handling("bind()  error");
    }

    if(listen(serv_sock,5)==-1){
        error_handling("listen()  error");
    }

    pipe(fds);
    pid=fork();

    if(pid==0){
        FILE* fp=fopen("echomsg.txt","wt");
        char msgbuf[BUF_SIZE];
        int len;

        for(int i=0;i<10;++i){
            len=read(fds[0],msgbuf,BUF_SIZE);
            fwrite((void *)msgbuf,1,len,fp);
        }
        fclose(fp);
        return 0;
    }

    while(1){
        adr_sz=sizeof(clnt_adr);
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&adr_sz);
        if(clnt_sock==-1) continue;
        else puts("new connected.....");

        pid=fork();
        if(pid==0){
            close(serv_sock);//fork后,父子进程各有一个指向套接字的文件描述符,因此在子程序退出前需关闭子程序指向套接字的文件描述符
            while((str_len=read(clnt_sock,buf,BUF_SIZE))!=0){
                write(clnt_sock,buf,str_len);
                write(fds[1],buf,str_len);
            }

            close(clnt_sock);
            puts("client disconnected....");
            return 0;
        }
        else close(clnt_sock);
    }

    close(serv_sock);
    return 0;
       
}

void read_childproc(int sig){
    pid_t pid;
    int status;
    pid=waitpid(-1,&status,WNOHANG);
    printf("removed proc id : %d \n",pid);
}


void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}