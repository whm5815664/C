/*
进程：哲学家进餐问题
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>

union semun {                                    // 信号量
    int val;                                     //信号量初值
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};



void wait_l(int semid, int num)                //定义p操作 -1
{
    struct sembuf sb = {num, -1, 0};                  // struct sembuf sb = {信号量下标, 信号量操作一次改变的数值, 0};
    semop(num, &sb, 1);                               // semop(信号量ID, struct sembuf *sem_ops,size_t num_sem_ops)
}

void signal_l(int semid, int num)             //定义v操作 +1
{
    struct sembuf sb = {num, +1, 0};
    semop(num, &sb, 1);
}
#define DELAY (rand() % 5 + 1)

void ph(int semid, int num)
{
    for (;;)
    {
        if (num % 2 != 0)
        {
            printf("%d is thinking\n", num);
            sleep(DELAY);
            printf("%d is hungry\n", num);
            sleep(DELAY);
            wait_l(semid, num);
            wait_l(semid, (num + 1) % 5);
            printf("%d is eating\n", num);
            sleep(DELAY);
            signal_l(semid, num);
            signal_l(semid, (num + 1) % 5);
        }
        if (num % 2 == 0)
        {
            printf("%d is thinking\n", num);
            sleep(DELAY);
            printf("%d is hungry\n", num);
            sleep(DELAY);
            wait_l(semid, (num + 1) % 5);
            wait_l(semid, num);
            signal_l(semid, num);
            signal_l(semid, (num + 1) % 5);
        }
    }
}

int main(int argc, char *argv[])
{
    int semid;
    semid = semget(IPC_PRIVATE, 5, IPC_CREAT | 0666);      //创建信号量id semget(信号量对象, 信号量数目, 信号量权限1 | 信号量权限2)

    union semun su;   //设置信号量修改操作
    su.val = 1;

    int i;
    for (i = 0; i < 5; i++)             //初始化5个线程，即semaphore fork[5]={1,1,1,1,1};
    {
        semctl(semid, i, SETVAL, su);   //初始化，对信号量的值进行修改 semctl(信号量id，信号量下标编号，具体操作命令semun联合体)
    }


    pid_t pid;                        //建立线程
    int num = 5;
    for (i = 0; i <= 4; i++) //
    {
        pid = fork();
        if (pid == 0)
        {
            num = i;
            break;
        }
    }
    ph(semid, num);
    return 0;
}