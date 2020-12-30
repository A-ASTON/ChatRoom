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
#include <iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<string>
#include <time.h>
#define MAXLINE 4096
// 最大连接数
#define MAXCONN 100
using namespace std;
/*
    #ifndef SERVER_H
    #define SERVER_H
    #endif
    这三句是为了防止重复编译
    通常在头文件中写类的声明、函数原型、#define常数，但不给出具体实现
    定义服务器类
*/
class ClientSubject {
    // 代表接入Server的用户实体，存放了Client的相关信息
    public:
        // 属于Client的连接描述符
        int clientfd;
        string name;
        ClientSubject():clientfd(0), name(""){
            //空参构造
        }
        ClientSubject(int clientfd, string name):clientfd(clientfd),name(name){}
        void init();
        
};

class Server {
    // 服务器采用饿汉单例模式
    private:
        static Server *server;
        Server() {
            std::cout<<"Create Server!"<<std::endl;
        }
        // 服务器socket信息配置
        struct sockaddr_in servaddr;
        // 全局维护一个监听sockfd即可
        int sockfd;
        // 全局维护一个fd_set，IO多路复用中使用到的fd集合
        fd_set fdset;
        // 全局维护一个客户端实体集
        ClientSubject clients[MAXCONN];
        
    public:
        // 返回单例服务器实例
        static Server &GetServer();

        // 初始化服务器
        int Init(int);

        // 启动服务器
        int Start();

        // 关闭服务器
        int Close();

        // 用户注册
        void Register(int);

        // 业务逻辑
        void Serve(ClientSubject);

};

#endif