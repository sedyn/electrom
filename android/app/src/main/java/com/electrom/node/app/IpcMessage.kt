package com.electrom.node.app

data class IpcMessage(
    val originId: String,
    val targetId: String,
    val data: String
)
