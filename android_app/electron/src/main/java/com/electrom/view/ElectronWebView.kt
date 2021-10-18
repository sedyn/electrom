package com.electrom.view

import android.annotation.SuppressLint
import android.app.AlertDialog
import android.graphics.Bitmap
import android.webkit.JsResult
import android.webkit.WebChromeClient
import android.webkit.WebView
import android.webkit.WebViewClient
import com.electrom.Electron
import com.electrom.process.MainProcess

@SuppressLint("SetJavaScriptEnabled", "JavascriptInterface", "ViewConstructor")
internal class ElectronWebView(
    electron: Electron,
    mainProcess: MainProcess
) : WebView(electron.activity) {

    private val electronInterface: ElectronInterface

    companion object {
        private const val PRE_LOAD_SCRIPT =
            """
            (function(window) {
                const ipcRenderer = new Function();
                
                ipcRenderer.ipcTracker = new Map();
                ipcRenderer.handler = new Map();
                
                ipcRenderer.sendSync = function(channel, data) {
                    return window['@@android'].ipcRendererSendSync(channel, data);
                };
                
                ipcRenderer.on = function(channel, cb) {
                    if (!this.handler.has(channel)) {
                        this.handler.add(channel, cb);
                    }
                };
                
                ipcRenderer.emit = function(channel, event) {
                    const cb = this.handler.get(channel);
                    if (cb !== undefined) {
                        cb(event);
                    }
                };
                
                ipcRenderer.send = function(channel, data) {
                    const trackId = window['@@android'].ipcRendererSend(channel, data);
                    return new Promise((resolve, reject) => {
                        this.ipcTracker[trackId] = {
                            resolve, reject, trackId
                        }
                    });
                };
                
                ipcRenderer.__resolve = function(trackId, success, data) {
                    const promise = this.ipcTracker[trackId];
                    if (promise === undefined) return;
                    if (success) {
                        promise.resolve(data);
                    } else {
                        promise.reject();
                    }
                };
                
                window['@@electron'] = {
                    ipcRenderer
                };

                window.require = function(path) {
                    if (path === 'electron') {
                        return window['@@electron'];
                    } else {
                        throw new Error('Module not found ' + path)
                    }
                };
                
                window.module = {};
            })(window);
            """
    }

    init {
        settings.apply {
            javaScriptEnabled = true
            allowFileAccess = true

            // Resolve CORS policy for http requests
            // https://chromium.googlesource.com/chromium/src/+/HEAD/android_webview/docs/cors-and-webview-api.md
            allowUniversalAccessFromFileURLs = true

            webViewClient = object : WebViewClient() {
                override fun onPageFinished(view: WebView, url: String) {
                    electron.setTitle(view.title ?: "<electron>")
                }

                override fun onLoadResource(view: WebView?, url: String?) {
                    evaluateJavascript(PRE_LOAD_SCRIPT, null)
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
        electronInterface = ElectronInterface(context, mainProcess)
        addJavascriptInterface(electronInterface, "@@android")
    }

    fun resolveAsyncMessage(trackId: String, isSuccess: Boolean, data: String?) {
        evaluateJavascript(
            "window['@@electron'].ipcRenderer.__resolve('$trackId', $isSuccess, '$data');",
            null
        )
    }
}