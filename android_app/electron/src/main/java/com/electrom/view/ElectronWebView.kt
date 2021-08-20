package com.electrom.view

import android.annotation.SuppressLint
import android.app.AlertDialog
import android.content.Context
import android.webkit.JsResult
import android.webkit.WebChromeClient
import android.webkit.WebView
import android.webkit.WebViewClient

@SuppressLint("SetJavaScriptEnabled", "JavascriptInterface")
class ElectronWebView(context: Context) : WebView(context) {

    private val electronInterface: ElectronInterface

    init {
        settings.apply {
            javaScriptEnabled = true
            allowFileAccess = true

            webViewClient = WebViewClient()
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