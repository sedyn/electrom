package com.electrom.view

import android.content.Context
import android.util.Log
import android.webkit.JavascriptInterface

class ElectronInterface(context: Context) {

    @JavascriptInterface
    fun ipcRendererSend(channel: String, data: String?) {

    }

    @JavascriptInterface
    fun ipcRendererSendSync(channel: String, data: String?): String {
        Log.d("electron", "sendSync -> $channel $data")
        return "test"
    }

}