package mobi.ifunny.search.explore;

import java.util.Map;

package com.flurry.sdk;

import com.flurry.sdk.km;
import com.flurry.sdk.ly;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;

/**
 * Created by alejandro on 09.09.16.
 */

public class FlurryEvent {//jc
	private final Map<String, String> parameters = new HashMap();//e
	private int id;
	public String eventName;//a
	private long deltaSessionAndStartEvent;
	public boolean isTimedEvent;
	public boolean isReadyToSend;
	public long duration;

	public FlurryEvent(int id, String eventName, Map<String, String> parameters, long deltaSessionAndStartEvent, boolean timed) {
		this.id = id;
		this.eventName = eventName;
		if(parameters != null) {
			this.parameters.putAll(parameters);
		}
		this.deltaSessionAndStartEvent = deltaSessionAndStartEvent;
		this.isTimedEvent = timed;
		this.isReadyToSend = !this.isTimedEvent;// ? WTF
		// if Timed, this is FALSE
		// if without time, this is TRUE
	}

	public final void stopTrackEvent(long endTime) {
		this.isReadyToSend = true;
		this.duration = endTime - this.deltaSessionAndStartEvent;
		km.a(3, "FlurryAgent", "Ended event \'" + this.eventName + "\' (" + this.deltaSessionAndStartEvent + ") after " + this.duration + "ms");
	}

	public final synchronized void a(Map<String, String> var1) {
		if(var1 != null) {
			this.parameters.putAll(var1);
		}

	}

	public final synchronized Map<String, String> a() {
		return new HashMap(this.parameters);
	}

	public final synchronized void b(Map<String, String> var1) {
		this.parameters.clear();
		this.parameters.putAll(var1);
	}

	public final synchronized byte[] b() {
		DataOutputStream var2 = null;

		byte[] var1;
		try {
			ByteArrayOutputStream var9 = new ByteArrayOutputStream();
			(var2 = new DataOutputStream(var9)).writeShort(this.id);
			var2.writeUTF(this.eventName);
			var2.writeShort(this.parameters.size());
			Iterator var3 = this.parameters.entrySet().iterator();

			while(var3.hasNext()) {
				Map.Entry var4 = (Map.Entry)var3.next();
				var2.writeUTF(ly.b((String)var4.getKey()));
				var2.writeUTF(ly.b((String)var4.getValue()));
			}

			var2.writeLong(this.deltaSessionAndStartEvent);
			var2.writeLong(this.duration);
			var2.flush();
			var1 = var9.toByteArray();
		} catch (IOException var7) {
			var1 = new byte[0];
		} finally {
			ly.a(var2);
		}

		return var1;
	}
}
