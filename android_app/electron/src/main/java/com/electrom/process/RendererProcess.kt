package com.electrom.process

import android.graphics.Color
import android.os.Handler
import android.os.Looper
import android.view.ViewGroup
import com.electrom.ElectronApp
import com.electrom.view.ElectronWebView
import java.util.*

class RendererProcess(
    private val electronApp: ElectronApp,
    private val browserWindowProperties: Map<String, Any>
) : ElectronProcess {

    override val processId: String = UUID.randomUUID().toString()

    private fun attachWebViewOnStart() {
        Handler(Looper.getMainLooper()).post {
            val electronWebView = ElectronWebView(electronApp.context).apply {
                layoutParams = ViewGroup.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT,
                    ViewGroup.LayoutParams.MATCH_PARENT
                )
            }
            electronApp.viewGroup.addView(electronWebView)

            browserWindowProperties["backgroundColor"]?.also {
                electronWebView.setBackgroundColor(Color.parseColor(it as String))
            }

            electronWebView.loadData("<html><body>Hello!</body></html>", "text/html", "utf8")
        }
    }

    override fun run() {
        attachWebViewOnStart()
    }
}