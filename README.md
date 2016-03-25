# UninstallMonitor
Monitor self app to action url when uninstall by user for Android.
用于Android平台，对自身app进行卸载检测，检测到就自动跳转url。

# What's in it
c层fork一个子进程，并且使子进程的父进程指向init（pid=1）进程。
此子进程利用Linux inotify的文件监控机制，对/data/data/{pkg}文件进行监控，不存在表示已经卸载了

# How to use
直接调用监控入口：
UninstallMonitor.openUrlWhenUninstall(context, "http://www.baidu.com");

注：有防止进程重复生成机制处理
