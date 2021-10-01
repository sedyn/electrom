package com.electrom.process

import android.util.Log
import com.electrom.ElectronApp
import com.electrom.extension.ELECTRON_ASSETS_FOLDER
import com.electrom.extension.LOG_TAG
import com.electrom.extension.toObject

internal class MainProcess(
    private val electronApp: ElectronApp,
    private val mainPath: String
) : ElectronProcess() {

    /**
     * Start mobile Node.js for main process in electron.
     * This function starts embedded Node.js (https://nodejs.org/api/embedding.html) environment.
     */
    private external fun startMainModule(arguments: Array<String>): Int

    private lateinit var peerRendererProcess: RendererProcess

    private fun startRendererProcess(properties: String): String {
        Log.d(LOG_TAG, "Renderer process started by $processId")
        peerRendererProcess = electronApp.requestRendererProcess(properties.toObject())
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

    private fun startEmbeddedNodeJs() {
        startMainModule(
            arrayOf(
                "${electronApp.appData}/$ELECTRON_ASSETS_FOLDER",
                mainPath
            )
        )
    }

    fun execute() {
        startEmbeddedNodeJs()
    }
}