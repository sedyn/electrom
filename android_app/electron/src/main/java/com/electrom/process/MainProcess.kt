package com.electrom.process

import android.os.Handler
import android.os.Looper
import android.util.Log
import com.electrom.Electron
import com.electrom.extension.LOG_TAG
import com.electrom.extension.electronInternalScriptFolderPath
import com.electrom.extension.electronResourceFolderPath
import com.electrom.extension.toObject

internal class MainProcess(
    private val electron: Electron,
    private val mainStartupScript: String
) : ElectronProcess() {

    private val handler = Handler(Looper.getMainLooper())

    /**
     * Start mobile Node.js for main process in electron.
     * This function starts embedded Node.js (https://nodejs.org/api/embedding.html) environment.
     */
    private external fun startMainModule(arguments: Array<String>): Int

    private external fun uvRunOnce()

    private lateinit var webContents: WebContents

    private fun createWebContents(weakMapId: Int, properties: String) {
        webContents = WebContents(electron, properties.toObject(), weakMapId, this)
    }

    private external fun emitWebContentsEvent(webContentsId: Int, event: String)

    private external fun emitIpcMainSync(event: String, data: String): String

    internal fun sendSyncToIpcMain(event: String, data: String): String {
        return emitIpcMainSync(event, data)
    }

    private fun commandToWebContents(webContentsId: Int, command: String, arguments: String?) {
        Log.d(LOG_TAG, "CALL -> $command(${arguments ?: ""})")
        when (command) {
            "LoadURL" -> {
                handler.post {
                    webContents.loadUrl(arguments!!)
                    emitWebContentsEvent(webContents.weakMapId, "ready-to-show")
                }
            }
            "Show" -> {
                webContents.show()
            }
        }
    }

    /**
     * Run UvEvent in UIThread
     */
    private fun addTask() {
        handler.post {
            uvRunOnce()
        }
    }

    private fun startEmbeddedNodeJs() {
        electron.activity.run {
            startMainModule(
                arrayOf(
                    electronInternalScriptFolderPath,
                    electronResourceFolderPath,
                    mainStartupScript
                )
            )
        }
    }

    fun execute() {
        startEmbeddedNodeJs()
    }
}