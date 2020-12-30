/*
 * @Author: your name
 * @Date: 2020-12-12 11:40:01
 * @LastEditTime: 2020-12-13 01:06:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /C++/chatRoom/Server.cpp
 */
#include "Server.h"
#include <iostream>

Server *Server::server = new Server;
Server &Server::GetServer() {
    // 返回一个引用，该引用就是server指针指向的server实例
    return *server;
}

// 服务器初始化
int Server::Init(int port) {
    // 具体作用就是，初始化socket配置，设置本地监听的端口，不需要设置ip，因为就是服务器的ip由客户端连接
    cout<<"Init Server..."<<endl;
    // 内存分配
    memset(&server->servaddr, 0, sizeof(servaddr));
    // ipv4
    server->servaddr.sin_family = AF_INET;
    // htonl函数，字节序转换host to network long,大端小端问题
    server->servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 服务器监听port端口
    server->servaddr.sin_port = htons(port);

    // 接下来可以添加，线程池的初始化，等等
    return 1;
}

int Server::Start() {
    // 最大描述符
    int maxfd;
    // 当前连接数
    int ccfn = 0;
    // 缓冲区
    char buff[MAXLINE];
    // 服务器启动，开始监听socket，accept连接后返回一个connfd，并传入Server()函数中
    // 打开socket返回socket描述字，唯一标识一个socket，把它作为参数能进行一些读写操作
    // socket函数:参数1domain:协议域，参数2type:指定socket类型，参数3protocol:指定协议，0为根据type自动选择
    if((server->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        // 获取socket描述字,失败则提示错误信息
        perror("socket()");
        return -1;
    }

    // bind函数,把一个地址族中的特定地址赋给socket，ip地址和端口号等等（根据socket的协议域）
    if(bind(server->sockfd, (struct sockaddr*)&(server->servaddr), sizeof(server->servaddr)) == -1) {
        perror("bind()");
        return -1;
    }

    // listen函数，开始监听，等待连接请求，第二个参数为相应socket可以排队的最大连接数
    if(listen(server->sockfd, 10) == -1) {
        perror("listen()");
        return -1;
    }

    // 初始化最大描述符为监听描述符，后面会更新maxfd，用于select的第一个参数
    maxfd = server->sockfd;
    // 等待连接
    cout<<"Waiting for connection......"<<endl;
    while(1) {
        // 采用IO多路复用
        // fdset置0
        FD_ZERO(&server->fdset);

        // 监听fd置入fdset中
        FD_SET(server->sockfd, &server->fdset);

        // 将客户端的连接描述符加入到fdset中
        for(int i = 0; i < MAXCONN; i++) {
            if(server->clients[i].clientfd != 0) {
                FD_SET(server->clients[i].clientfd, &server->fdset);
            }
        }
        
        // select返回准备好的fd数量,关心传入的fdset读(readset)
        // 参数依次为：最大文件描述符+1，读，写，异常集，等待时间NULL代表一直等待
        if(select(maxfd+1, &server->fdset, NULL, NULL, NULL) < 0) {
            // select操作将fdset位图置位代表该位掩码下的文件描述符可读
            perror("select()");
            // 此处不应该break吧！！而是将出错的连接sock踢出fdset
            break;
        }

        for(int i = 0; i < ccfn; i++) {
            // 检查每个客户端描述符是否可读
            if(FD_ISSET(server->clients[i].clientfd, &server->fdset)) {
                // 可读则将该描述符传入Serve方法进行处理
                server->Serve(server->clients[i]);
            }
        }

        // 检查是否有新连接
        if(FD_ISSET(server->sockfd, &server->fdset)) {
            // 将来用Register函数替换
            int clientsock;
            int len;
            if ((clientsock = accept(server->sockfd, NULL, NULL)) < 0) {
                perror("accpet()");
                continue;
            }
            if(ccfn < MAXCONN) {
                // 不超出最大连接数即添加进用户列表
                server->clients[ccfn++].clientfd = clientsock;
                strcpy(buff, "Welcome to ChatRoom!\n");
                send(clientsock, buff, strlen(buff), 0);
                // 接收注册信息：用户名
                len = recv(clientsock, buff, MAXLINE, 0);
                if(len < 0) {
                    perror("recv()");
                    // 异常处理
                    continue;
                }
                // 昵称
                buff[len] = '\0';
                server->clients[ccfn - 1].name = buff;
                strcat(buff, "join the ChatRoom!\n");
                send(clientsock, buff, sizeof(buff), 0);
                
                // 更新文件描述符最大值
                maxfd = max(maxfd, clientsock);
            } else {
                strcpy(buff, "ChatRoom is over the max connections!\n");
                send(clientsock, buff, sizeof(buff), 0);
                // 发送友好信息后断开连接
                close(clientsock);
            }
        }

    }
    server->Close();
    return 0;
}

int Server::Close() {
    // 服务器关闭，优雅地关闭资源
    // close监听socket
    close(server->sockfd);
    return 0;
}

void Server::Register(int connfd) {
    char buff[4096];
    char name[4096];
    string res = ""; // 利用string.c_str()字符串转换为char*

    res = "Welcome to ChatRoom!\nInput your name:";
    send(connfd, res.c_str(), strlen(res.c_str()),0);

    int n = recv(connfd, name, MAXLINE, 0);
    name[n] = '\0';

    res = "Welcome You! ";
    res.append(name);
    send(connfd, res.c_str(), strlen(res.c_str()), 0);
}

void Server::Serve(ClientSubject client) {
    // 服务器业务逻辑，如何处理与用户的聊天
    // 根据client回显信息
    // 读用户发送的信息，然后构造输出格式
    // 当前时间
    char buff[MAXLINE];
    string res;
    time_t now = time(0);
    char* dt = ctime(&now);
    int len;
    // 这里有一个阻塞读emmm
    len = recv(client.clientfd, buff, sizeof(buff), 0);
    if(len <= 0) {
        // 该描述符出错
        // 关闭连接并删除
        close(client.clientfd);
        FD_CLR(client.clientfd, &server->fdset);
        // 先不处理
    } else {
        buff[len] = '\0';
        res = string(dt)+ "\n" + client.name + ":" + buff;
        send(client.clientfd, res.c_str(), sizeof(res.c_str()), 0);
    }
}