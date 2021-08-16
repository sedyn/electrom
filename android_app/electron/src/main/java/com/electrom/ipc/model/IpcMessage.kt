package com.electrom.ipc.model

data class IpcMessage(
    val originId: String,
    val targetId: String,
    val data: String
)
