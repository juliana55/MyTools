package mbaas.com.nifcloud.kotlincameraquickstart

import android.Manifest
import android.app.AlertDialog
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Bundle
import android.provider.Settings
import android.support.v7.app.AppCompatActivity
import android.view.Menu
import android.view.MenuItem
import android.view.View
import android.widget.Button
import android.widget.ImageView
import com.karumi.dexter.Dexter
import com.karumi.dexter.PermissionToken
import com.karumi.dexter.listener.PermissionDeniedResponse
import com.karumi.dexter.listener.PermissionGrantedResponse
import com.karumi.dexter.listener.PermissionRequest
import com.karumi.dexter.listener.single.PermissionListener
import com.nifcloud.mbaas.core.NCMB
import com.nifcloud.mbaas.core.NCMBAcl
import com.nifcloud.mbaas.core.NCMBFile
import java.io.ByteArrayOutputStream


class MainActivity : AppCompatActivity() {
    internal var b1: Button? = null
    internal var b2: Button? = null
    internal var iv: ImageView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        //**************** APIキーの設定とSDKの初期化 **********************
        NCMB.initialize(this, "YOUR_APPLICATION_KEY", "YOUR_CLIENT_KEY");

        setContentView(R.layout.activity_main)

        b1 = findViewById<View>(R.id.button) as Button
        iv = findViewById<View>(R.id.imageView) as ImageView

        b1?.setOnClickListener { requestCameraPermission() }
    }

    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        // TODO Auto-generated method stub
        super.onActivityResult(requestCode, resultCode, data)
        if (data == null || data.extras == null) {
            return
        } else {
            val bp = data.extras!!.get("data") as Bitmap
            //******* NCMB file upload *******
            val byteArrayStream = ByteArrayOutputStream()
            bp.compress(Bitmap.CompressFormat.PNG, 0, byteArrayStream)
            val dataByte = byteArrayStream.toByteArray()

            //ACL 読み込み:可 , 書き込み:可
            val acl = NCMBAcl()
            acl.publicReadAccess = true
            acl.publicWriteAccess = true

            //通信実施
            val file = NCMBFile("test.png", dataByte, acl)
            file.saveInBackground { e ->
                val result: String
                if (e != null) {
                    //保存失敗
                    AlertDialog.Builder(this@MainActivity)
                            .setTitle("Notification from NIFCloud")
                            .setMessage("Error:" + e.message)
                            .setPositiveButton("OK", null)
                            .show()
                } else {
                    //******* NCMB file download *******
                    val file = NCMBFile("test.png")
                    file.fetchInBackground { dataFetch, er ->
                        if (er != null) {
                            //失敗処理
                            AlertDialog.Builder(this@MainActivity)
                                    .setTitle("Notification from NIFCloud")
                                    .setMessage("Error:" + er.message)
                                    .setPositiveButton("OK", null)
                                    .show()
                        } else {
                            //成功処理
                            val bMap = BitmapFactory.decodeByteArray(dataFetch, 0, dataFetch.size)
                            iv?.setImageBitmap(bMap)
                        }
                    }


                }
            }
        }
    }

    private fun requestCameraPermission() {
        Dexter.withActivity(this)
                .withPermission(Manifest.permission.CAMERA)
                .withListener(object : PermissionListener {
                    override fun onPermissionGranted(response: PermissionGrantedResponse) {
                        // permission is granted
                        openCamera()
                    }

                    override fun onPermissionDenied(response: PermissionDeniedResponse) {
                        // check for permanent denial of permission
                        if (response.isPermanentlyDenied) {
                            showSettingsDialog()
                        }
                    }

                    override fun onPermissionRationaleShouldBeShown(permission: PermissionRequest, token: PermissionToken) {
                        token.continuePermissionRequest()
                    }
                }).check()
    }

    private fun showSettingsDialog() {
        val builder = AlertDialog.Builder(this@MainActivity)
        builder.setTitle("Need Permissions")
        builder.setMessage("This app needs permission to use this feature. You can grant them in app settings.")
        builder.setPositiveButton("GOTO SETTINGS") { dialog, which ->
            dialog.cancel()
            openSettings()
        }
        builder.setNegativeButton("Cancel") { dialog, which -> dialog.cancel() }
        builder.show()

    }

    // navigating user to app settings
    private fun openSettings() {
        val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS)
        val uri = Uri.fromParts("package", packageName, null)
        intent.data = uri
        startActivityForResult(intent, 101)
    }

    private fun openCamera() {
        val intent = Intent(android.provider.MediaStore.ACTION_IMAGE_CAPTURE)
        startActivityForResult(intent, 0)
    }

    override fun onDestroy() {
        super.onDestroy()
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.

        val id = item.itemId


        return if (id == R.id.action_settings) {
            true
        } else super.onOptionsItemSelected(item)
    }
}