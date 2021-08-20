package com.electrom.android_app

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.constraintlayout.widget.ConstraintLayout
import com.electrom.ElectronApp

class MainActivity : AppCompatActivity() {

    private lateinit var electronApp: ElectronApp

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        electronApp = ElectronApp.with(
            this, findViewById<ConstraintLayout>(R.id.container)
        )
    }

    override fun onStart() {
        super.onStart()
        electronApp.startMainProcess()
    }
}
