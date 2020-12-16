# C++ 利用线程池实现多人聊天室

## 具体步骤

- 实现聊天室（单线程，服务器阻塞等待连接，创建单条TCP连接）
- 添加多线程功能（多人同时登陆，服务器等待连接，创建线程处理连接）
- 使用线程池实现线程管理（控制最大连接数、线程调度）
- 进一步研究网络模型，进一步进行改进

## 当前进度
- CMakeLists.txt cmake分开编译和运行ServerMain和ClientMain成功


## 接下来要做的事
- 编写Server和Client，实现单线程利用socket进行简单的信息互通