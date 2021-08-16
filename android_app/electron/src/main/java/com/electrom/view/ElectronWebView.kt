package com.electrom.view

import android.annotation.SuppressLint
import android.content.Context
import android.webkit.WebView

@SuppressLint("SetJavaScriptEnabled", "JavascriptInterface")
class ElectronWebView(context: Context) : WebView(context) {

    private val electronInterface: ElectronInterface

    init {
        settings.apply {
            javaScriptEnabled = true
        }
        electronInterface = ElectronInterface(context)
        addJavascriptInterface(electronInterface, "@@electron")
    }
}