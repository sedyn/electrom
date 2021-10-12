package com.electrom.process

import android.os.Handler
import android.os.Looper
import android.util.Log
import com.electrom.Electron
import com.electrom.extension.LOG_TAG
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

    private lateinit var peerRendererProcess: RendererProcess

    private fun startRendererProcess(properties: String): String {
        Log.d(LOG_TAG, "Renderer process started by $processId")
        peerRendererProcess = electron.requestRendererProcess(properties.toObject())
        return peerRendererProcess.processId
    }

    private fun commandToRendererProcess(command: String, arguments: String?) {
        Log.d(LOG_TAG, "CALL -> $command(${arguments ?: ""})")
        when (command) {
            "loadURL" -> {
                peerRendererProcess.loadUrl(arguments!!)
            }
            "show" -> {
                peerRendererProcess.show()
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
                    "electron/js2c/browser_init",
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