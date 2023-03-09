#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

void* thread_main(void *arg);

int main(){
    pthread_t t_id;
    int thread_param=5;

    if(pthread_create(&t_id,NULL,thread_main,(void*)&thread_param)!=0){
        puts("pthread_create error");
        return -1;
    }
    int* thr_ret;
    pthread_join(t_id,(void **)(&thr_ret));
    printf("thread return : %d",*thr_ret);
    free(thr_ret);
    puts("end of main");
    return 0;
}

void* thread_main(void *arg){
    int i=0;
    int cnt=*((int*)arg);
    for(;i<cnt;++i){
        sleep(1);
        puts("runing thread");
    }
    int c=9;
    return (void*)&c;
}