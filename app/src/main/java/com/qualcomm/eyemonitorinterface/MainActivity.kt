package com.qualcomm.eyemonitorinterface

import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat

class MainActivity : AppCompatActivity() {
    companion object {
        init {
            System.loadLibrary("ufseom")
        }
    }

    private external fun runUfseom(args: Array<String>): Int

    private lateinit var submitBttn: Button
    private lateinit var ufsResult: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main)) { v, insets ->
            val systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars())
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom)
            insets
        }

        submitBttn = findViewById(R.id.submitButton)
        ufsResult = findViewById(R.id.ufsResult)
        val args = arrayOf("./ufs_eom","-h")

        submitBttn.setOnClickListener{
            val result: Int = runUfseom(args)
            Toast.makeText(this, result.toString(), Toast.LENGTH_SHORT).show()
        }
    }
}