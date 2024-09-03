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


// ׼������RTPFrame����RTPHeader��m_pyload�ϲ������кţ�ʱ�����ssrc��Ҫת���������ֽ���
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
    //RTPFrame rtpFrame;    Ҫ��֤���кţ�ʱ����ĳ�ʱ�䱣�������Դ��ν�������
    size_t frame_size = frame.size();
    int sep_size = GetFrameSepSize(frame);

    // ����ǰ3 / 4���ֽ� 
    frame_size -= sep_size;
    BYTE* pFrame = (BYTE*)frame + sep_size;

    if (frame_size > RTP_MAX_SIZE) {  //  ��Ƭ
        size_t count = frame_size / RTP_MAX_SIZE;
        size_t end_size = frame_size % RTP_MAX_SIZE;        // ���һ����Ƭ�Ĵ�С
        if (end_size > 0) {
            count++;
        }
        BYTE nalu = pFrame[0] & 0x1F;
        for (size_t i = 0; i < count; i++) {
            // ÿ�η�Ƭ�����·����ڴ�
            rtpFrame.m_pyload.resize(RTP_MAX_SIZE + 2);    // bug��ÿ�η�Ƭ��������ͷ�����ֽڡ������ RTP_MAX_SIZE + 2   
            memset(rtpFrame.m_pyload, 0, RTP_MAX_SIZE + 2);

            // m_pyload�����ֽ�: 
            // FU Indicator F(1)����0������1     NRI(2)��Ҫ�� 00��� 01 10�м� 11���     Type(5) NALU���ͣ�һ����28��0x1c=11100��H264�淶       
            // FU Header    S(1)��ʼ1���ǿ�ʼ0   E(1)����1���ǽ���0     R(1)������0       Type(5) NALU�����ͣ�֡�ĵ�һ���ֽڵĵ���λ��
            ((BYTE*)rtpFrame.m_pyload)[0] = 0x60 | 28;           // 0x7C
            ((BYTE*)rtpFrame.m_pyload)[1] = nalu;                // �м� 0000 0000
            if (i == 0) {
                ((BYTE*)rtpFrame.m_pyload)[1] |= 0x80;     // ��ʼ 1000 0000
            }
            else if (i == count - 1) {
                ((BYTE*)rtpFrame.m_pyload)[1] |= 0x40;     // ���� 0100 0000
            }

            if (i == count - 1 && end_size > 0) {
                memcpy((BYTE*)rtpFrame.m_pyload + 2, pFrame + i * RTP_MAX_SIZE + 1, end_size);
                rtpFrame.m_pyload.resize(end_size + 2);
            }
            else {
                memcpy((BYTE*)rtpFrame.m_pyload + 2, pFrame + i * RTP_MAX_SIZE + 1, RTP_MAX_SIZE);
            }

            // ��������
            // rtpFrameǿתMBuffer������RTPHeader�����кţ�ʱ�����ssrc��Ҫת���������ֽ���
            SendFrame(rtpFrame, addr);

            // ���к��ۼ�
            rtpFrame.m_head.serial++;

        }
    }
    else {
        //  ��Ƭ  ǣ�����ֽڲ��ö���
        rtpFrame.m_pyload.resize(frame.size() - sep_size);
        memcpy(rtpFrame.m_pyload, frame.data() + sep_size, frame.size() - sep_size);

        // TODO: rtp��header
        // ���к��ۼӣ�ʱ��������������0��ʼ��ÿ֡׷�� ʱ��Ƶ��/ÿ��֡��24
        // ���ͺ󣬼������ߣ��ȵ�udp������ɡ�û�����ߵĻ���udp�ķ����ٶȻ�ǳ��죬����udp�Ľ��ն˽��ղ������ݡ�

        // ��������
        SendFrame(rtpFrame, addr);

        rtpFrame.m_head.serial++;

    }
    // �趨ÿ֡ʱ���
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
    result += (MBuffer)m_head; // ����RTPHeader�����кţ�ʱ�����ssrc��Ҫת���������ֽ��� 
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
