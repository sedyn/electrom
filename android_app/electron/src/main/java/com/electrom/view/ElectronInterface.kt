package com.electrom.view

import android.os.Handler
import android.os.Looper
import android.util.Log
import android.webkit.JavascriptInterface
import com.electrom.Electron
import java.util.*
import java.util.concurrent.CountDownLatch

internal class ElectronInterface(
    private val electron: Electron
) {

    companion object {
        const val JAVASCRIPT_UNDEFINED = "undefined"
    }

    private fun checkUndefined(v: String?): String? {
        return if (v == JAVASCRIPT_UNDEFINED || v == null) {
            null
        } else {
            v
        }
    }

    private val handler = Handler(Looper.getMainLooper())

    @JavascriptInterface
    fun ipcRendererSend(channel: String, data: String?): String {
        val trackId = UUID.randomUUID().toString()
        handler.post {
            electron.sendAsyncToIpcMain(trackId, channel, checkUndefined(data))
        }

        return trackId
    }

    @JavascriptInterface
    fun ipcRendererSendSync(channel: String, data: String?): String {
        Log.d("electron", "sendSync -> $channel $data")
        val locker = CountDownLatch(1)
        var result: String? = null
        handler.post {
            result = electron.sendSyncToIpcMain(channel, checkUndefined(data))
            locker.countDown()
        }
        locker.await()
        return result ?: "FAILED"
    }

}