```c++
    UINT val = *(UINT*)(buffer.size() - 4 + (char*)buffer);
    if (val == *(UINT*)"\r\n\r\n") {        // RTSP完整包 \r\n\r\n 结尾
	    break;
    } 


    // 创建一个sessionId
    UUID uuid;
    UuidCreate(&uuid);

    m_id.resize(8);
    snprintf((char*)m_id.c_str(), m_id.size(), "%08d", uuid.Data1);


int RTPHelper::GetFrameSepSize(MBuffer& frame)
{
    BYTE buf[] = { 0,0,0,1 };
    if (memcmp(frame, buf, 4) == 0) return 4;
    return 3;
}
```

# 宏重定义
把套接字的头文件，放到window.h前，或者尽可能放到引用头文件的前面。  

# 重定义
多个cpp 文件都引用了同一个头文件，导致头文件内的函数被多次定义。 -> 把函数实现放到cpp 文件中，避免重复定义。

# 缺库 
严重性	代码	说明	项目	文件	行	禁止显示状态	详细信息    
错误	LNK2019	无法解析的外部符号 __imp__UuidCreate@4，函数 "public: __thiscall RTSPSession::RTSPSession(class MSocket const &)" (??0RTSPSession@@QAE@ABVMSocket@@@Z) 中引用了该符号	VideoRTSPServer	E:\Cpp_Projects\VideoPlay\VideoRTSPServer\RTSPServer.obj	1		

=======================================================     
include <rpc.h>  
#pragma comment(lib, "rpcrt4.lib") 

# 缺库 
严重性	代码	说明	项目	文件	行	禁止显示状态	详细信息  
错误	LNK2019	无法解析的外部符号 __imp__accept@12，函数 "public: class MSocket __thiscall MSocket::Accept(class MAddress &)" (?Accept@MSocket@@QAE?AV1@AAVMAddress@@@Z) 中引用了该符号	VideoRTSPServer	E:\Cpp_Projects\VideoPlay\VideoRTSPServer\RTSPServer.obj	1		

=======================================================  
#pragma comment(lib, "ws2_32.lib")	// 网络


# 调试
socket返回-1，参数没问题。有可能是 WSAStartup() 没有调用。


# 位域
```c++
    unsigned short cc : 4;
	unsigned short extension : 1;
	unsigned short padding : 1;
	unsigned short version : 2;	// 位域
```
高位和低位的位置：  
DataSheet中，默认左边是高位，低位在右边  
位域中先声明的，是低位的，后声明的是高位。以字节为单位。  
所以每个字节的变量声明需要倒置。  


# 通过socket获取远端IP
```c++
MAddress RTSPSession::GetClientUDPAddress() const
{
    MAddress addr;
    int len = addr.size();
    getsockname(m_client, addr, &len);  //获取客户端地址
    addr.Fresh();
    addr.setPort(m_port);               //设置客户端的udp端口。
    return addr;
}
```

> #include <sys/socket.h>  
> int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);  
> sockfd：这是套接字描述符，标识一个打开的套接字。  
> addr：指向 struct sockaddr 类型结构体的指针，该结构体用于存储本地地址信息。  
> addrlen：指向 socklen_t 类型变量的指针，该变量用于指定 addr 结构体的大小，并且在调用后会被更新为实际使用的大小。  


# DESCRIBE
服务器回应 DESCRIBE消息中，拼接的sdp前面，一定要多添加一个\r\n。