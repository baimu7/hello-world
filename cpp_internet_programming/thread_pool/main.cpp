#include "threadpool.h"
#include<iostream>
void testFun(void *arg){
    std::cout<<"i="<<*(int*)arg<<std::endl;
}

int main(){
    ThreadPool *pool=new ThreadPool(1000,2000);

    std::cout<<"线程初始化成功"<<std::endl;
    for(int i=0;i<1000;++i){
        pool->pushJob(testFun,&i,sizeof(int));
    }
    return 0;
}