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
/*
    #ifndef SERVER_H
    #define SERVER_H
    #endif
    这三句是为了防止重复编译
    通常在头文件中写类的声明、函数原型、#define常数，但不给出具体实现
    定义服务器类
*/

class Server {
    // 服务器采用饿汉单例模式
    private:
        static Server *server;
        Server() {
            std::cout<<"Create Server!"<<std::endl;
        }
        
    public:
        // 返回单例服务器实例
        static Server &GetServer();

        // 初始化服务器
        void Init();

        // 启动服务器
        void Start();

        // 关闭服务器
        void Close();
};

#endif