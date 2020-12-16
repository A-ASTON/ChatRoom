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

void Server::Init() {
    std::cout<<"Init~"<<std::endl;
    // 服务器初始化
}

void Server::Start() {
    // 服务器启动
}

void Server::Close() {
    // 服务器关闭
}