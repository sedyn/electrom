package com.electrom

import android.content.Context
import android.view.ViewGroup
import com.electrom.extension.copyElectronAssetFolder
import com.electrom.ipc.IpcBridge
import com.electrom.process.ElectronProcess
import com.electrom.process.MainProcess
import java.util.*


class ElectronApp private constructor(
    val context: Context,
    val viewGroup: ViewGroup
) {

    companion object {
        init {
            System.loadLibrary("electron")
            System.loadLibrary("node")
        }

        fun with(context: Context, targetViewGroup: ViewGroup): ElectronApp {
            context.copyElectronAssetFolder()
            return ElectronApp(
                context,
                targetViewGroup
            )
        }
    }

    private lateinit var mainProcess: ElectronProcess
    private val rendererProcesses: MutableList<ElectronProcess> = LinkedList()

    val ipcBridge = IpcBridge()

    fun startMainProcess() {
        mainProcess = MainProcess(this, "electron_app/app.js")

        val mainProcessThread = Thread(mainProcess)
        mainProcessThread.start()
    }

}