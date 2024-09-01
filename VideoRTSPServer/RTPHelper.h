#pragma once
#include "Base.h"
#include "Socket.h"

class RTPHeader
{
public:	
	// λ��
	// ��λ�͵�λ��λ�ã�
	// DataSheet�У�Ĭ������Ǹ�λ����λ���ұ�
	// λ�����������ģ��ǵ�λ�ģ����������Ǹ�λ��������Ҫ���á�
	unsigned short cc : 4;			// ����ָʾCSRC��Contributing Source����ʶ���ĸ�����CSRC�ǲ���Դ������RTP���������׵�Դ��������
	unsigned short extension : 1;	// ��չ������ָʾ�Ƿ��ڸ�RTP��ͷ֮�����һ����չ��ͷ�����X=1�����ʾ������չ��ͷ
	unsigned short padding : 1;		// ��䣬���P=1�����ʾ����������ݣ���Щ������ݲ�����Ч�غɵ�һ���֡�
	unsigned short version : 2;		// λ��		�汾ͨ��Ϊ2
	
	unsigned short pytype : 7;		// ռ7λ������˵��RTP��������Ч�غɵ����͡�
	unsigned short marker : 1;		// �京��ȡ������Ч�غɵ����͡�������Ƶ���ݣ�Mͨ�����ڱ��һ֡�Ľ�����������Ƶ���ݣ�M�������ڱ�ǻỰ�Ŀ�ʼ�������

	unsigned short serial;			// ռ16λ�����ڱ�ʶ�����������͵�RTP���ĵ����кš�ÿ����һ�����ģ����кžͻ�����1��
	unsigned timestamp;				// ռ32λ����ʾRTP�����е�һ����λ��Ĳ���ʱ�̡�ʱ����ĵ�λ��RTPʱ�䵥λ��RTP Timestamp Unit������ֵ��90kHz��ʱ��Ƶ�ʽ��м�����ʱ�������ͬ��������RTP���ġ�
	unsigned ssrc;					// ����ͬ��Դ��ͬ��Դ��һ�������� RTP ���鲢�� RTP ������������ź�ʱ�����ʵ�壬��ʶ���ǻỰ��ȫ��Ωһ�ģ��� RTP �������Ի������ͬ��Դ��ʶ���������ǻ�����ı�ʶ��
	unsigned csrc[15];				// �� RTP �������Ի�������ṩԴ��ʶ���б�������������ĸ����źŵ��ź�Դ��ʶ��

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
		m_socket.Bind(MAddress("0.0.0.0", (short)55000));	// �󶨶˿�
	}
	~RTPHelper() {}

	int SendMediaFrame(RTPFrame& rtpFrame, MBuffer& frame, const MAddress& addr);
	
private:
	int GetFrameSepSize(MBuffer& frame);

	int SendFrame(const MBuffer& frame, const MAddress& addr);
	DWORD timestamp = 0;
	MSocket m_socket;
};

