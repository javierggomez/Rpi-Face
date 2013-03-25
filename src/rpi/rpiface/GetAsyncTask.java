package rpi.rpiface;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.utils.URLEncodedUtils;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import android.os.AsyncTask;
import android.util.Log;

public class GetAsyncTask extends AsyncTask<String, Float, Boolean> {

	private static final String LOGTAG = GetAsyncTask.class.getCanonicalName();

	protected void onPreExecute() {
		Log.i(LOGTAG + " Asynctask", "Async iniciada");
	}

	protected Boolean doInBackground(String... param) {
		String value = param[0];
		String rpi = param[1];
		String port = param[2];
		String rpiPath = param[3];
		String rpiParam = param[4];
		HttpClient httpClient = new DefaultHttpClient();
		List<NameValuePair> params = new LinkedList<NameValuePair>();
		params.add(new BasicNameValuePair(rpiParam, value));
		String paramString = URLEncodedUtils.format(params, "iso-8859-15");
		String url = rpi + ":" + port + rpiPath + "?" + paramString;
		Log.i(LOGTAG + " Asynctask" + " Http get:", url);
		HttpGet httpGet = new HttpGet(url);
		try {
			HttpResponse response = httpClient.execute(httpGet);
			Log.i(LOGTAG + " Asynctask" + " Http Response:",
					response.toString());
		} catch (ClientProtocolException e) {
			Log.i(LOGTAG + " Asynctask" + " Http Response:",
					"Error en el protocolo httml");
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			Log.i(LOGTAG + " Asynctask" + " Http Response:",
					"Conexión abortada. No se pudo contactar con el servidor");
			e.printStackTrace();
			return false;
		}
		return true;

	}

	protected void onPostExecute(Integer bytes) {
		Log.i(LOGTAG + " Asynctask", "Async terminada");
	}

}