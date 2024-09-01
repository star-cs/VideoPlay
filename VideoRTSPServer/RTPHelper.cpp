#include "RTPHelper.h"
#include <Windows.h>

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
    memcpy(buffer, this, size);
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
            // m_pyload�����ֽ�: 
            // FU Indicator F(1)����0������1     NRI(2)��Ҫ�� 00��� 01 10�м� 11���     Type(5) NALU���ͣ�һ����28��0x1c=11100��H264�淶       
            // FU Header    S(1)��ʼ1���ǿ�ʼ0   E(1)����1���ǽ���0     R(1)������0       Type(5) NALU�����ͣ�֡�ĵ�һ���ֽڵĵ���λ��
            ((BYTE*)rtpFrame.m_pyload)[0] = 0x60 | 28;
            ((BYTE*)rtpFrame.m_pyload)[1] = nalu;                // �м� 0000 0000
            if (i == 0) {
                ((BYTE*)rtpFrame.m_pyload)[1] = 0x80 | ((BYTE*)rtpFrame.m_pyload)[1];     // ��ʼ 1000 0000
            }
            else if (i == count - 1) {
                ((BYTE*)rtpFrame.m_pyload)[1] = 0x40 | ((BYTE*)rtpFrame.m_pyload)[1];     // ���� 0100 0000
            }

            if (i == count - 1 && end_size > 0) {
                memcpy((BYTE*)rtpFrame.m_pyload + 2, pFrame + i * RTP_MAX_SIZE, end_size);
            }
            else {
                memcpy((BYTE*)rtpFrame.m_pyload + 2, pFrame + i * RTP_MAX_SIZE, RTP_MAX_SIZE);
            }      

            // ���к��ۼ�
            rtpFrame.m_head.serial++;

            // ��������
            // rtpFrameǿתMBuffer������RTPHeader�����кţ�ʱ�����ssrc��Ҫת���������ֽ���
            SendFrame(rtpFrame, addr);
        }
    }
    else {
        //  ��Ƭ  ǣ�����ֽڲ��ö���
        rtpFrame.m_pyload.resize(frame.size() - sep_size);
        memcpy(rtpFrame.m_pyload, frame.data() + sep_size, frame.size() - sep_size);

        // TODO: rtp��header
        // ���к��ۼӣ�ʱ��������������0��ʼ��ÿ֡׷�� ʱ��Ƶ��/ÿ��֡��24
        // ���ͺ󣬼������ߣ��ȵ�udp������ɡ�û�����ߵĻ���udp�ķ����ٶȻ�ǳ��죬����udp�Ľ��ն˽��ղ������ݡ�

        rtpFrame.m_head.serial++;

        // ��������
        SendFrame(rtpFrame, addr);
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
    int ret = sendto(m_socket, frame, frame.size(), 0, addr, sizeof(addr));
    return ret;
}
