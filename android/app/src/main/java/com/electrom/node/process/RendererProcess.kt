package com.electrom.node.process

import java.util.*

class RendererProcess : ElectronProcess {

    override val processId: String = UUID.randomUUID().toString()

    override fun run() {
        // attach ElectronWebView
    }
}