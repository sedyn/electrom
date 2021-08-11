package com.electrom.node.app

import com.electrom.node.process.ElectronProcess
import com.electrom.node.process.MainProcess
import java.util.*
import java.util.concurrent.ConcurrentLinkedQueue

object ElectronApp {

    private val ipcQueue: Queue<IpcMessage> = ConcurrentLinkedQueue()

    private val mainProcess: ElectronProcess
    private val rendererProcesses: List<ElectronProcess> = listOf()

    init {
        System.loadLibrary("node")

        mainProcess = MainProcess("electron_app/app.js")
    }

    fun mainSendTo(ipcMessage: IpcMessage) {
        ipcQueue.add(ipcMessage)
    }

}