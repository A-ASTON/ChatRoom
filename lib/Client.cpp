#include "Client.h"

Client::Client() {
    
}

int Client::Init() {
    memset(&this->clientaddr, 0, sizeof(this->clientaddr));
    // 协议域
    this->clientaddr.sin_family = AF_INET;
    // 发送地址,通过inet_pton将点分十进制ip地址转化为网络字节序二进制
    // 此处默认为127.0.0.1,即本机地址
    inet_pton(AF_INET, "127.0.0.1", &this->clientaddr.sin_addr);
    // 发送端口
    this->clientaddr.sin_port = htons(8099);
    return 0;
}

int Client::Start() {
    int n;
    char recvline[4096], sendline[4096];
    // 创建socket描述字
    if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout<<"Create socket error!error:"<<strerror(errno)<<endl;
        return -1;
    }
    // 创建connection
    if (connect(sockfd, (struct sockaddr*)&this->clientaddr, sizeof(this->clientaddr)) < 0) {
        cout<<"Accept socket error!error:"<<strerror(errno)<<endl;
        return -1;
    }
    
    // 先注册(简化为向服务器发送昵称)
    Register(sockfd);

    // 注册完后，调用Run，开始聊天
    Run(sockfd);

    // 关闭sockfd连接，减少引用计数
    close(sockfd);

    return 0;
}

int Client::Register(int sockfd) {
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

    // 获取欢迎信息
    recv(sockfd, buff, sizeof(buff), 0);
    cout<<buff<<endl;
    // close(sockfd);
    return 0;
}

void* recvMessage(void* socket) {
    const int sockfd = *((int*)socket);
    char buff[MAXLINE];
    int len;
    while (1) {
        if ((len =  recv(sockfd, buff, sizeof(buff), 0)) <= 0) {
            perror("recv()");
            break;
        } else {
            buff[len] = '\0';
            cout<<buff<<endl;
        }
    }
    return 0;
}

void* sendMessage(void* socket) {
    const int sockfd = *((int*)socket);
    char buff[MAXLINE];
    while (1) {
        fgets(buff, MAXLINE, stdin);
        if (send(sockfd, buff, strlen(buff), 0) < 0) {
            perror("send()");
            close(sockfd);
            break;
        }
    }
    return 0;
}

int Client::Run(int sockfd) {
    // 利用pthread创建读写线程
    pthread_t threads[2];
    if (pthread_create(&threads[0], NULL, recvMessage, (void*)&(sockfd)) != 0) {
        perror("pthread_create()");
    }
    if (pthread_create(&threads[1], NULL, sendMessage, (void *)&(sockfd)) != 0) {
        perror("pthread_create()");
    }
    // 不让主线程结束
    while(1);
    pthread_exit(NULL);
    return 0;
}

int Client::Close() {
    return 0;
}