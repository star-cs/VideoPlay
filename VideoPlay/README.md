# vlc库的集成

1. 将编译生成的vlc库添加到项目中  
2. 项目 属性 VC++目录 包含目录 （添加 '.' ,  'include' , 'include\vlc')    环境变量  
3. 项目 属性 C/C++ 附加包含目录（添加 '.' ,  'include' , 'include\vlc'）   编译带参数  
4. 项目 属性 链接器 输入 附加依赖项（添加 'libvlc.lib' , 'libvlccore.lib'）  
5. 项目 属性 链接器 常规 附加库目录（添加 'lib'）  

> 如果想运行Debug文件夹内的生成的exe文件，需要添加 libvlc.dll ， libvlccore.dll 和 plugins文件夹
> 头文件 vlc/vlc.h
> libVLC使用utf-8编码，参数中有中文，需要转码才能生效。
