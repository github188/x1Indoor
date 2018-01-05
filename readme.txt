3、抓拍图片保存没看懂，数据直接用H264码流I帧数据？


1）板子上运行程序需要给libminigui_ths-3.0.so.12.0.0 文件增加连接文件
	libminigui_ths.so 和 文件 libminigui_ths-3.0.so.12
2）另外png文件看具体情况可能需要给 libpng12.so.0.37.0 文件增加链接文件
	libpng.so libpng12.so libpng12.so.0
3）另外lz文件看具体情况可能需要给 libz.so.1.2.11 文件增加链接文件
	libz.so libz.so.1

mount -o nolock -t nfs 10.110.114.17:/home/kf304/Working/work_path/x1/mount_dir /mnt


音频参数：
	ao增益



修改记录：
1、将gui/comm底下 au_language.h 提到storage/comm下面 逻辑尽量不要调用gui
2、