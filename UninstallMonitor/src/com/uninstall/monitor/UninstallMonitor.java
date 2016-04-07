package com.uninstall.monitor;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.os.Build;
import android.text.TextUtils;
import android.util.Log;


public class UninstallMonitor {

    private static final String TAG = "Monitor";


	static {
//        System.loadLibrary("UninstallMonitor");
    }

    private static final String EXECUTE_FILE_NAME = "UninstallMonitor";
    /**
     * 在需要开启子进程监听的地方调用该方法
     * @param context
     * @param openUrl
     */
    public static void openUrlWhenUninstall(Context context, String openUrl) {
        if (checkChildProcess(context)) {
            Log.i(TAG, "已经开启进程");
            return;
        }
        // url去空格，防止exec出错
        openUrl = openUrl.replace(" ", "");
        Log.i(TAG, "启动新检测进程");
        String dataPackageDir = context.getApplicationInfo().dataDir;
//        String dirStr = context.getApplicationInfo().dataDir + "/lib";// 监听lib目录是因为：lib目录在进行应用清理的时候不会被清理
        String activity = null;
        if (checkInstall(context, "com.android.browser")) {
            activity = "com.android.browser/com.android.browser.BrowserActivity";
        }
        String action = "android.intent.action.VIEW";
        String gPid = "";
        if (Build.VERSION.SDK_INT < 17) {
//        	gPid = nativeStartAndOpenUrl(dataPackageDir, activity, action, openUrl, null);
        	doOpenUrlWhenUninstall(context, dataPackageDir, activity, action, openUrl, null);
        } else {//4.2以上的系统由于用户权限管理更严格，调用API有所区别
//        	gPid = nativeStartAndOpenUrl(dataPackageDir, activity, action, openUrl, getUserSerial(context));
        	doOpenUrlWhenUninstall(context, dataPackageDir, activity, action, openUrl, getUserSerial(context));
        }
//        gPid = nativeStart(dataPackageDir, openUrl);
    }
    
    public static void doOpenUrlWhenUninstall(final Context context, final String pkgDir, final String acti, final String action, final String url, final String userSerial){
    	new Thread(new Runnable() {
			
			@Override
			public void run() {
				File file = new File(getExcuteFilePath(context));
				if(!file.exists()){
					copyFromAssets(context, "UninstallMonitor", file.getAbsolutePath());
				}
				Log.i(TAG, "exist " + file.exists());
				try {
					file.setExecutable(true);
					file.setWritable(true);
					file.setReadable(true);
					StringBuilder sBuilder = new StringBuilder();
					sBuilder.append(file.getAbsolutePath() + " ");
					sBuilder.append(pkgDir + " ");
					sBuilder.append(acti + " ");
					sBuilder.append(action + " ");
					sBuilder.append(url + " ");
					sBuilder.append(userSerial + "");
					Log.i(TAG, "exec:"+sBuilder);
					Runtime.getRuntime().exec(sBuilder.toString());
				} catch (Exception e) {
					Log.e(TAG, ""+e);
					e.printStackTrace();
				}
			}
		}).start();
    }

    
    
    public static void copyFromAssets(Context context, String origFileName, String outpath) {
        try {
            File destFile = new File(outpath);
            File dir = destFile.getParentFile();
            if (!dir.exists()) {
                dir.mkdirs();
            }
            if (!destFile.exists()) {
                destFile.createNewFile();
            }

            InputStream myInput;
            OutputStream myOutput = new FileOutputStream(outpath);
            myInput = context.getAssets().open(origFileName);
            byte[] buffer = new byte[1024];
            int length = myInput.read(buffer);
            while (length > 0) {
                myOutput.write(buffer, 0, length);
                length = myInput.read(buffer);
            }
            myOutput.flush();
            myInput.close();
            myOutput.close();
        } catch (Exception e) {
        	Log.e(TAG, ""+e);
            e.printStackTrace();
        }
    }
    
    public static String getExcuteFilePath(Context context){
    	return context.getCacheDir().getParent() + File.separator + "files" + File.separator + EXECUTE_FILE_NAME;
    }
    
    /**
     * 检查安装
     * @param context
     * @param packageName
     * @return
     */
    private static boolean checkInstall(Context context, String packageName) {
        try {
            PackageInfo packageInfo = context.getPackageManager().getPackageInfo(packageName, 0);
            if (packageInfo == null) {
                return false;
            } else {
                return true;
            }
        } catch (Exception e) {
            return false;
        }
    }

    /**
     * 获取 UserSerial
     * @param ctx
     * @return
     */
    private static String getUserSerial(Context ctx) {
        Object userManager = ctx.getSystemService("user");
        if (userManager == null) {
            return null;
        }
        try {
            Method myUserHandleMethod = android.os.Process.class.getMethod("myUserHandle", (Class<?>[]) null);
            Object myUserHandle = myUserHandleMethod.invoke(android.os.Process.class, (Object[]) null);
            Method getSerialNumberForUser = userManager.getClass().getMethod("getSerialNumberForUser", myUserHandle.getClass());
            long userSerial = (Long) getSerialNumberForUser.invoke(userManager, myUserHandle);
            return String.valueOf(userSerial);
        } catch (NoSuchMethodException e) {
        } catch (IllegalArgumentException e) {
        } catch (IllegalAccessException e) {
        } catch (InvocationTargetException e) {
        }
        return null;
    }

    /**
     * 检查子进程
     * @param context
     * @param pid
     * @return
     */
    private static boolean checkChildProcess(Context context) {
        boolean resflag = false;
        int mypid = android.os.Process.myPid();
        Log.i(TAG, "my pid: " + mypid);
        BufferedReader in = null;
        List<Map<String, String>> listdata = new ArrayList<Map<String, String>>();
        boolean hasProcessName = false;
        try {
            Process p = Runtime.getRuntime().exec("ps");
            in = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String line = null;
            String[] temp;
            boolean flag = false;
//          int length = 0;
            while ((line = in.readLine()) != null) {
                if (!flag) {
                    flag = true;
                    continue;
                }
                line = line.replaceAll(" +", " ");
                temp = line.split(" ");
                hasProcessName = line.contains(getExcuteFilePath(context));
                // 过滤出父进程pid，进程名=包名的所有子进程id
                if (temp.length > 8 && temp[8].trim().equals(getExcuteFilePath(context)) && (temp[2].trim().equals(mypid + "") || temp[2].trim().equals("1"))) {
                    Log.i(TAG, "get it");
                    Map<String, String> map = new HashMap<String, String>();
                    map.put("pid", temp[1]);
                    map.put("pname", temp[8]);
                    listdata.add(map);
                }
            }
        } catch (IOException e) {
            resflag = hasProcessName;
            e.printStackTrace();
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                in = null;
            }
        }
        
        // 防止多个同名进程
        if(listdata.size() > 1){
        	for (int i = 0; i < listdata.size(); i++) {
        		Log.w(TAG, "kill unuse process" + listdata.get(i).get("pid") + " " + listdata.get(i).get("pname"));
                try {
                    android.os.Process.killProcess(Integer.valueOf(listdata.get(i).get("pid").trim()));
                } catch (NumberFormatException e) {
                    e.printStackTrace();
                }
            }
        }
        else if(listdata.size() == 1){
        	resflag = true;
        }
        
        return resflag;
    }
    

	
    
    
    private static native String nativeStartAndOpenUrl(String targetDir, String activity, String action, String actionData, String userSerial);
    
//    public native static String nativeStart();
    public native static String nativeStart(String pkgName, String url);
}
