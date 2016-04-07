/*
 * jni_main.cc
 *
 *  Created on: 2016-3-18
 *      Author: white
 */

#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include <Monitor.h>


/* LOG宏定义 */
#define LOG_TAG "Monitor"
#define LOG_D(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)
#define LOG_E(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args)

/**
 * 动态注册
 */
#define NELEM(x) ((int) (sizeof(x) / sizeof((x)[0])))

/** begin of register methods  **/
void registerNativeMethods(JNIEnv *env, const char *claxxname, JNINativeMethod *methods, int nmethods){
	jclass claxx = env->FindClass(claxxname);

	if(claxx == NULL){
		LOG_E("Coun't find class %s.", claxxname);
		// TODO why
		return;
	}

	env->RegisterNatives(claxx, methods, nmethods);

	// TODO why
//	delete_ref(env, claxx);
	claxx = NULL;
}

jstring Monitor_start (JNIEnv *env, jclass clazz, jstring targetDir, jstring activity, jstring action, jstring url, jstring userSerial);
inline void registerMethods(JNIEnv *env){

	JNINativeMethod methods[] = {
//			{"nativeStart", "()Ljava/lang/String;", (void *)Monitor_start},
			{"nativeStartAndOpenUrl", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", (void *)Monitor_start},
	};
	registerNativeMethods(env, "com/uninstall/monitor/UninstallMonitor", methods, NELEM(methods));
}

/** end of register methods **/
extern "C" {

jint JNI_OnLoad(JavaVM *vm, void *reserved){
	JNIEnv* env = NULL;
	jint result = -1;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
		return result;
	}

	registerMethods(env);

	return JNI_VERSION_1_4;
}

};


/**
 * 开始执行检测入口
 */
jstring Monitor_start (JNIEnv *env, jclass clazz, jstring targetDir, jstring activity, jstring action, jstring url, jstring userSerial){
//	return doStart(env, clazz, targetDir, url);
//	return doStartAndOpenUrl(env, clazz, targetDir, activity, action, url, userSerial);
	return env->NewStringUTF("invoke success~");
}



/**
 * 可执行入口
 */
int main(int argc, char *args[]){
	LOG_D("main argc:%d args_c:", argc, sizeof(args));
	LOG_D("args : %s %s %s %s %s", args[0], args[1], args[2], args[3], args[4], args[5]);
	if(argc < 6){
		LOG_D("args params size less than five !!!");
		return 0;
	}
	doStartAndOpenUrl(args[1], args[2], args[3], args[4], args[5]);
	return 0;
}



