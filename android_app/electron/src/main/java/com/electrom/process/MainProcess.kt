package com.electrom.process

import android.util.Log
import com.electrom.ElectronApp
import com.electrom.extension.ELECTRON_ASSETS_FOLDER
import com.electrom.extension.LOG_TAG
import com.electrom.extension.appData
import com.electrom.extension.toObject
import java.util.*

class MainProcess(
    private val electronApp: ElectronApp,
    private val mainPath: String
) : ElectronProcess {

    override val processId: String = UUID.randomUUID().toString()

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

    override fun run() {
        Log.d(LOG_TAG, "Main process started in $processId")
        startMainModule(
            arrayOf(
                "${electronApp.context.appData}/${ELECTRON_ASSETS_FOLDER}",
                mainPath
            )
        )
    }
}