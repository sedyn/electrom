package com.electrom.process

import android.graphics.Color
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.view.View
import android.view.ViewGroup
import com.electrom.Electron
import com.electrom.extension.LOG_TAG
import com.electrom.process.data.BrowserWindowProperty
import com.electrom.view.ElectronWebView
import java.util.concurrent.CountDownLatch

class RendererProcess(
    private val electron: Electron,
    private val browserWindowProperties: BrowserWindowProperty
) : ElectronProcess(), Runnable {

    private lateinit var webView: ElectronWebView

    init {
        attachWebViewOnStart()
    }

    private inline fun awaitMainLooper(crossinline block: () -> Unit) {
        val wg = CountDownLatch(1)
        Handler(Looper.getMainLooper()).post {
            block()
            wg.countDown()
        }
        wg.await()
    }

    private fun attachWebViewOnStart() {
        awaitMainLooper {
            webView = ElectronWebView(electron).apply {
                layoutParams = ViewGroup.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.MATCH_PARENT
                )
            }

            browserWindowProperties.run {
                backgroundColor?.also {
                    webView.setBackgroundColor(Color.parseColor(it))
                }

                show?.also {
                    if (!show) {
                        webView.visibility = View.INVISIBLE
                    }
                }

                electron.rendererLayout.addView(webView)
            }
        }
        Log.d(LOG_TAG, "after attach")
    }

    internal fun loadUrl(url: String) {
        awaitMainLooper {
            webView.loadUrl(url)
        }
    }

    internal fun show() {
        awaitMainLooper {
            webView.visibility = View.VISIBLE
        }
    }

    override fun run() {
        // TODO Add Looper for IPC?
    }
}