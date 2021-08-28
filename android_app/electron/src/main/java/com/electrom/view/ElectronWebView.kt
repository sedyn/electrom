package com.electrom.view

import android.annotation.SuppressLint
import android.app.AlertDialog
import android.content.Context
import android.webkit.*
import com.electrom.ElectronApp

@SuppressLint("SetJavaScriptEnabled", "JavascriptInterface", "ViewConstructor")
class ElectronWebView(electronApp: ElectronApp) : WebView(electronApp.context) {

    private val electronInterface: ElectronInterface

    init {
        settings.apply {
            javaScriptEnabled = true
            allowFileAccess = true

            // Resolve CORS policy for http requests
            // https://chromium.googlesource.com/chromium/src/+/HEAD/android_webview/docs/cors-and-webview-api.md
            allowUniversalAccessFromFileURLs = true

            webViewClient = object : WebViewClient() {
                override fun onPageFinished(view: WebView, url: String) {
                    electronApp.setTitle(view.title ?: "<electron>")
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
        electronInterface = ElectronInterface(context)
        addJavascriptInterface(electronInterface, "@@electron")
    }
}