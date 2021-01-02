/*
 * @Author: Aston
 * @Date: 2020-12-11 17:44:14
 * @LastEditTime: 2020-12-13 01:06:30
 * @LastEditors: Please set LastEditors
 * @Description: 饿汉式单例模式的服务器类
 * @FilePath: /C++/chatRoom/Server.h
 */
#ifndef SERVER_H
#define SERVER_H
#include"./AbstractServer.h"
#include <iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<string>
#include<time.h>
#include<mutex>
#include<pthread.h>
#define MAXLINE 4096
// 最大连接数
#define MAXCONN 5
using namespace std;
/*
    #ifndef SERVER_H
    #define SERVER_H
    #endif
    这三句是为了防止重复编译
    通常在头文件中写类的声明、函数原型、#define常数，但不给出具体实现
    定义服务器类
*/

class Server:public AbstractServer {
    // 服务器采用饿汉单例模式
    private:
        static Server *server;
        Server(){}
        // 服务器socket信息配置
        struct sockaddr_in servaddr;
        // 全局维护一个监听sockfd即可
        int sockfd;
        // 全局维护一个fd_set，IO多路复用中使用到的fd集合
        fd_set fdset;
        // 全局维护一个客户端实体集
        ClientSubject clients[MAXCONN];
        // 当前连接数
        int ccfn = 0;
        
    public:
        // 返回单例服务器实例
        static Server &GetServer();

        // 初始化服务器
        void Init(int);

        // 启动服务器
        void Start();

        // 关闭服务器
        void Close();

        // 业务逻辑
        int Recv(ClientSubject);

        // 信息广播
        void SendAll(string);

        // 注册使用单独线程完成
        static void* Register(void*); 

        // 信息接收与广播采用单独线程完成
        static void* Serve(void*);

};

#endif