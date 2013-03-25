package rpi.rpiface;

import java.util.concurrent.ExecutionException;

import android.os.AsyncTask;
import android.os.Bundle;
import android.app.Activity;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class SpeakFaceActivity extends Activity implements OnClickListener {

	private static final String LOGTAG = SpeakFaceActivity.class
			.getCanonicalName();
	Button botonVoice;
	Button botonText;
	Button botonRecord;
	Button botonSend;
	EditText editInsert;
	private final String RPI_PARAM = "message";
	private ConnectionStatus cd;
	Boolean internetConnection = false;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_speak);
		Log.v(LOGTAG, "Se ha cargado la interfaz de SpeakFaceActivity");
		botonVoice = (Button) findViewById(R.id.button_voice);
		botonText = (Button) findViewById(R.id.button_text);
		botonRecord = (Button) findViewById(R.id.button_record);
		botonSend = (Button) findViewById(R.id.button_send);
		editInsert = (EditText) findViewById(R.id.editText_text);
		voiceMode();

		botonVoice.setOnClickListener(this);
		botonText.setOnClickListener(this);
		botonRecord.setOnClickListener(this);
		botonSend.setOnClickListener(this);
		Log.v(LOGTAG, "Actividad creada");
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.activity_speak, menu);
		Log.i(LOGTAG, "Se ha creado el menú con éxito");
		return true;
	}

	private void voiceMode() {
		botonSend.setVisibility(View.GONE);
		editInsert.setVisibility(View.GONE);
		botonRecord.setVisibility(View.VISIBLE);
		Log.i(LOGTAG, "Se ha pasado al modo voz");
	}

	private void textMode() {
		botonSend.setVisibility(View.VISIBLE);
		editInsert.setVisibility(View.VISIBLE);
		botonRecord.setVisibility(View.GONE);
		Log.i(LOGTAG, "Se ha pasado al modo texto");
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.button_voice:
			Log.i(LOGTAG + " onclick", "Se ha pulsado el botón de voz");
			voiceMode();
			break;
		case R.id.button_text:
			Log.i(LOGTAG + " onclick", "Se ha pulsado el botón de texto");
			textMode();
			break;
		case R.id.button_record:
			Log.i(LOGTAG + " onclick", "Se ha pulsado el botón de grabación");

			break;
		case R.id.button_send:
			String text = editInsert.getText().toString();
			Log.i(LOGTAG + " onclick", "Se ha pulsado el botón de enviar");
			Log.v(LOGTAG + " Texto a enviar:", text);

			cd = new ConnectionStatus(getApplicationContext());
			internetConnection = cd.isConnectingToInternet();
			if (!internetConnection) {
				Toast.makeText(getApplicationContext(),
						"No está conectado a internet.", Toast.LENGTH_SHORT)
						.show();
				return;
			}

			AsyncTask<String, Void, Boolean> postAsyncTask = new PostAsyncTask()
					.execute(text, Url.RPI, Url.RPI_PORT, Url.RPI_PATH, RPI_PARAM);
			try {
				if (!postAsyncTask.get()) {
					Toast.makeText(
							getApplicationContext(),
							"Hubo problemas con el servidor. Reinténtelo más tarde.",
							Toast.LENGTH_SHORT).show();
				}
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (ExecutionException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			break;
		default:
			Log.w(LOGTAG + " onclick", "Esa función no está implementada");
			Toast.makeText(getApplicationContext(),
					"Error, esa opción no está implementada",
					Toast.LENGTH_SHORT).show();
			break;
		}

	}

	// Conservado por si se quiere hacer privado, de momento p�blico funciona y
	// se ahorran l�neas de c�digo
	/*
	 * private class Get1 extends AsyncTask<String, Float, Integer> {
	 * 
	 * protected void onPreExecute() { Log.i(LOGTAG + " Asynctask",
	 * "Async iniciada"); }
	 * 
	 * protected Integer doInBackground(String... param) { String value =
	 * param[0]; HttpClient httpClient = new DefaultHttpClient();
	 * List<NameValuePair> params = new LinkedList<NameValuePair>();
	 * params.add(new BasicNameValuePair(RPI_PARAM, value)); String paramString
	 * = URLEncodedUtils.format(params, "utf-8"); String url = RPI + RPI_PATH +
	 * "?" + paramString; Log.i(LOGTAG + " Asynctask" + " Http get:", url);
	 * HttpGet httpGet = new HttpGet(url); try { HttpResponse response =
	 * httpClient.execute(httpGet); Log.i(LOGTAG + " Asynctask" +
	 * " Http Response:", response.toString()); } catch (ClientProtocolException
	 * e) { e.printStackTrace(); } catch (IOException e) { e.printStackTrace();
	 * } return null; }
	 * 
	 * protected void onPostExecute(Integer bytes) { Log.i(LOGTAG +
	 * " Asynctask", "Async terminada"); }
	 * 
	 * }
	 */

}
