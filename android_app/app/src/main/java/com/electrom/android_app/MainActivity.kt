package com.electrom.android_app

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.constraintlayout.widget.ConstraintLayout
import com.electrom.Electron

class MainActivity : AppCompatActivity() {

    private lateinit var electron: Electron

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        electron = Electron.with(
            this, findViewById<ConstraintLayout>(R.id.container)
        )
    }

    override fun onStart() {
        super.onStart()
        electron.startMainProcess()
    }
}
