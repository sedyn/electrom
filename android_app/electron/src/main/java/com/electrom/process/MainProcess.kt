package com.electrom.process

import android.util.Log
import com.electrom.ipc.IpcBridge
import com.electrom.ipc.model.IpcMessage
import java.util.*

class MainProcess(
    private val mainPath: String,
    private val ipcBridge: IpcBridge
) : ElectronProcess {

    init {

    }

    override val processId: String = UUID.randomUUID().toString()

    private external fun startMainModule(arguments: Array<String>): Int

    private fun sendTo(sourceId: String, data: String) {
        ipcBridge.mainSendTo(
            IpcMessage(
                originId = processId,
                targetId = sourceId,
                data = data
            )
        )
    }

    private fun startRendererProcess() {
        Log.d("electron", "test")
    }

    override fun run() {
        startMainModule(arrayOf(mainPath))
    }
}