package rpi.rpiface;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import android.os.AsyncTask;
import android.util.Log;

public class PostAsyncTask extends AsyncTask<String, Void, Boolean> {

	private static final String LOGTAG = PostAsyncTask.class.getCanonicalName();

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
		String url = rpi + ":" + port + rpiPath;
		HttpPost httpPost = new HttpPost(url);

		List<NameValuePair> params = new LinkedList<NameValuePair>();
		params.add(new BasicNameValuePair(rpiParam, value));

		// String paramString = URLEncodedUtils.format(params, "iso-8859-15");
		Log.i(LOGTAG + " Asynctask" + " Http get:", url);

		try {
			httpPost.setEntity(new UrlEncodedFormEntity(params, "iso-8859-15"));
			HttpResponse response = httpClient.execute(httpPost);
			Log.i(LOGTAG + " Asynctask" + " Http Response:",
					response.toString());
		} catch (ClientProtocolException e) {
			e.printStackTrace();
			return false;
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}

	protected void onPostExecute(Integer bytes) {
		Log.i(LOGTAG + " Asynctask", "Async terminada");
	}

}