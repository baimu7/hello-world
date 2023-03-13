#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<poll.h>
#include<errno.h>
#include<fcntl.h>

int main(int argc,char *argv[]){
    if(argc!=3){
        printf("Usage : %s <port>",argv[0]);
        exit(1);
    }

    struct sockaddr_in serv_adr,clnt_adr;
    int serv_sock,clnt_sock;
    socklen_t clnt_adr_sz;

    memset(&serv_adr,0,sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_adr.sin_port=htons(atoi(argv[2]));

    serv_sock=socket(PF_INET,SOCK_STREAM,0);

    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1){
        perror("bind() failed");
    }    

    if(listen(serv_sock,5)==-1){
        perror("listen() failed");
    }

    clnt_adr_sz=sizeof(clnt_adr);
    clnt_adr_sz=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
    if(clnt_sock==-1){
        perror("accept() failed");
    }

    close(STDOUT_FILENO);
    dup(clnt_sock);
    
    printf("abc \n");
    close(clnt_sock);
    close(serv_sock);

    return 0;

}