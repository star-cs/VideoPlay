# vlc��ļ���
> ���ϻ����� x86 Debug����������

1. ���������ɵ�vlc����ӵ���Ŀ��  
2. ��Ŀ ���� VC++Ŀ¼ ����Ŀ¼ ����� '.' ,  'include' , 'include\vlc')    ��������  
3. ��Ŀ ���� C/C++ ���Ӱ���Ŀ¼����� '.' ,  'include' , 'include\vlc'��   ���������  
4. ��Ŀ ���� ������ ���� ������������ 'libvlc.lib' , 'libvlccore.lib'��  
5. ��Ŀ ���� ������ ���� ���ӿ�Ŀ¼����� 'lib'��  

> ���������Debug�ļ����ڵ����ɵ�exe�ļ�����Ҫ��� libvlc.dll �� libvlccore.dll �� plugins�ļ���  
> ͷ�ļ� vlc/vlc.h  
> libVLCʹ��utf-8���룬�����������ģ���Ҫת�������Ч��   

 # vlc�� API

1. libvlc_new  
2. libvlc_media_new_path  
3. libvlc_media_player_new_from_media  
4. libvlc_media_player_play  
5. libvlc_media_player_get_length libvlc_video_get_width  libvlc_video_get_height  
6. libvlc_media_player_pause libvlc_media_player_play libvlc_media_player_stop  
7. libvlc_media_player_release libvlc_media_release libvlc_release  

## �������̣�
��ʼ�� VLC ʵ��������һ�� VLC ʵ�������ں�����ý�岥�Ų�����
������־���û�����������־��ϸ������û�������Ϣ���Ա���Ժͱ�ʶ��
����ý����󣺴�ָ�����ļ�·������һ��ý�����׼�����š�
����ý�岥��������ý����󴴽�һ��ý�岥���������ڿ��Ʋ��š�
��ʼ����ý�壺���ò��ź�����ʼ����ý�塣
�ȴ�������ɣ���������ֱ�����Ž�����������
�ͷ���Դ���ͷ�ý�岥������ý������ VLC ʵ���������ڴ档
