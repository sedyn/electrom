package com.electrom.extension

import android.content.Context
import android.content.res.AssetManager
import java.io.File

val Context.appData: String
    get() = this.filesDir.absolutePath

val Context.electronResourceFolderPath: String
    get() = "$appData/$ELECTRON_ASSETS_FOLDER"

val Context.electronInternalScriptFolderPath: String
    get() = "$appData/$ELECTRON_INTERNAL_SCRIPT_FOLDER"

private fun copyAsset(assetManager: AssetManager, fromAssetPath: String, toPath: String) {
    assetManager.open(fromAssetPath).use { stream ->
        File(toPath).outputStream().use {
            stream.copyTo(it)
        }
    }
}

fun Context.copyFolderFromAssetsToApplicationDirectory(assetPath: String, targetPath: String) {
    val electronAssetsReference = File(targetPath)
    if (electronAssetsReference.exists()) {
        electronAssetsReference.deleteRecursively()
    }

    electronAssetsReference.mkdir()

    assets.list(assetPath)!!.forEach { file ->
        copyAsset(
            assets,
            "$assetPath/$file",
            "$targetPath/$file"
        )
    }
}