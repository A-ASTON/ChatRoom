#ifndef ABSTRACTCLIENT_H
#define ABSTRACTCLIENT_H
class AbstractClient {
public:
    AbstractClient(){};
    // 客户端初始化，返回值为bool
    virtual void Init(int)=0;

    // 客户端启动，
    virtual void Start()=0;

    // 客户端关闭
    virtual void Close()=0;

    // 注册
    virtual void Register(int)=0;
    
    // 启动
    virtual void Run(int)=0;
};
#endif