package com.electrom.node.view

import android.content.Context
import android.webkit.JavascriptInterface

class ElectronInterface(context: Context) {

    @JavascriptInterface
    fun require(modulePath: String) {

    }

}