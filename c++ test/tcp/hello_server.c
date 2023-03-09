#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ 4
void error_handling(char* message);
int calculate(int opnum,int opnds[],char operator);

int main(int argc,char *argv[]){
    int serv_sock;
    int clnt_sock;
    int result,opnd_cnt;
    int recv_len,recv_cnt;
    char opinfo[BUF_SIZE];

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size;


    if(argc!=2){
        printf("Usage : %s <proot>\n",argv[0]);
        exit(1);
    }
    
    serv_sock=socket(PF_INET,SOCK_STREAM,0);//创建服务器端套接字,PF_INET为IPv4协议簇,SOCK_STEAM代表套接字类型,即TCP协议套接字类型,0代表选择默认协议
    if(serv_sock==-1){
        error_handling("socket() error");
    }

    memset(&serv_addr,0,sizeof(serv_addr));//memset万能初始化内存函数,将结构变量addr的所有成员初始化为0
    serv_addr.sin_family=AF_INET;//IPv4协议族
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//将IP地址转换网络字序,host to ip long类型,INADDR_ANY自动获取运行服务器端计算机的IP
    serv_addr.sin_port=htons(atoi(argv[1]));//atoi()将字符串转换为整数型,host to ip short类型

    //serv_addr为sockaddr_in类型,需转换为sockaddr类型,使用sockaddr_in类型是方便输入ip地址,端口;
    //bind函数调用成功,会将第二参数指定的地址信息分配给第一个参数中的相应套接字;
    if (bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
        error_handling("bind() error");
    }
    
    //5表示请求等待队列的长度
    if(listen(serv_sock,5)==-1){
        error_handling("listen() error");
    }
    
    clnt_addr_size=sizeof(clnt_addr);

    for(int i=0;i<5;++i){
        opnd_cnt=0;
        clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_addr,&clnt_addr_size);
        read(clnt_sock,&opnd_cnt,1);

        recv_len=0;
        while((opnd_cnt*OPSZ+1)>recv_len){
            recv_cnt=read(clnt_sock,&opinfo[recv_len],BUF_SIZE-1);
            recv_len+=recv_cnt;
        }
        result=calculate(opnd_cnt,(int*)opinfo,opinfo[recv_len-1]);
        write(clnt_sock,(char*)&result,sizeof(result));
        close(clnt_sock);
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}

int calculate(int opnum,int opnds[],char op){
    int result=opnds[0];
    switch(op){
        case '+':
        for(int i=1;i<opnum;++i) result+=opnds[i];
        break;
        case '-':
        for(int i=1;i<opnum;++i) result-=opnds[i];
        break;
        case '*':
        for(int i=1;i<opnum;++i) result*=opnds[i];
        break;
    }
    return result;
}