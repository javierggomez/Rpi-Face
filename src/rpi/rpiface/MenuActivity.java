package rpi.rpiface;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class MenuActivity extends Activity implements OnClickListener {
	Button botonHablar;
	Button botonMover;
	Button botonVotar;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_menu);
		botonHablar = (Button) findViewById(R.id.button_hablar);
		botonMover = (Button) findViewById(R.id.button_mover);
		botonVotar = (Button) findViewById(R.id.button_votar);
		botonHablar.setOnClickListener(this);
		botonMover.setOnClickListener(this);
		botonVotar.setOnClickListener(this);

	}

	public void onClick(View v) {

		switch (v.getId()) {
		case R.id.button_hablar:
			Intent intentHablar = new Intent(this, SpeakFaceActivity.class);
			startActivity(intentHablar);
			break;
		case R.id.button_mover:
			Intent intentMover = new Intent(this, MoveFaceActivity.class);
			startActivity(intentMover);
			break;
		case R.id.button_votar:
			Intent intentVotar = new Intent(this, VoteActivity.class);
			startActivity(intentVotar);
			break;
		default:
			Toast.makeText(getApplicationContext(),
					"Error, esa opción no está implementada",
					Toast.LENGTH_SHORT).show();
			break;
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.activity_menu, menu);
		return true;
	}
}
