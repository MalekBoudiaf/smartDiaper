/*
Copyright (c) 2016, Cypress Semiconductor Corporation
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



For more information on Cypress BLE products visit:
http://www.cypress.com/products/bluetooth-low-energy-ble
 */

package com.vub.smartdiaper;

import androidx.appcompat.app.AppCompatActivity;
import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.content.res.ColorStateList;
import android.graphics.Color;
import android.graphics.drawable.ColorDrawable;
import android.media.MediaPlayer;
import android.os.Build;
import android.os.IBinder;
import androidx.annotation.NonNull;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.vub.smartdiaper.ble.R;

public class MainActivity extends AppCompatActivity {


    // TAG is used for informational messages
    private final static String TAG = MainActivity.class.getSimpleName();

    // Variables to access objects from the layout such as buttons, switches, values
    private static TextView mHumidityTV;
    private static TextView mTemperatureTV;
    private static Button disconnect_button;
    private ImageView ble_iv;
    public Spinner powerSpinner;
    public MediaPlayer alert;


    // Variables to manage BLE connection
    private static boolean deviceConnected;
    private static boolean deviceFound;
    private static BleService mSmartDiaperBleService;

    private static final int REQUEST_ENABLE_BLE = 1;

    //This is required for Android 6.0 (Marshmallow)
    private static final int PERMISSION_REQUEST_FINE_LOCATION = 1;

    Dialog diaperDialog;



    /**
     * This manages the lifecycle of the BLE service.
     * When the service starts we get the service object and initialize the service.
     */
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        /**
         * This is called when the BleService is connected
         *
         * @param componentName the component name of the service that has been connected
         * @param service service being bound
         */
        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            Log.i(TAG, "onServiceConnected");
            mSmartDiaperBleService = ((BleService.LocalBinder) service).getService();
            mSmartDiaperBleService.initialize();
            //mPSoCCapSenseLedService.scan();
        }

        /**
         * This is called when the PSoCCapSenseService is disconnected.
         *
         * @param componentName the component name of the service that has been connected
         */
        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            Log.i(TAG, "onServiceDisconnected");
            mSmartDiaperBleService = null;
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE)) {
            Toast.makeText(this, R.string.ble_not_supported, Toast.LENGTH_SHORT).show();
            finish();
        }


        // Set up a variable to point to the CapSense value on the display
        mHumidityTV = (TextView) findViewById(R.id.humidity_value);
        mTemperatureTV = (TextView) findViewById(R.id.temperature_value);
        disconnect_button = (Button) findViewById(R.id.disconnect_button);
        ble_iv = findViewById(R.id.BLE_IV);
        powerSpinner = findViewById(R.id.powerSpinner);
        powerSpinner.setEnabled(false);
        diaperDialog=new Dialog(this);


        // Initialize service and connection state variable
        deviceConnected = false;
        deviceFound =false;





        ArrayAdapter<CharSequence> spinnerAdapter= ArrayAdapter.createFromResource(this,R.array.powerSpinner,android.R.layout.simple_spinner_item);
        spinnerAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        powerSpinner.setAdapter(spinnerAdapter);
        powerSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                String powerModeSelected = adapterView.getItemAtPosition(i).toString();
                if(deviceConnected) {
                    Toast.makeText(adapterView.getContext(), powerModeSelected, Toast.LENGTH_SHORT).show();
                    // code block
                    int powerMode=0;
                    if (powerModeSelected.equals("Performance")) {
                        powerMode=0;
                    } else if (powerModeSelected.equals("Balanced")) {
                        powerMode=1;
                    } else {
                        powerMode=2;
                    }
                    mSmartDiaperBleService.writePowerModeCharacterristic(powerMode);
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });


        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // Android M Permission check 
            if (this.checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.ACCESS_FINE_LOCATION}, PERMISSION_REQUEST_FINE_LOCATION);
            }

            if (this.checkSelfPermission(Manifest.permission.BLUETOOTH) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.BLUETOOTH}, PERMISSION_REQUEST_FINE_LOCATION);
            }
        }



    }

    //This method required for Android 6.0 and above
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String permissions[], @NonNull int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUEST_FINE_LOCATION: {
                if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    Log.d("Permission for 6.0:", "Coarse location permission granted");
                } else {
                    final AlertDialog.Builder builder = new AlertDialog.Builder(this);
                    builder.setTitle("Functionality limited");
                    builder.setMessage("Since location access has not been granted, this app will not be able to discover beacons when in the background.");
                    builder.setPositiveButton(android.R.string.ok, null);
                    builder.setOnDismissListener(new DialogInterface.OnDismissListener() {
                        @Override
                        public void onDismiss(DialogInterface dialog) {
                        }
                    });
                    builder.show();
                }
            }
        }
    } //End of section for Android 6.0 (Marshmallow)

    @Override
    protected void onResume() {
        super.onResume();
        // Register the broadcast receiver. This specified the messages the main activity looks for from the BleService
        final IntentFilter filter = new IntentFilter();
        filter.addAction(BleService.ACTION_BLESCAN_CALLBACK);
        filter.addAction(BleService.ACTION_CONNECTED);
        filter.addAction(BleService.ACTION_DISCONNECTED);
        filter.addAction(BleService.ACTION_SERVICES_DISCOVERED);
        filter.addAction(BleService.ACTION_DATA_RECEIVED);
        registerReceiver(mBleUpdateReceiver, filter);
        startBluetooth();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // User chose not to enable Bluetooth.
        if (requestCode == REQUEST_ENABLE_BLE && resultCode == Activity.RESULT_CANCELED) {
            finish();
            return;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onPause() {
        super.onPause();
        unregisterReceiver(mBleUpdateReceiver);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Close and unbind the service when the activity goes away
        mSmartDiaperBleService.close();
        unbindService(mServiceConnection);
        mSmartDiaperBleService = null;
    }


    /**
     * This method handles the start bluetooth button
     *
     *
     */
    public void startBluetooth() {

        // Find BLE service and adapter
        final BluetoothManager bluetoothManager =
                (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter mBluetoothAdapter = bluetoothManager.getAdapter();

        // Ensures Bluetooth is enabled on the device.  If Bluetooth is not currently enabled,
        // fire an intent to display a dialog asking the user to grant permission to enable it.
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, REQUEST_ENABLE_BLE);
        }

        // Start the BLE Service
        Log.d(TAG, "Starting BLE Service");
        Intent gattServiceIntent = new Intent(this, BleService.class);
        bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);

        Log.d(TAG, "Bluetooth is Enabled");

    }
    /**
     * This method handles the Disconnect button
     *
     * @param view the view object
     */
    public void Disconnect(View view) {

        if (deviceConnected){
            mSmartDiaperBleService.disconnect();
            deviceConnected=false;
        }else if (deviceFound){
            mSmartDiaperBleService.connect();
        }else{
            mSmartDiaperBleService.scan();
        }
    }

    /**
     * Listener for BLE event broadcasts
     */
    private final BroadcastReceiver mBleUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            switch (action) {
                case BleService.ACTION_BLESCAN_CALLBACK:
                    Log.d(TAG, "inside ble scan callback ");
                    deviceFound =true;
                    mSmartDiaperBleService.connect();
                    break;

                case BleService.ACTION_CONNECTED:
                    /* This if statement is needed because we sometimes get a GATT_CONNECTED */
                    /* action when sending Capsense notifications */
                    if (!deviceConnected) {
                        powerSpinner.setEnabled(true);
                        ble_iv.setImageResource(R.drawable.ic_ble_green);
                        disconnect_button.setBackgroundResource(R.drawable.button_connected_background);
                        disconnect_button.setText("DISCONNECT");
                        mSmartDiaperBleService.discoverServices();
                        deviceConnected = true;
                        Log.d(TAG, "Connected to Device");
                    }
                    break;
                case BleService.ACTION_DISCONNECTED:
                    // Disable the disconnect, discover svc, discover char button, and enable the search button
                    powerSpinner.setEnabled(false);
                    ble_iv.setImageResource(R.drawable.ic_ble);
                    mHumidityTV.setText("N/A");
                    mTemperatureTV.setText("N/A");
                    disconnect_button.setBackgroundResource(R.drawable.button_background);
                    disconnect_button.setText("CONNECT");
                    deviceConnected = false;
                    Log.d(TAG, "Disconnected");
                    break;
                case BleService.ACTION_SERVICES_DISCOVERED:

                    Log.d(TAG, "Services Discovered");
                    mSmartDiaperBleService.enableReadingsNotifications(true);
                    break;
                case BleService.ACTION_DATA_RECEIVED:
                    // This is called after a notify or a read completes
                    Log.i(TAG, "in the action data received broadcast callback");
                    // Get humidity Value
                    int humidityValue = mSmartDiaperBleService.getHumidityValue();
                    Log.i(TAG, "humidity value : "+ humidityValue);
                    mHumidityTV.setText(""+humidityValue+"%");

                    int temperatureValue = mSmartDiaperBleService.getTemperatureValue();
                    Log.i(TAG, "temperature value : "+temperatureValue );
                    mTemperatureTV.setText(""+temperatureValue+"°C");
                    // update humidity widget

                   if (humidityValue >= 96 && temperatureValue >= 32){
                        showDialog();
                    }

                default:
                    break;
            }
        }
    };

    void showDialog(){
        diaperDialog.setContentView(R.layout.wet_dialog_layout);
        diaperDialog.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));
        if (alert==null){
            alert = MediaPlayer.create(this,R.raw.baby_tone);
        }
        Button okBu=diaperDialog.findViewById(R.id.okButton);
        okBu.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                alert.stop();
                alert.release();
                diaperDialog.cancel();
                mSmartDiaperBleService.disconnect();
            }
        });
        alert.start();
        diaperDialog.show();

    }
}
