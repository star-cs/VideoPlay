// VideoPlay.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>
#include <iostream>
#include "vlc/vlc.h"
#include <conio.h>

/*
1. libvlc_new
2. libvlc_media_new_path
3. libvlc_media_player_new_from_media
4. libvlc_media_player_play
5. libvlc_media_player_get_length libvlc_video_get_width  libvlc_video_get_height
6. libvlc_media_player_pause libvlc_media_player_play libvlc_media_player_stop
7. libvlc_media_player_release libvlc_media_release libvlc_release
*/

std::string Unicode2Utf8(const std::wstring& strIn)
{
	std::string str;
	int length = ::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), NULL, 0, NULL, NULL);
	str.resize(length + 1);
	::WideCharToMultiByte(CP_UTF8, 0, strIn.c_str(), strIn.size(), (LPSTR)str.c_str(), length, NULL, NULL);
	return str;
}


int main()
{
	int argc = 1;
	char* argv[2];
	argv[0] = (char*)"--ignore-config";		//多字节 - Unicode - utf-8
	libvlc_instance_t* vlc_ins = libvlc_new(argc, argv);
	
	std::string path = Unicode2Utf8(L"test.mp4");

	libvlc_media_t* media = libvlc_media_new_path(vlc_ins, path.c_str());
	// libvlc_media_t* media = libvlc_media_new_location(vlc_ins, "file:///E:\\Cpp_Projects\\VideoPlay\\VideoPlay\\test.mp4");
	libvlc_media_player_t* player = libvlc_media_player_new_from_media(media);
	do {
		int ret = libvlc_media_player_play(player);
		if (ret == -1) {
			printf("errror found! \r\n");
			break;
		}

		// 音视频 存在不确定性
		int vol = -1;
		while (vol == -1) {
			Sleep(10);
			vol = libvlc_audio_get_volume(player);
		}

		//Sleep(3000);	// 媒体解析加载完成，才能拿到参数
		//int vol = libvlc_audio_get_volume(player);
		printf("volume is %d \r\n", vol);
		libvlc_audio_set_volume(player, 10);

		libvlc_time_t tm = libvlc_media_player_get_length(player);
		printf("%02d:%02d%:%02d:%03d\r\n", int(tm / 3600000), int(tm / 60000) % 60, int(tm / 1000) % 60, int(tm) % 1000);
		int width = libvlc_video_get_width(player);
		int height = libvlc_video_get_height(player);
		printf("width = %d , height = %d \r\n", width, height);

		while (!_kbhit()) {
			printf("%f%%\r", 100.0 * libvlc_media_player_get_position(player));
			Sleep(500);
		}

		int k = 0;
		while (std::cin >> k) {
		

			if (k == 1) {
				libvlc_media_player_pause(player);
			}
			else if (k == 2) {
				libvlc_media_player_play(player);
			}
			else if (k == 3) {
				libvlc_media_player_stop(player);
				break;
			}
		}

	} while (0);

	libvlc_media_player_release(player);
	libvlc_media_release(media);
	libvlc_release(vlc_ins);

	return 0;
}