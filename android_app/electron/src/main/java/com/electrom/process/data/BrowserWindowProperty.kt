package com.electrom.process.data

import com.fasterxml.jackson.annotation.JsonIgnoreProperties

@JsonIgnoreProperties(ignoreUnknown = true)
data class BrowserWindowProperty(
    val backgroundColor: String?,
    val show: Boolean?
)
