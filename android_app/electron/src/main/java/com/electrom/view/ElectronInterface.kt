package com.electrom.view

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.webkit.JavascriptInterface
import com.electrom.process.MainProcess
import java.util.*
import java.util.concurrent.CountDownLatch

internal class ElectronInterface(
    context: Context,
    private val mainProcess: MainProcess
) {

    private val handler = Handler(Looper.getMainLooper())

    @JavascriptInterface
    fun ipcRendererSend(channel: String, data: String?): String {
        val trackId = UUID.randomUUID().toString()
        return trackId
    }

    @JavascriptInterface
    fun ipcRendererSendSync(channel: String, data: String?): String {
        Log.d("electron", "sendSync -> $channel $data")
        val locker = CountDownLatch(1)
        var result: String? = null
        handler.post {
            result = mainProcess.sendSyncToIpcMain(channel, data ?: "!!")
            locker.countDown()
        }
        locker.await()
        return result ?: "FAILED"
    }

}