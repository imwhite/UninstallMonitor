package com.uninstall.monitor;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.text.TextUtils;
import android.util.Log;

import com.uninstall.monitor.utils.ContextFinder;

public class JNIHelper {

	/**
	 * 浏览器打开url
	 * @description  
	 * @param context
	 * @param url
	 */
	public static boolean openUrl(String url){
		Log.i("Monitor", "openUrl " + url);
		if(TextUtils.isEmpty(url)){
			return false;
		}
		Context context = ContextFinder.getApplication();
		
		try {
			Intent intent = new Intent();
			intent.setAction("android.intent.action.VIEW");
			Uri CONTENT_URI_BROWSERS = Uri.parse(url);
			intent.setData(CONTENT_URI_BROWSERS);
			intent.setClassName("com.android.browser", "com.android.browser.BrowserActivity");
			intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
			context.startActivity(intent);
		} catch (Exception er) {
			Log.e("Monitor", er+"");
			try {
				Intent it = new Intent(Intent.ACTION_VIEW, Uri.parse(url));
				it.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
				context.startActivity(it);
			} catch (Exception err) {
                return false;
			}
		}
        return true;
	}
}
