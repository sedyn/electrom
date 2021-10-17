package com.electrom

import android.app.Activity
import android.os.Handler
import android.os.Looper
import android.view.ViewGroup
import com.electrom.extension.*
import com.electrom.ipc.IpcBridge
import com.electrom.process.MainProcess
import com.electrom.process.WebContents
import com.electrom.process.data.BrowserWindowProperty
import java.util.concurrent.ConcurrentHashMap


class Electron private constructor(
    internal val activity: Activity,
    internal val rendererLayout: ViewGroup
) {

    companion object {
        init {
            System.loadLibrary("electron")
            System.loadLibrary("node")
        }

        fun with(activity: Activity, targetViewGroup: ViewGroup): Electron {
            activity.run {
                listOf(
                    Pair(ELECTRON_ASSETS_FOLDER, electronResourceFolderPath),
                    Pair(ELECTRON_INTERNAL_SCRIPT_FOLDER, electronInternalScriptFolderPath)
                ).forEach {
                    copyFolderFromAssetsToApplicationDirectory(it.first, it.second)
                }
            }
            return Electron(
                activity,
                targetViewGroup
            )
        }

        const val THREAD_POOL_SIZE = 2
    }

    private val webContentsMap: MutableMap<Int, WebContents> = ConcurrentHashMap()

    val ipcBridge = IpcBridge()

    internal fun setTitle(title: String) {
        Handler(Looper.getMainLooper()).post {
            activity.title = title
        }
    }

    internal fun requestRendererProcess(weakMapId: Int, browserWindowProperty: BrowserWindowProperty): WebContents {
        val webContents = WebContents(this, browserWindowProperty, weakMapId)
        webContentsMap[webContents.weakMapId] = webContents
        return webContents
    }

    fun startMainProcess() {
        MainProcess(this, "app.js").execute()
    }

}