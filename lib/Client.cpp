#include "Client.h"

Client::Client() {
    cout<<"ok"<<endl;
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
    if((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout<<"Create socket error!error:"<<strerror(errno)<<endl;
        return -1;
    }
    // 创建connection
    if(connect(sockfd, (struct sockaddr*)&this->clientaddr, sizeof(this->clientaddr)) < 0) {
        cout<<"Accept socket error!error:"<<strerror(errno)<<endl;
        return -1;
    }
    char buff[4096];
    // 先尝试读,注册信息
    n = recv(sockfd, buff, MAXLINE, 0);
    buff[n] = '\0';
    cout<<"Message from server:\n"<<buff<<endl;
    fgets(sendline, 4096, stdin);
    if(send(sockfd, sendline, strlen(sendline), 0) < 0) {
        cout<<"Send message error!error:"<<strerror(errno)<<endl;
        return -1;
    }
    n = recv(sockfd, buff, MAXLINE, 0);
    buff[n] = '\0';
    cout<<buff<<endl;
    while(1) {
        cout<<"Send message to server:"<<endl;
        fgets(sendline, 4096, stdin);
        cout<<sendline<<endl;
        if(send(sockfd, sendline, strlen(sendline), 0) < 0) {
            cout<<"Send message error!error:"<<strerror(errno)<<endl;
            return -1;
        }
        n = recv(sockfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        cout<<buff<<endl;
    }
    

    // close(sockfd);
    return 0;
}

int Client::Close() {
    return 0;
}