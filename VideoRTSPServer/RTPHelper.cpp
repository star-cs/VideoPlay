#include "RTPHelper.h"
#include <Windows.h>
#include "CTool.h"		

#define RTP_MAX_SIZE 1300

RTPHeader::RTPHeader(const RTPHeader& head)
{
    memset(csrc, 0, sizeof(csrc));
    int size = cc * 4 + 12;
    memcpy(this, &head, size);
}

RTPHeader& RTPHeader::operator=(const RTPHeader& head)
{
    if (this != &head) {
        memset(csrc, 0, sizeof(csrc));
        int size = cc * 4 + 12;
        memcpy(this, &head, size);
    }
    return *this;
}

RTPHeader::RTPHeader()
{
    version = 2;  
    padding = 0;
    extension = 0;
    cc = 0;

    marker = 0;
    pytype = 96;

    serial = 0;
    timestamp = 0;
    ssrc = 0x98765432;

    memset(csrc, 0, sizeof(csrc));
}


// 准备发送RTPFrame，将RTPHeader和m_pyload合并。序列号，时间戳，ssrc需要转换成网络字节序。
RTPHeader::operator MBuffer()
{
    RTPHeader head = *this;
    head.serial = htons(head.serial);
    head.timestamp = htonl(head.timestamp);
    head.ssrc = htonl(head.ssrc);

    int size = cc * 4 + 12;
    MBuffer buffer(size);
    memcpy(buffer, &head, size);

    return buffer;
}


int RTPHelper::SendMediaFrame(RTPFrame& rtpFrame, MBuffer& frame, const MAddress& addr)
{
    //RTPFrame rtpFrame;    要保证序列号，时间戳的长时间保留。所以传参进来处理。
    size_t frame_size = frame.size();
    int sep_size = GetFrameSepSize(frame);

    // 跳过前3 / 4个字节 
    frame_size -= sep_size;
    BYTE* pFrame = (BYTE*)frame + sep_size;

    if (frame_size > RTP_MAX_SIZE) {  //  分片
        size_t count = frame_size / RTP_MAX_SIZE;
        size_t end_size = frame_size % RTP_MAX_SIZE;        // 最后一个分片的大小
        if (end_size > 0) {
            count++;
        }
        BYTE nalu = pFrame[0] & 0x1F;
        for (size_t i = 0; i < count; i++) {
            // 每次分片，重新分配内存
            rtpFrame.m_pyload.resize(RTP_MAX_SIZE + 2);    // bug：每次分片，会少了头两个字节。解决： RTP_MAX_SIZE + 2   
            memset(rtpFrame.m_pyload, 0, RTP_MAX_SIZE + 2);

            // m_pyload两个字节: 
            // FU Indicator F(1)正常0，错误1     NRI(2)重要性 00最低 01 10中间 11最高     Type(5) NALU类型，一般是28（0x1c=11100）H264规范       
            // FU Header    S(1)开始1，非开始0   E(1)结束1，非结束0     R(1)保留，0       Type(5) NALU的类型，帧的第一个字节的低五位。
            ((BYTE*)rtpFrame.m_pyload)[0] = 0x60 | 28;           // 0x7C
            ((BYTE*)rtpFrame.m_pyload)[1] = nalu;                // 中间 0000 0000
            if (i == 0) {
                ((BYTE*)rtpFrame.m_pyload)[1] |= 0x80;     // 开始 1000 0000
            }
            else if (i == count - 1) {
                ((BYTE*)rtpFrame.m_pyload)[1] |= 0x40;     // 结束 0100 0000
            }

            if (i == count - 1 && end_size > 0) {
                memcpy((BYTE*)rtpFrame.m_pyload + 2, pFrame + i * RTP_MAX_SIZE + 1, end_size);
                rtpFrame.m_pyload.resize(end_size + 2);
            }
            else {
                memcpy((BYTE*)rtpFrame.m_pyload + 2, pFrame + i * RTP_MAX_SIZE + 1, RTP_MAX_SIZE);
            }

            // 发送数据
            // rtpFrame强转MBuffer，其中RTPHeader的序列号，时间戳，ssrc需要转换成网络字节序。
            SendFrame(rtpFrame, addr);

            // 序列号累加
            rtpFrame.m_head.serial++;

        }
    }
    else {
        //  单片  牵连个字节不用动。
        rtpFrame.m_pyload.resize(frame.size() - sep_size);
        memcpy(rtpFrame.m_pyload, frame.data() + sep_size, frame.size() - sep_size);

        // TODO: rtp的header
        // 序列号累加，时间戳计算出来，从0开始，每帧追加 时钟频率/每秒帧率24
        // 发送后，加入休眠，等等udp发送完成。没有休眠的话，udp的发送速度会非常快，导致udp的接收端接收不到数据。

        // 发送数据
        SendFrame(rtpFrame, addr);

        rtpFrame.m_head.serial++;

    }
    // 设定每帧时间戳
    rtpFrame.m_head.timestamp += 90000 / 24;

    Sleep(1000 / 30);
    return 0;

}


int RTPHelper::GetFrameSepSize(MBuffer& frame)
{
    BYTE buf[] = { 0,0,0,1 };
    if (memcmp(frame, buf, 4) == 0) return 4;
    return 3;
}

RTPFrame::operator MBuffer()
{
    MBuffer result;
    result += (MBuffer)m_head; // 其中RTPHeader的序列号，时间戳，ssrc需要转换成网络字节序。 
    result += m_pyload;

    return result;
}


int RTPHelper::SendFrame(const MBuffer& frame, const MAddress& addr)
{
    fwrite(frame, 1, frame.size(), m_file);
    fwrite("00000000", 1, 8, m_file);
    fflush(m_file);
    int ret = sendto(m_socket, frame, frame.size(), 0, addr, sizeof(addr));
    printf("ret = %d ip = %s port = %d \r\n", ret , addr.Ip().c_str(), addr.Port());
    return ret;
}
