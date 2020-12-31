/*
 * @Author: your name
 * @Date: 2020-12-12 11:40:01
 * @LastEditTime: 2020-12-13 01:06:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /C++/chatRoom/Server.cpp
 */
#include "Server.h"

mutex m;
// 最大描述符
int maxfd;
Server *Server::server = new Server;
Server &Server::GetServer() {
    // 返回一个引用，该引用就是server指针指向的server实例
    return *server;
}

// 服务器初始化
void Server::Init(int port) {
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
    cout<<"Init Finished!"<<endl;
    // 接下来可以添加，线程池的初始化，等等
    return;
}

void Server::Start() {
    // 当前连接数
    server->ccfn = 0;
    // 缓冲区
    char buff[MAXLINE];
    // 服务器启动，开始监听socket，accept连接后返回一个connfd，并传入Server()函数中
    // 打开socket返回socket描述字，唯一标识一个socket，把它作为参数能进行一些读写操作
    // socket函数:参数1domain:协议域，参数2type:指定socket类型，参数3protocol:指定协议，0为根据type自动选择
    if ((server->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        // 获取socket描述字,失败则提示错误信息
        perror("socket()");
        return;
    }

    // bind函数,把一个地址族中的特定地址赋给socket，ip地址和端口号等等（根据socket的协议域）
    if (bind(server->sockfd, (struct sockaddr*)&(server->servaddr), sizeof(server->servaddr)) == -1) {
        perror("bind()");
        return;
    }

    // listen函数，开始监听，等待连接请求，第二个参数为相应socket可以排队的最大连接数
    if (listen(server->sockfd, 10) == -1) {
        perror("listen()");
        return;
    }

    // 初始化最大描述符为监听描述符，后面会更新maxfd，用于select的第一个参数
    maxfd = server->sockfd;
    // 等待连接
    cout<<"Waiting for connection......"<<endl;

    // 创建注册线程,最大线程数为连接数
    pthread_t threads[MAXCONN];

    while(1) {
        // 采用IO多路复用
        // fdset置0
        FD_ZERO(&server->fdset);

        // 服务端的监听描述符加入到fdset中
        FD_SET(server->sockfd, &server->fdset);

        for (int i = 0; i < MAXCONN; i++) {
            cout<<"clientfd:"<<server->clients[i].clientfd<<endl;
        }
        // 将客户端的连接描述符加入到fdset中
        for (int i = 0; i < MAXCONN; i++) {
            if (server->clients[i].clientfd != 0) {
                FD_SET(server->clients[i].clientfd, &server->fdset);
            }
        }
        cout<<"select maxfd"<<maxfd<<endl;
        // select返回准备好的fd数,关心传入的fdset读(readset)
        // 参数依次为：最大文件描述符+1，读，写，异常集，等待时间NULL代表一直等待
        if (select(maxfd+1, &server->fdset, NULL, NULL, NULL) < 0) {
            // select操作将fdset位图置位代表该位掩码下的文件描述符可读
            perror("select()");
            // 此处不应该break吧！！而是将出错的连接sock踢出fdset
            // 有bug... ctrl+c退出的话。。。
            // break;
            // 出错，将对应的客户踢出队列
        }

        for (int i = 0; i < server->ccfn; i++) {
            // 检查每个客户端描述符是否可读
            if (FD_ISSET(server->clients[i].clientfd, &server->fdset)) {
                // 可读则将该描述符传入Serve方法进行处理
                server->Serve(server->clients[i]);
            }
        }

        if (FD_ISSET(server->sockfd, &server->fdset)) {
            // 监听到连接，创建线程进行处理
            // 先accpet获取到clientfd
            int clientsock;
            if ((clientsock = accept(server->sockfd, NULL, NULL)) < 0) {
                perror("accpet()");
                return;
            }
            if (pthread_create(&threads[server->ccfn], NULL, Server::Register, (void*)&(clientsock)) != 0) {
                perror("pthread_create()");
            }
        }
        cout<<"maxfd"<<maxfd<<endl;
    }
    server->Close();
    return;
}


void* Server::Register(void* sock) {
    const int clientsock = *((int*)sock);
    // 由于单独抽出注册线程，那么在这里可以阻塞读写
    int len;
    char buff[MAXLINE];
    string res;
    if (server->ccfn < MAXCONN) {
        // 不超出最大连接数即添加进用户列表
        cout<<"Registering..."<<endl;
        server->clients[server->ccfn++].clientfd = clientsock;
        strcpy(buff, "Welcome to ChatRoom!\nInput your name:");
        send(clientsock, buff, strlen(buff), 0);
        // 接收注册信息：用户名
        len = recv(clientsock, buff, MAXLINE, 0);
        if (len < 0) {
            perror("recv()");
            // 异常处理
        }
        // 昵称
        buff[len-1] = '\0';
        cout<<buff<<" registered successfully!"<<endl;
        server->clients[server->ccfn - 1].name = buff;
        res = " join the ChatRoom!";

        server->SendAll(buff + res);
        res = "Just input to enjoy your chat!";
        send(clientsock, res.c_str(), strlen(res.c_str()), 0);
        
        // 更新文件描述符最大值
        // 加锁
        m.lock();
        maxfd = max(maxfd, clientsock);
        m.unlock();
        // 解锁
    } else {
        strcpy(buff, "ChatRoom is over the max connections!\n");
        send(clientsock, buff, sizeof(buff), 0);
        // 发送友好信息后断开连接
        close(clientsock);
    }
    return 0;
}

void Server::Close() {
    // 服务器关闭，优雅地关闭资源
    // close监听socket
    close(server->sockfd);
    return;
}

// void Server::Register(int sockfd, int &maxfd) {
//     int clientsock;
//     int len;
//     char buff[MAXLINE];
//     if ((clientsock = accept(server->sockfd, NULL, NULL)) < 0) {
//         perror("accpet()");
//         return;
//     }
//     if (server->ccfn < MAXCONN) {
//         // 不超出最大连接数即添加进用户列表
//         cout<<"Registering..."<<endl;
//         server->clients[server->ccfn++].clientfd = clientsock;
//         strcpy(buff, "Welcome to ChatRoom!\nInput your name:");
//         send(clientsock, buff, strlen(buff), 0);
//         // 接收注册信息：用户名
//         len = recv(clientsock, buff, MAXLINE, 0);
//         if (len < 0) {
//             perror("recv()");
//             // 异常处理
//             return;
//         }
//         // 昵称
//         buff[len-1] = '\0';
//         cout<<buff<<" registered successfully!"<<endl;
//         server->clients[server->ccfn - 1].name = buff;
//         strcat(buff, " join the ChatRoom!\n");
//         server->SendAll(string(buff));
        
//         // 更新文件描述符最大值
//         maxfd = max(maxfd, clientsock);
//     } else {
//         strcpy(buff, "ChatRoom is over the max connections!\n");
//         send(clientsock, buff, sizeof(buff), 0);
//         // 发送友好信息后断开连接
//         close(clientsock);
//     }
// }


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
    // 这里有一个阻塞读emmm,没问题的吧，因为是可读才到这里
    len = recv(client.clientfd, buff, sizeof(buff), 0);
    if (len <= 0) {
        // 该描述符出错
        // 关闭连接并删除
        close(client.clientfd);
        FD_CLR(client.clientfd, &server->fdset);
        // 先不处理
    } else {
        buff[len] = '\0';
        res = "---------------------------\n" + string(dt) + client.name + ":" + buff;
        cout<<res<<endl;
        server->SendAll(res);
    }
}

void Server::SendAll(string res) {
    // 广播信息
    for (int i = 0; i < server->ccfn; i++) {
        send(server->clients[i].clientfd, res.c_str(), strlen(res.c_str()), 0);
    }
}