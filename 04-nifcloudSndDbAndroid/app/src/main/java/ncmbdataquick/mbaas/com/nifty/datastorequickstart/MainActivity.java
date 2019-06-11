package ncmbdataquick.mbaas.com.nifcloud.datastorequickstart;

import android.app.AlertDialog;
import android.os.Bundle;
//import android.support.design.widget.FloatingActionButton;
//import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;

//import com.nifty.cloud.mb.core.DoneCallback;
//import com.nifty.cloud.mb.core.NCMB;
//import com.nifty.cloud.mb.core.NCMBException;
//import com.nifty.cloud.mb.core.NCMBObject;
import com.nifcloud.mbaas.core.DoneCallback;
import com.nifcloud.mbaas.core.NCMB;
import com.nifcloud.mbaas.core.NCMBException;
import com.nifcloud.mbaas.core.NCMBObject;


public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        //**************** APIキーの設定とSDKの初期化 **********************
        NCMB.initialize(this, "c229cea695ec78ddfe6a5eb5cd69f11318db14f05d7dc990ac7b8d3408497579", "a51818098d824662b6723bdf86dbce3963dc47abf64796c234dad321dcdacf48");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public void doStartDemo(View view) {
        final NCMBObject obj = new NCMBObject("TestClass");
        try
        {
            obj.put("message", "Hello, NCMB!");
            obj.saveInBackground(new DoneCallback()
            {
                @Override
                public void done(NCMBException e)
                {
                    if (e != null)
                    {
                        //保存失敗
                        new AlertDialog.Builder(MainActivity.this)
                                .setTitle("Notification from NIFCLOUD")
                                .setMessage("Error:" + e.getMessage())
                                .setPositiveButton("OK", null)
                                .show();

                    }
                    else
                     {
                        //保存成功
                        new AlertDialog.Builder(MainActivity.this)
                                .setTitle("Notification from NIFCLOUD")
                                .setMessage("Save successfull! with ID:" + obj.getObjectId())
                                .setPositiveButton("OK", null)
                                .show();

                    }
                }
            }
            );
        }
        catch (NCMBException e)
        {
            e.printStackTrace();
        }


    }
}
