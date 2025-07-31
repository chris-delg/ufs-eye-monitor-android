package com.qualcomm.eyemonitorinterface

import android.os.Bundle
import android.util.Log
import android.widget.ArrayAdapter
import android.widget.Button
import android.widget.CheckBox
import android.widget.EditText
import android.widget.Spinner
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import java.nio.file.Files
import java.nio.file.Paths

class MainActivity : AppCompatActivity() {
    companion object {
        init {
            System.loadLibrary("ufseom")
        }
    }

    private external fun runUfseom(args: Array<String>): Int

    private lateinit var submitBttn: Button
    private lateinit var peerLocalSpinner: Spinner
    private lateinit var randomDataCheckbox: CheckBox
    private lateinit var laneNumberSpinner: Spinner
    private lateinit var voltageInput: EditText
    private lateinit var targetInput: EditText
    private lateinit var devicePathInput: EditText

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
        peerLocalSpinner = findViewById(R.id.peerLocalSpinner)
        randomDataCheckbox = findViewById(R.id.useRandomDataCheckbox)
        laneNumberSpinner = findViewById(R.id.laneNumberSpinner)
        voltageInput = findViewById(R.id.voltageInput)
        targetInput = findViewById(R.id.targetInput)
        devicePathInput = findViewById(R.id.devicePathInput)

        setUpSpinners()

        val args = mutableListOf("./ufs_eom")
        submitBttn.setOnClickListener {
            if (!validateInputs()) {
                return@setOnClickListener
            }

            args.add("--${peerLocalSpinner.selectedItem}")

            if (randomDataCheckbox.isChecked) {
                args.add("--data")
            }

            if (laneNumberSpinner.selectedItem != "N/A") {
                args.addAll(listOf("--lane", "${laneNumberSpinner.selectedItem}"))
            }

            if (voltageInput.text.toString().isNotBlank()) {
                args.addAll(listOf("--voltage", "${voltageInput.text}"))
            }

            if (targetInput.text.toString().isNotBlank()) {
                args.addAll(listOf("--target", "${targetInput.text}"))
            }

            args.addAll(listOf("--output", this.getExternalFilesDir(null).toString()))

            args.addAll(listOf("--device", "${devicePathInput.text}"))

            Log.d("TEST", args.toString())
            args.retainAll(setOf(args.first()))
            //TODO: send args array to loading activity which will then call runUfseom
            //val result: Int = runUfseom(args.toTypedArray())
            //Toast.makeText(this, result.toString(), Toast.LENGTH_SHORT).show()
        }
    }

    private fun setUpSpinners() {
        val peerLocalAdapter = ArrayAdapter.createFromResource(
            this,
            R.array.peer_local_options,
            android.R.layout.simple_spinner_item
        )
        peerLocalAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)

        val laneNumberAdapter = ArrayAdapter.createFromResource(
            this,
            R.array.lane_number_options,
            android.R.layout.simple_spinner_item
        )
        laneNumberAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item)

        // adding adapters to spinners
        peerLocalSpinner.adapter = peerLocalAdapter
        laneNumberSpinner.adapter = laneNumberAdapter
    }

    private fun validateInputs(): Boolean {
        if (devicePathInput.text.toString().isBlank()) {
            Toast.makeText(this, "Path cannot be empty", Toast.LENGTH_SHORT).show()
            return false
        }

        val path = Paths.get(devicePathInput.text.toString())
        if (!Files.exists(path)) {
            Toast.makeText(this, "Path does not exist", Toast.LENGTH_SHORT).show()
            return false
        }

        return true
    }
}
