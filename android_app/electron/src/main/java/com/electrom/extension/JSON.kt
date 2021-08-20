package com.electrom.extension

import com.fasterxml.jackson.databind.ObjectMapper
import com.fasterxml.jackson.module.kotlin.KotlinModule

object JSON {

    private val objectMapper = object : ThreadLocal<ObjectMapper>() {
        override fun initialValue(): ObjectMapper {
            return ObjectMapper().apply {
                registerModule(KotlinModule())
            }
        }
    }

    fun get(): ObjectMapper {
        return objectMapper.get()!!
    }

}

fun Any.toJSON(): String {
    return JSON.get().writeValueAsString(this)
}

inline fun <reified T> String.toObject(): T {
    return JSON.get().readValue(this, T::class.java)
}
