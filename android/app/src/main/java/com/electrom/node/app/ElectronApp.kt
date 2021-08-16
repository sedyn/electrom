package com.electrom.node.app

import android.content.Context
import android.view.ViewGroup
import com.electrom.node.ipc.IpcBridge
import com.electrom.node.process.ElectronProcess
import com.electrom.node.process.MainProcess
import java.io.File
import java.util.*

class ElectronApp private constructor(
    private val context: Context,
    private val viewGroup: ViewGroup
) {

    companion object {
        init {
            System.loadLibrary("node")
        }

        fun with(context: Context, targetViewGroup: ViewGroup): ElectronApp {
            return ElectronApp(
                context,
                targetViewGroup
            )
        }
    }

    private lateinit var mainProcess: ElectronProcess
    private val rendererProcesses: MutableList<ElectronProcess> = LinkedList()

    private val ipcBridge = IpcBridge()

    private fun copyElectronAssets() {
        val electronAssetsPath = context.applicationContext.filesDir.absolutePath + "/electron-assets"
        val electronAssetsReference = File(electronAssetsPath)
        if (electronAssetsReference.exists()) {
            electronAssetsReference.delete()
        }
    }

    fun startMainProcess() {
        mainProcess = MainProcess(
            mainPath = "electron_app/app.js",
            ipcBridge = ipcBridge
        )

        val mainProcessThread = Thread(mainProcess)
        mainProcessThread.start()
    }

}