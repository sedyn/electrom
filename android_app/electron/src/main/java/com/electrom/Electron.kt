package com.electrom

import android.app.Activity
import android.os.Handler
import android.os.Looper
import android.view.ViewGroup
import com.electrom.extension.*
import com.electrom.process.MainProcess


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

    }

    internal fun setTitle(title: String) {
        Handler(Looper.getMainLooper()).post {
            activity.title = title
        }
    }

    fun startMainProcess() {
        MainProcess(this, "app.js").execute()
    }

}