import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.bluetooth.le.BluetoothLeScanner;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.ParcelUuid;
import android.util.Log;

import androidx.core.app.ActivityCompat;

import java.util.Collections;
import java.util.List;
import java.util.UUID;

public class BLEManager {
    private static final String TAG = "BLEManager";

    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bluetoothLeScanner;
    private BluetoothGatt bluetoothGatt;
    private Context context;

    // Service UUID と Characteristic UUID
    private final UUID SERVICE_UUID = UUID.fromString("0000180D-0000-1000-8000-00805f9b34fb");
    private final UUID CHARACTERISTIC_UUID = UUID.fromString("00002A37-0000-1000-8000-00805f9b34fb");
    private final UUID CCCD_UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");

    public BLEManager(Context context) {
        this.context = context;
        BluetoothManager manager = (BluetoothManager) context.getSystemService(Context.BLUETOOTH_SERVICE);
        bluetoothAdapter = manager.getAdapter();
        bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();
    }

    public void startScan() {
        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
            Log.e(TAG, "BLUETOOTH_SCANの権限がない");
            return;
        }

        ScanFilter filter = new ScanFilter.Builder()
                //.setDeviceName("BLEデバイス名")
                //.setServiceUuid(new ParcelUuid(SERVICE_UUID))
                .build();

        ScanSettings settings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
                .build();

        bluetoothLeScanner.startScan(Collections.singletonList(filter), settings, scanCallback);
        Log.d(TAG, "BLE装置のスキャンを開始");
    }

    public void stopScan() {
        if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
            return;
        }
        bluetoothLeScanner.stopScan(scanCallback);
    }

    private final ScanCallback scanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            BluetoothDevice device = result.getDevice();
            Log.d(TAG, "スキャンされたデバイス: " + device.getName() + " - " + device.getAddress());

            stopScan();

            if (ActivityCompat.checkSelfPermission(context, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                Log.e(TAG, "BLUETOOTH_CONNECTの権限がない");
                return;
            }

            bluetoothGatt = device.connectGatt(context, false, gattCallback);
        }
    };

    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {

        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothGatt.STATE_CONNECTED) {
                Log.d(TAG, "コネクトしました。サービスを探し始まります");
                gatt.discoverServices();
            } else if (newState == BluetoothGatt.STATE_DISCONNECTED) {
                Log.d(TAG, "デバイスとのリンクが断った");
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            BluetoothGattService service = gatt.getService(SERVICE_UUID);
            if (service == null) {
                Log.e(TAG, "指定するサービスが見つからない");
                return;
            }

            BluetoothGattCharacteristic characteristic = service.getCharacteristic(CHARACTERISTIC_UUID);
            if (characteristic == null) {
                Log.e(TAG, "特性値が見つからない");
                return;
            }

            gatt.setCharacteristicNotification(characteristic, true);
            BluetoothGattDescriptor descriptor = characteristic.getDescriptor(CCCD_UUID);
            if (descriptor != null) {
                descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
                gatt.writeDescriptor(descriptor);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            byte[] data = characteristic.getValue();
            StringBuilder sb = new StringBuilder();
            for (byte b : data) {
                sb.append(String.format("%02X ", b));
            }
            Log.d(TAG, "もらったデータ: " + sb.toString().trim());
        }
    };
}
