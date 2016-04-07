/*
 * Monitor.h
 *
 *  Created on: 2016-3-15
 *      Author: white
 */

/**
 * 开始检测，java层打开url
 */
jstring doStart(JNIEnv* env, jclass clazz, jstring targetDir, jstring url);

/*
 * 开始检测，c层浏览器打开url
 */
//jstring doStartAndOpenUrl(JNIEnv* env, jclass clazz,jstring targetDir, jstring activity, jstring action, jstring url, jstring userSerial);
jstring doStartAndOpenUrl(char* targetDir, char* activity, char* action, char* url, char* userSerial);
