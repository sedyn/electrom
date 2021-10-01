package com.electrom.process

import java.util.*

abstract class ElectronProcess {

    val processId: String = UUID.randomUUID().toString()

}