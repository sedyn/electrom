package com.electrom.node.ipc

import com.electrom.node.ipc.model.IpcMessage
import java.util.*
import java.util.concurrent.ConcurrentLinkedQueue

class IpcBridge {

    private val ipcQueue: Queue<IpcMessage> = ConcurrentLinkedQueue()

    fun mainSendTo(ipcMessage: IpcMessage) {
        ipcQueue.add(ipcMessage)
    }

    fun rendererSendTo(ipcMessage: IpcMessage) {
        ipcQueue.add(ipcMessage)
    }

}