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

