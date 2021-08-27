package com.electrom

import android.content.Context
import android.view.ViewGroup
import com.electrom.extension.copyElectronAssetFolder
import com.electrom.ipc.IpcBridge
import com.electrom.process.ElectronProcess
import com.electrom.process.MainProcess
import com.electrom.process.RendererProcess
import com.electrom.process.data.BrowserWindowProperty
import java.util.*
import java.util.concurrent.SynchronousQueue
import java.util.concurrent.ThreadPoolExecutor
import java.util.concurrent.TimeUnit


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
            // TODO: copy assets files only when changed
            context.copyElectronAssetFolder()
            return ElectronApp(
                context,
                targetViewGroup
            )
        }

        const val THREAD_POOL_SIZE = 2
    }

    external fun emit(arguments: Array<String>): Int

    private val threadPoolExecutor = ThreadPoolExecutor(
        THREAD_POOL_SIZE,
        THREAD_POOL_SIZE,
        Long.MAX_VALUE,
        TimeUnit.SECONDS,
        SynchronousQueue()
    )
    private lateinit var mainProcess: ElectronProcess
    private val rendererProcesses: MutableList<ElectronProcess> =
        Collections.synchronizedList(LinkedList())

    val ipcBridge = IpcBridge()

    internal fun requestRendererProcess(browserWindowProperty: BrowserWindowProperty) {
        if (rendererProcesses.size > 1) {
            TODO("throw max renderer process exception")
        }

        val rendererProcess = RendererProcess(this, browserWindowProperty)
        rendererProcesses.add(rendererProcess)
        threadPoolExecutor.execute(rendererProcess)
    }

    fun startMainProcess() {
        mainProcess = MainProcess(this, "electron_app/app.js")
        threadPoolExecutor.execute(mainProcess)
    }

}