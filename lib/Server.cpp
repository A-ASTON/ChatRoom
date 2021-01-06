/*
 * @Author: your name
 * @Date: 2020-12-12 11:40:01
 * @LastEditTime: 2020-12-13 01:06:44
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /C++/chatRoom/Server.cpp
 */
#include "Server.h"

// 锁定义为全局变量
mutex m;
// 最大描述符定义为全局变量
int maxfd;

void ClientSubject::clear() {
    this->clientfd = 0;
    this->name = "";
}

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
    pthread_t readThread;
    if ((pthread_create(&readThread, NULL, Server::Serve, NULL)) < 0) {
        perror("readThread pthread_create()");
    }

    while(1) {
        FD_ZERO(&server->fdset);
        FD_SET(server->sockfd, &server->fdset);
        if (select(server->sockfd + 1, &server->fdset, NULL, NULL, NULL) < 0) {
            perror("select()");
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
        cout<<"-----------------------------------"<<endl;
        cout<<"Registering..."<<endl;
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
        server->clients[server->ccfn++].clientfd = clientsock;
        server->clients[server->ccfn - 1].name = buff;
        res = string(buff);
        res = "*********************************\n"+res+" join the ChatRoom!\n*********************************\n";
        m.lock();
        maxfd = max(maxfd, clientsock);
        m.unlock();
        server->SendAll(res);
        res = "Just input to enjoy your chat!";
        send(clientsock, res.c_str(), strlen(res.c_str()), 0);

        cout<<"***********The Num of Online:"<<server->ccfn<<"***********"<<endl;
    } else {
        strcpy(buff, "ChatRoom is over the max connections!\n");
        send(clientsock, buff, sizeof(buff), 0);
        // 发送友好信息后断开连接
        close(clientsock);
    }
    return NULL;
}

void Server::Close() {
    // 服务器关闭，优雅地关闭资源
    // close监听socket
    close(server->sockfd);
    return;
}

void* Server::Serve(void* temp) {
    fd_set readset;
    struct timeval timeout;
    while (1) {
        // 设置超时时间为0秒0毫秒，则select检测不到变化就返回，不会阻塞
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        FD_ZERO(&readset);
        for (int i = 0; i < server->ccfn; i++) {
            FD_SET(server->clients[i].clientfd, &readset);
        }

        if (select(maxfd+1, &readset, NULL, NULL, &timeout) < 0) {
            perror("select()");
        }

        for (int i = 0; i < server->ccfn; i++) {
            if (FD_ISSET(server->clients[i].clientfd, &readset)) {
                if (server->Recv(server->clients[i]) < 0) {
                    // 退出
                    for (int j = i; j < server->ccfn - 1; j++) {
                        server->clients[j] = server->clients[j+1];
                    }
                    server->clients[server->ccfn].clear();
                    server->ccfn--;
                    cout<<"***********The Num of Online:"<<server->ccfn<<"***********"<<endl;
                    
                }
            }
        }
    }
    return NULL;
}

int Server::Recv(ClientSubject client) {
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
        // 如果信息为exit关键字，则关闭连接并使在线人数-1
        if (strcmp(buff, "exit") == 0) {
            // 退出处理
            cout<<client.name<<" "<<buff<<endl;
            res = "*********************************\n"+client.name+" exit\n*********************************\n";
            server->SendAll(res);
            // 返回错误信息
            return -1;
        }
        // 正常的信息广播处理
        res = "----------------"+ client.name +"---------------\n" + string(dt) + client.name + ":" + buff 
        + "-----------------------------------";
        server->SendAll(res);
    }
    return 0;
}

void Server::SendAll(string res) {
    // 广播信息
    for (int i = 0; i < server->ccfn; i++) {
        send(server->clients[i].clientfd, res.c_str(), strlen(res.c_str()), 0);
    }
}