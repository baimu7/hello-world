#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc,char* argv[]){
    int sock;
    int snd_buf,rcv_buf;
    socklen_t len;

    sock=socket(PF_INET,SOCK_STREAM,0);
    len=sizeof(rcv_buf);
    if(getsockopt(sock,SOL_SOCKET,SO_RCVBUF,(void *)&rcv_buf,&len)==-1) {
        error_handling("getsockopt()....error");
    }   

    len=sizeof(snd_buf);
    if(getsockopt(sock,SOL_SOCKET,SO_SNDBUF,(void *)&snd_buf,&len)==-1){
        error_handling("getsockopt()...error");
    }

    printf("Input buffer : %d\n",rcv_buf);
    printf("Output buffer : %d \n",snd_buf);

    return 0;
}
void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1); 
}