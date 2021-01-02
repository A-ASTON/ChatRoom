#include "Client.h"

// 信号标志
volatile sig_atomic_t flag = 0;

static void handler(int sig) {
    flag = 1;
}

void Client::Init(int port) {
    memset(&this->clientaddr, 0, sizeof(this->clientaddr));
    // 协议域
    this->clientaddr.sin_family = AF_INET;
    // 发送地址,通过inet_pton将点分十进制ip地址转化为网络字节序二进制
    // 此处默认为127.0.0.1,即本机地址
    inet_pton(AF_INET, "127.0.0.1", &this->clientaddr.sin_addr);
    // 发送端口
    this->clientaddr.sin_port = htons(port);
    return;
}

void Client::Start() {
    int n;
    char recvline[4096], sendline[4096];
    // 创建socket描述字
    if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout<<"Create socket error!error:"<<strerror(errno)<<endl;
    }
    // 创建connection
    if (connect(sockfd, (struct sockaddr*)&this->clientaddr, sizeof(this->clientaddr)) < 0) {
        cout<<"Accept socket error!error:"<<strerror(errno)<<endl;
    }
    
    // 先注册(简化为向服务器发送昵称)
    Register(sockfd);

    // 注册完后，调用Run，开始聊天
    Run(sockfd);

    // 主线程在Run函数中，等待Run返回后Start返回即可
    Close();
    return;
}

void Client::Register(int sockfd) {
    // 阻塞读写即可，获取需注册的信息，因为不注册无法加入聊天室
    int len;
    char buff[MAXLINE];
    len = recv(sockfd, buff, sizeof(buff), 0);
    if (len < 0) {
        perror("recv()");
    }
    buff[len] = '\0';
    cout<<buff<<endl;

    // 获取标准输入流的输入，用户名
    fgets(buff, MAXLINE, stdin);
    send(sockfd, buff, strlen(buff), 0);

    // 清buff！！
    memset(buff, '\0', sizeof(buff));
    // 获取欢迎信息
    recv(sockfd, buff, sizeof(buff), 0);
    cout<<buff<<endl;
    // close(sockfd);
}

void Client::Run(int sockfd) {
    // 利用pthread创建读写线程
    pthread_t threads[2];
    if (pthread_create(&threads[0], NULL, Client::recvMessage, (void*)&(sockfd)) != 0) {
        perror("pthread_create()");
    }
    if (pthread_create(&threads[1], NULL, Client::sendMessage, (void*)&(sockfd)) != 0) {
        perror("pthread_create()");
    }
    // 主线程检测关闭信号
    string buff;
    while(1) {
        signal(SIGINT, handler);
        if (flag) {
            // 检测到信号，执行处理
            buff = "exit";
            send(sockfd, buff.c_str(), strlen(buff.c_str()), 0);
            break;
        }
    }
    cout<<"exit"<<endl;
    return;
}

void* Client::recvMessage(void* socket) {
    const int sockfd = *((int*)socket);
    char buff[MAXLINE];
    int len;
    while (1) {
        // buff清空
        memset(buff, '\0', sizeof(buff));
        if ((len =  recv(sockfd, buff, sizeof(buff), 0)) <= 0) {
            perror("recv()");
            flag = 1;
            break;
        } else {
            buff[len] = '\0';
            cout<<buff<<endl;
        }
    }
    close(sockfd);
    return 0;
}

void* Client::sendMessage(void* socket) {
    const int sockfd = *((int*)socket);
    char buff[MAXLINE];
    while (1) {
        fgets(buff, MAXLINE, stdin);
        if (send(sockfd, buff, strlen(buff), 0) < 0) {
            perror("send()");
            flag = 1;
            break;
        }
    }
    close(sockfd);
    return 0;
}

void Client::Close() {
    close(sockfd);
    return;
}