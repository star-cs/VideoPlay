# vlc库的集成
> 以上环境在 x86 Debug环境下配置

1. 将编译生成的vlc库添加到项目中  
2. 项目 属性 VC++目录 包含目录 （添加 '.' ,  'include' , 'include\vlc')    环境变量  
3. 项目 属性 C/C++ 附加包含目录（添加 '.' ,  'include' , 'include\vlc'）   编译带参数  
4. 项目 属性 链接器 输入 附加依赖项（添加 'libvlc.lib' , 'libvlccore.lib'）  
5. 项目 属性 链接器 常规 附加库目录（添加 'lib'）  

> 如果想运行Debug文件夹内的生成的exe文件，需要添加 libvlc.dll ， libvlccore.dll 和 plugins文件夹  
> 头文件 vlc/vlc.h  
> libVLC使用utf-8编码，参数中有中文，需要转码才能生效。   

 # vlc库 API

1. libvlc_new  
2. libvlc_media_new_path  
3. libvlc_media_player_new_from_media  
4. libvlc_media_player_play  
5. libvlc_media_player_get_length libvlc_video_get_width  libvlc_video_get_height  
6. libvlc_media_player_pause libvlc_media_player_play libvlc_media_player_stop  
7. libvlc_media_player_release libvlc_media_release libvlc_release  

## 基本流程：
初始化 VLC 实例：创建一个 VLC 实例，用于后续的媒体播放操作。
设置日志和用户代理：配置日志详细级别和用户代理信息，以便调试和标识。
创建媒体对象：从指定的文件路径创建一个媒体对象，准备播放。
创建媒体播放器：从媒体对象创建一个媒体播放器，用于控制播放。
开始播放媒体：调用播放函数开始播放媒体。
等待播放完成：阻塞程序，直到播放结束或发生错误。
释放资源：释放媒体播放器、媒体对象和 VLC 实例，清理内存。
