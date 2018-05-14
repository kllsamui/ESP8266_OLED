//_____________________________________________________ SINUS
float f_off = 20.0; // signal base 
float f_amp = 10.0; // sinus amplitude
float f_sin = 0.0;  // running sinus
float f_val = 0.0;  // running noisy sinus
int i_count = 0;
int i_end = 180;
float f_dw = 2 * 3.14159 / i_end;
//_____________________________________________________ Filter
float A = 0.1;
float B = 1.0 - A;
float f_last = 0.0;
float f_fil = 0.0;  // filtered sinus
//_____________________________________________________ LED / RELAY to MQTT
float f_led = 0.0;

//_____________________________________________________ random noise
long l_random = 50;
float f_noise_amp = 3.0;
float f_noise = 0.0;    // between 0 .. f_noise_amp in l_random steps
//_____________________________________________________ Timing
int i_deltaT = 15;   // sec for measurement
long oldtime = 0;
bool diag_sin = true;
char signal_buffer[50];

float my_signal() {
  f_noise = f_noise_amp*random(l_random)/l_random;
  if (relayState == 0) {
            f_sin = f_off + f_amp * sin(i_count * f_dw);
  } else {  
            f_sin = 0.0; 
          }
  f_val = f_noise + f_sin;
  i_count += 1;
  if (i_count >= i_end ) { i_count = 0; }
}

float my_filter() {
  f_fil = f_val * A + f_last * B;
  f_last = f_fil;
}

void make_signal() {
 long newtime = millis(); 
 if ( newtime > oldtime + i_deltaT*1000 ) {
  oldtime=newtime;
  my_signal();             // makes f_val
  my_filter();            // makes f_fil
  f_led = 0.0;
  if (relayState == 1){        // from main
    f_led = 1.0;          // makes f_led ( status )
  }
  if (diag_sin) {
    Serial.print("time: "); Serial.print(oldtime);  
    Serial.print(", i_count: "); Serial.print(i_count);
    Serial.print(", f_sin: "); Serial.print(f_sin);
    Serial.print(", f_noise: "); Serial.print(f_noise);
    Serial.print(", f_val: "); Serial.print(f_val);
    Serial.print(", f_fil: "); Serial.print(f_fil);
    Serial.print(", f_led: "); Serial.println(f_led);
  }
  String mysignal_string="{\"Sin\":"+String(f_val)+",\"Fil\":"+String(f_fil)+",\"Led\":"+String(f_led)+"}";
  mysignal_string.toCharArray(signal_buffer,mysignal_string.length()+1);
  client.publish(sensorTopic,signal_buffer);
 }  
}

