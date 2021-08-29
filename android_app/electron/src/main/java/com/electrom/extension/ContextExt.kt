package com.electrom.extension

import android.content.Context
import android.content.res.AssetManager
import java.io.File

val Context.appData: String
    get() = this.filesDir.absolutePath

const val ELECTRON_ASSETS_FOLDER = "electron_app"

private fun copyAsset(assetManager: AssetManager, fromAssetPath: String, toPath: String) {
    assetManager.open(fromAssetPath).use { stream ->
        File(toPath).outputStream().use {
            stream.copyTo(it)
        }
    }
}

fun Context.copyElectronAssetFolder() {
    val electronAssetsPath = "$appData/$ELECTRON_ASSETS_FOLDER"
    val electronAssetsReference = File(electronAssetsPath)
    if (electronAssetsReference.exists()) {
        electronAssetsReference.deleteRecursively()
    }

    electronAssetsReference.mkdir()

    val assetManager = assets

    val files = assetManager.list(ELECTRON_ASSETS_FOLDER)!!
    for (file in files) {
        copyAsset(assetManager, "$ELECTRON_ASSETS_FOLDER/$file", "$electronAssetsPath/$file")
    }
}