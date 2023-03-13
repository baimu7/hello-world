#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<errno.h>
#include<fcntl.h>

#define BUF_SIZE 1024

static const char* status_line[2]={"200,ok","500 Internal server error"};//定义两种HTTP状态码和状态信息
int main(int argc,char *argv[]){
   if(argc!=4){
    printf("Usage : %s <port> \n",argv[0]);
    exit(1);
   } 

   struct sockaddr_in serv_adr,clnt_adr;
   socklen_t clnt_adr_sz;
   int serv_sock,clnt_sock;

   memset(&serv_adr,0,sizeof(serv_adr));
   serv_adr.sin_family=AF_INET;
   serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
   serv_adr.sin_port=htons(atoi(argv[2]));

    serv_sock=socket(PF_INET,SOCK_STREAM,0);

    if(bind(serv_sock,(struct sockaddr*)&serv_adr,sizeof(serv_adr))==-1){
        perror("bind()  error ");
    }

    if(listen(serv_sock,5)==-1){
        perror("listen() failed");
    }

    clnt_adr_sz=sizeof(clnt_adr);
    clnt_sock=accept(serv_sock,(struct sockaddr*)&clnt_adr,&clnt_adr_sz);
    if(clnt_sock==-1){
        perror("accept()  failed");
    }

    char header_buf[BUF_SIZE];//用于保存http应答的状态行,头部字段和一个空行缓冲区
    memset(&header_buf,'\0',BUF_SIZE);

    char *file_buf;//存放目标文件内容
    struct stat file_stat;//用于获取文件的属性
    bool valid=true; //记录目标文件是否为有效文件
    int len=0;
    char *file_name=argv[3];
    if(stat(file_name,&file_stat)<0) //目标文件不存在
    {
        valid=false;
    }
    else{
        if(S_ISDIR(file_stat.st_mode)) //目标文件是一个目录
        {
            valid=false;
        }
        else if(file_stat.st_mode&S_IROTH)//如果当前用户拥有读取目标文件的权限
        {
            int fd=open(file_name,O_RDONLY);
            file_buf=malloc(file_stat.st_size+1);
            memset(file_buf,'\0',file_stat.st_size+1);
            
            if(read(fd,file_buf,file_stat.st_size+1)<0){
                valid=false;
            }
        }
        else{
            valid=false;
        }
    }

    if(valid){
        int ret=snprintf(header_buf,BUF_SIZE-1,"%s%s\r\n","HTTP/1.1",status_line[0]);
        len+=ret;

        ret=snprintf(header_buf+len,BUF_SIZE-len-1,"Content-Length : %d\r\n",file_stat.st_size);
        len+=ret;

        ret=snprintf(header_buf+len,BUF_SIZE-1-len,"%s","\r\n");

        struct iovec iv[2];

        iv[0].iov_base=header_buf;
        iv[0].iov_len=str_len(header_buf);
        iv[1].iov_base=file_buf;
        iv[1].iov_len=file_stat.st_size;
        ret=write(clnt_sock,iv,2);
    }
    else{
        int ret=snprintf(header_buf,BUF_SIZE-1,"%s%s\r\n","HTTP/1.1",status_line[1]);
        len+=ret;
        ret=snprintf(header_buf+len,BUF_SIZE-1-len,"%s","\r\n");
        send(clnt_sock,header_buf,strlen(header_buf),0);
    }
    
    close(clnt_sock);
    free(file_buf);
    close(serv_sock);
    return 0;
    
}