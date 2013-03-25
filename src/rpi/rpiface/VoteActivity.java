package rpi.rpiface;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class VoteActivity extends Activity implements OnClickListener {
	Button botonYes;
	Button botonNo;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_vote);
		botonYes = (Button) findViewById(R.id.button_yes);
		botonNo = (Button) findViewById(R.id.button_no);
		botonYes.setOnClickListener(this);
		botonNo.setOnClickListener(this);

	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.activity_vote, menu);
		return true;
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.button_yes:

			break;
		case R.id.button_no:

			break;

		default:
			Toast.makeText(getApplicationContext(),
					"Error, esa opción no está implementada",
					Toast.LENGTH_SHORT).show();
			break;
		}

	}
}
