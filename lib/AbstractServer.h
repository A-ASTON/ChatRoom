#ifndef ABSTRACTSERVER_H
#define ABSTRACTSERVER_H
#include<string>
using namespace std;
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
        void clear();
        
};

class AbstractServer {
public:
    AbstractServer(){};
    // 初始化服务器
    virtual void Init(int)=0;

    // 启动服务器
    virtual void Start()=0;

    // 关闭服务器
    virtual void Close()=0;

    // 业务逻辑
    virtual int Recv(ClientSubject)=0;

    // 信息广播
    virtual void SendAll(string)=0;
};
#endif