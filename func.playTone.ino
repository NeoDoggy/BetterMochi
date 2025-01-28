extern const int TONE_OUTPUT_PIN;
extern const int TONE_PWM_CHANNEL;



void playSong(int tones[],int tt,int l){
  for(int i=0;i<l;i++){
    ledcWriteTone(TONE_PWM_CHANNEL,tones[i]);
    delay(tt);
  }
  ledcWriteTone(TONE_PWM_CHANNEL,0);
}