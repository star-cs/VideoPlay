#pragma once
#include "Base.h"
#include "Socket.h"

class RTPHeader
{
public:	
	// 位域
	// 高位和低位的位置：
	// DataSheet中，默认左边是高位，低位在右边
	// 位域中先声明的，是低位的，后声明的是高位。所以需要倒置。
	unsigned short cc : 4;			// 用于指示CSRC（Contributing Source）标识符的个数。CSRC是参与源，即对RTP流做出贡献的源的数量。
	unsigned short extension : 1;	// 扩展，用于指示是否在该RTP报头之后存在一个扩展报头。如果X=1，则表示存在扩展报头
	unsigned short padding : 1;		// 填充，如果P=1，则表示存在填充数据，这些填充数据不是有效载荷的一部分。
	unsigned short version : 2;		// 位域		版本通常为2
	
	unsigned short pytype : 7;		// 占7位，用于说明RTP报文中有效载荷的类型。
	unsigned short marker : 1;		// 其含义取决于有效载荷的类型。对于视频数据，M通常用于标记一帧的结束；对于音频数据，M可能用于标记会话的开始或结束。

	unsigned short serial;			// 占16位，用于标识发送者所发送的RTP报文的序列号。每发送一个报文，序列号就会增加1。
	unsigned timestamp;				// 占32位，表示RTP报文中第一个八位组的采样时刻。时间戳的单位是RTP时间单位（RTP Timestamp Unit），其值以90kHz的时钟频率进行计数。时间戳用于同步和排序RTP报文。
	unsigned ssrc;					// 标明同步源，同步源是一个负责发送 RTP 分组并在 RTP 分组中设置序号和时间戳的实体，标识符是会话中全局惟一的，若 RTP 分组来自混合器则同步源标识符给出的是混合器的标识符
	unsigned csrc[15];				// 若 RTP 分组来自混合器则提供源标识符列表给出进入混合器的各个信号的信号源标识符

public:
	RTPHeader();
	RTPHeader(const RTPHeader& head);
	RTPHeader& operator=(const RTPHeader& head);
	operator MBuffer();
	
	
};

class RTPFrame
{
public:
	RTPFrame() {}
	~RTPFrame() {}
	
	operator MBuffer();

	RTPHeader m_head;
	MBuffer m_pyload;
};

class RTPHelper
{
public:
	RTPHelper() :timestamp(0), m_socket(MUDP) {
		m_socket.Bind(MAddress("0.0.0.0", (short)55000));	// 绑定端口
	}
	~RTPHelper() {}

	int SendMediaFrame(RTPFrame& rtpFrame, MBuffer& frame, const MAddress& addr);
	
private:
	int GetFrameSepSize(MBuffer& frame);

	int SendFrame(const MBuffer& frame, const MAddress& addr);
	DWORD timestamp = 0;
	MSocket m_socket;
};

