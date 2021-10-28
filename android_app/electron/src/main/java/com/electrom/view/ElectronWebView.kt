package com.electrom.view

import android.annotation.SuppressLint
import android.app.AlertDialog
import android.webkit.JsResult
import android.webkit.WebChromeClient
import android.webkit.WebView
import android.webkit.WebViewClient
import com.electrom.Electron
import com.electrom.extension.ELECTRON_INTERNAL_SCRIPT_FOLDER

@SuppressLint("SetJavaScriptEnabled", "JavascriptInterface", "ViewConstructor")
internal class ElectronWebView(
    electron: Electron
) : WebView(electron.activity) {

    private val electronInterface: ElectronInterface

    private val preLoadScript = electron
        .activity
        .assets
        .open("${ELECTRON_INTERNAL_SCRIPT_FOLDER}/renderer-init.js")
        .use {
            val buffer = ByteArray(it.available())
            it.read(buffer)
            String(buffer, Charsets.UTF_8)
        }

    init {
        settings.apply {
            javaScriptEnabled = true
            allowFileAccess = true

            // Resolve CORS policy for http requests
            // https://chromium.googlesource.com/chromium/src/+/HEAD/android_webview/docs/cors-and-webview-api.md
            allowUniversalAccessFromFileURLs = true

            webViewClient = object : WebViewClient() {
                override fun onPageFinished(view: WebView, url: String?) {
                    electron.setTitle(view.title ?: "<electron>")
                    super.onPageFinished(view, url)
                }
            }
            webChromeClient = object : WebChromeClient() {
                override fun onJsAlert(
                    view: WebView?,
                    url: String?,
                    message: String?,
                    result: JsResult
                ): Boolean {
                    AlertDialog.Builder(context).apply {
                        setTitle("Alert")
                        setMessage(message)
                        setPositiveButton("Yes") { _, _ ->
                            result.confirm()
                        }
                    }.setCancelable(false).create().show()
                    return true
                }
            }

            setWebContentsDebuggingEnabled(true)
        }
        electronInterface = ElectronInterface(electron, preLoadScript)
        addJavascriptInterface(electronInterface, "@@android")
    }

    fun resolveAsyncMessage(channel: String, data: String) {
        evaluateJavascript(
            "window['@@electron'].ipcRenderer.emit('$channel', '$data');",
            null
        )
    }
}