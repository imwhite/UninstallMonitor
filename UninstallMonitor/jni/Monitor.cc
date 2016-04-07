/*
 * Monitor.cc
 *
 *  Created on: 2016-3-15
 *      Author: white
 */
/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <sys/prctl.h>
#include <Utils.h>

/* 宏定义begin */
//清0宏
#define MEM_ZERO(pDest, destSize) memset(pDest, 0, destSize)

/* LOG宏定义 */
#define LOG_TAG "Monitor"
#define LOG_D(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)
#define LOG_E(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args)

/**	调用java路径	 */
#define NAME_JAVA_CLASS "com/uninstall/monitor/JNIHelper"
#define NAME_JAVA_METHOD "openUrl"



/**
 * c层打开url
 * 扩展：可以执行其他shell命令，am(即activity manager)，可以打开某程序、服务，broadcast intent，等等
 */
void doOpenUrlByNative(char* activity, char* action,
		char* action_data, char* userSerial) {

	if (userSerial == NULL || strcmp(userSerial, "null") == 0) {
		// 执行命令am start -a android.intent.action.VIEW -d $(url)
		execlp("am", "am", "start", "-a", "android.intent.action.VIEW", "-d",
				action_data, (char *) NULL);
	}
	else {
		if (activity == NULL || strcmp(activity, "null") == 0) {
			execlp("am", "am", "start", "--user", userSerial, "-a", action, "-d", action_data, (char *) NULL);
		}
		else {
			if (action == NULL || strcmp(action, "null") == 0) {
				if (action_data == NULL || strcmp(action_data, "null") == 0) {
					execlp("am", "am", "start", "--user",
							userSerial, "-n",
							activity,
							(char *) NULL);
				} else {
					execlp("am", "am", "start", "--user",
							userSerial, "-n",
							activity, "-d",
							action_data,
							(char *) NULL);
				}
			} else {
				if (action_data == NULL || strcmp(action_data, "null") == 0) {
					execlp("am", "am", "start", "--user",
							userSerial, "-n",
							activity, "-a",
							action,
							(char *) NULL);
				} else {
					execlp("am", "am", "start", "--user",
							userSerial, "-n",
							activity, "-a",
							action, "-d",
							action_data,
							(char *) NULL);
				}
			}
		}
	}
}



char* doStartAndOpenUrl(char* targetDir, char* activity, char* action, char* url, char* userSerial) {

	LOG_D("start------------------");

	//fork子进程，这时候会有两个进程执行下面的代码，返回的pid对于父进程是子进程的id，对于子进程id=0
	pid_t pid = fork();
	char showpid[10];
	sprintf(showpid, "%d", pid);
	LOG_D("fork pid=%d", pid);

	if (pid < 0) {
		LOG_E("fork failed !!!");
	}
	// 第一个子进程执行
	else if (pid == 0) {
		LOG_D("child process execute");

		// 第一个子进程再fork第二个子进程，第二个使对应父进程是init（1）
		pid_t pid = fork();
		sprintf(showpid, "%d", pid);
		// 第一个子进程执行
		if(pid != 0){
			char showpid[10];
			sprintf(showpid, "%d", pid);
			LOG_D("child process fork pid=%d", pid);
			exit(1);
		}

		// 第二个子进程执行
		// 进程重命名
		prctl(PR_SET_NAME, "uninstall_monitor", NULL, NULL, NULL);
		// 初始化文件监听
		int fileDescriptor = inotify_init();
		if (fileDescriptor < 0) {
			LOG_E("inotify_init failed !!");
			exit(1);
		}

		const char* targetDirPath = targetDir;
		LOG_D("target file %s", targetDirPath);
		// 子进程注册"/data/data/{package}"目录监听器
		int watchDescriptor;
		watchDescriptor = inotify_add_watch(fileDescriptor, targetDirPath, IN_DELETE);
		if (watchDescriptor < 0) {
			LOG_D("inotify_add_watch failed !!!");
			exit(1);
		}

		//分配缓存，以便读取event，缓存大小=一个struct inotify_event的大小，这样一次处理一个event
		void *p_buf = malloc(sizeof(struct inotify_event));
		if (p_buf == NULL) {
			LOG_D("malloc failed !!!");
			exit(1);
		}

		// 开始监听
		LOG_D("start monitor...");
		size_t readBytes = read(fileDescriptor, p_buf, sizeof(struct inotify_event));

		LOG_D("receive result:");
		// 文件操作有event：文件还在，覆盖安装
		FILE *p_appDir = fopen(targetDir, "r");
		if (p_appDir != NULL) {
			LOG_D("reinstall");
			exit(1);
		}

		// 文件操作有event：文件不在已经卸载了，释放
		free(p_buf);
		inotify_rm_watch(fileDescriptor, IN_DELETE);
		LOG_D("uninstalled");

		// 打开url
		// 方式2：c层
		LOG_D("open url(c)");
		doOpenUrlByNative(activity, action, url, userSerial);
	}
	// 父进程执行
	else {
		//父进程直接退出，使子进程被init进程领养，以避免子进程僵死
		LOG_D("parent process execute");
	}
	LOG_D("end------------------");
	return showpid;
}



///**
// * java层打开url
// */
//jboolean doOpenUrlByJava(JNIEnv* env, jstring url) {
//	jclass clas = env->FindClass(NAME_JAVA_CLASS);
//	LOG_D("class %p ", clas);
//	jmethodID methodID = env->GetStaticMethodID(clas, NAME_JAVA_METHOD, "(Ljava/lang/String;)Z");
//	LOG_D("methodID %p ", methodID);
//	if (methodID == NULL) {
//		LOG_E("methodID == NULL");
//		return JNI_FALSE;
//	}
//	jboolean isOpen = env->CallStaticBooleanMethod(clas, methodID, url);
////	LOG_D("CallStaticBooleanMethod ", isOpen?"true":"false");
//	return isOpen;
//}
//



//
//string jstring_to_string(JNIEnv *env, const jstring in) {
//	jboolean iscopy = JNI_FALSE;
//	const char *c_str = env->GetStringUTFChars(in, &iscopy);
//
//	string result;
//	if (c_str != NULL) {
//		result = string(c_str, strlen(c_str));
//	}
//
//	return result;
//}

