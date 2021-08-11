package com.electrom.node.process

import com.electrom.node.app.ElectronApp
import com.electrom.node.app.IpcMessage
import java.util.*

class MainProcess(private val mainPath: String) : ElectronProcess {

    override val processId: String = UUID.randomUUID().toString()

    private external fun startMainModule(arguments: Array<String>): Int

    private fun sendTo(sourceId: String, data: String) {
        ElectronApp.mainSendTo(
            IpcMessage(
                originId = processId,
                targetId = sourceId,
                data = data
            )
        )
    }

    override fun run() {
        startMainModule(arrayOf(mainPath))
    }
}