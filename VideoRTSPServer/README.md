```c++
    UINT val = *(UINT*)(buffer.size() - 4 + (char*)buffer);
    if (val == *(UINT*)"\r\n\r\n") {        // RTSP������ \r\n\r\n ��β
	    break;
    } 


    // ����һ��sessionId
    UUID uuid;
    UuidCreate(&uuid);

    m_id.resize(8);
    snprintf((char*)m_id.c_str(), m_id.size(), "%08d", uuid.Data1);
```

# ���ض���
���׽��ֵ�ͷ�ļ����ŵ�window.hǰ�����߾����ܷŵ�����ͷ�ļ���ǰ�档  

# �ض���
���cpp �ļ���������ͬһ��ͷ�ļ�������ͷ�ļ��ڵĺ�������ζ��塣 -> �Ѻ���ʵ�ַŵ�cpp �ļ��У������ظ����塣

# ȱ�� 
������	����	˵��	��Ŀ	�ļ�	��	��ֹ��ʾ״̬	��ϸ��Ϣ    
����	LNK2019	�޷��������ⲿ���� __imp__UuidCreate@4������ "public: __thiscall RTSPSession::RTSPSession(class MSocket const &)" (??0RTSPSession@@QAE@ABVMSocket@@@Z) �������˸÷���	VideoRTSPServer	E:\Cpp_Projects\VideoPlay\VideoRTSPServer\RTSPServer.obj	1		

=======================================================     
include <rpc.h>  
#pragma comment(lib, "rpcrt4.lib") 

# ȱ�� 
������	����	˵��	��Ŀ	�ļ�	��	��ֹ��ʾ״̬	��ϸ��Ϣ  
����	LNK2019	�޷��������ⲿ���� __imp__accept@12������ "public: class MSocket __thiscall MSocket::Accept(class MAddress &)" (?Accept@MSocket@@QAE?AV1@AAVMAddress@@@Z) �������˸÷���	VideoRTSPServer	E:\Cpp_Projects\VideoPlay\VideoRTSPServer\RTSPServer.obj	1		

=======================================================  
#pragma comment(lib, "ws2_32.lib")	// ����


# ����
socket����-1������û���⡣�п����� WSAStartup() û�е��á�

