// make a noisy sinus for test,
// but here should be a sensor code 
// sinus
float f_off = 20.0; // signal base 
float f_amp = 10.0; // sinus amplitude
float f_sin = 0.0;  // running sinus
float f_val = 0.0;  // running noisy sinus
int i_count = 0;
int i_end = 180;
float f_dw = 2 * 3.14159 / i_end;
// random noise
long l_random = 50;
float f_noise_amp = 3.0;
float f_noise = 0.0;    // between 0 .. f_noise_amp in l_random steps

int i_deltaT = 5;   // sec for measurement
long oldtime = 0;
bool diag_sin = true;
char signal_buffer[50];

float mysignal() {
  f_noise = f_noise_amp*random(l_random)/l_random;
  if (!relayState) {
            f_sin = f_off + f_amp * sin(i_count * f_dw);
  } else {  f_sin = 0.0; }
  f_val = f_noise + f_sin;
  i_count += 1;
  if (i_count >= i_end ) { i_count = 0; }
  if (diag_sin) {
    Serial.print("time: "); Serial.print(oldtime);  
    Serial.print(", i_count: "); Serial.print(i_count);
    Serial.print(", f_sin: "); Serial.print(f_sin);
    Serial.print(", f_noise: "); Serial.print(f_noise);
    Serial.print(", f_val: "); Serial.println(f_val);
  }
  return f_val;  
}

void make_signal() {
 long newtime = millis(); 
 if ( newtime > oldtime + i_deltaT*1000 ) {
  oldtime=newtime;
  float f_signal = mysignal();
  String mysignal_string="{\"SIN\":"+String(f_signal)+"}";
  mysignal_string.toCharArray(signal_buffer,mysignal_string.length()+1);
  client.publish(sensorTopic,signal_buffer);
 }  
}

