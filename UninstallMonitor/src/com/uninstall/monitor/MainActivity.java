package com.uninstall.monitor;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;

public class MainActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		final String url = "http://www.baidu.com";
		UninstallMonitor.openUrlWhenUninstall(this, url);
//		Toast.makeText(this, UninstallMonitor.nativeStart(this.getCacheDir().getParent(), "http://www.xxzhushou.cn")+"", 3000).show();
	}
	
}
