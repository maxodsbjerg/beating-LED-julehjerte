#include <Arduino.h>
#include <math.h>
 
 //
 // Calm resting heartbeat LED
 // Soft, rounded pulses – meditative
 //
 
 const int ledPin = 5;
 const float gammaVal = 2.4;
const float baseBrightness = 0.35;   // LED never goes below this
const float pulseDepth     = 0.35;   // how strong the heartbeat is

 
 // Calm heart rate
 float baseBPM = 40.0; // slow resting HR
 float bpmVariation = 1.5; // very subtle variation
 
 unsigned long cycleStart = 0;
 unsigned long beatPeriod = 0;
 
 void setup() {
 pinMode(ledPin, OUTPUT);
 randomSeed(analogRead(A0));
 updateBeatPeriod();
 }
 
 void updateBeatPeriod() {
 float bpm = baseBPM + random(-bpmVariation * 10, bpmVariation * 10) / 10.0;
 beatPeriod = (unsigned long)(60000.0 / bpm);
 }
 
 // Smooth pulse using raised cosine
 float smoothPulse(float x, float width) {
 if (x < 0.0 || x > width) return 0.0;
 float phase = x / width;
 return 0.5 - 0.5 * cos(PI * phase);
 }
 
 void loop() {
  unsigned long now = millis();
  unsigned long t = now - cycleStart;

  if (t >= beatPeriod) {
    cycleStart = now;
    updateBeatPeriod();
    t = 0;
  }

  float period = (float)beatPeriod;

  // Timing of the soft double pulse
  float lubStart = 0.12 * period;
  float dubStart = 0.32 * period;

  float lubWidth = 0.30 * period;
  float dubWidth = 0.22 * period;

  float pulse = 0.0;

  // LUB
  pulse += smoothPulse(t - lubStart, lubWidth);

  // DUB (very gentle echo)
  pulse += 0.35 * smoothPulse(t - dubStart, dubWidth);

  pulse = constrain(pulse, 0.0, 1.0);

  // Map pulse onto continuous glow
  float brightness =
      baseBrightness +
      pulseDepth * pulse;

  brightness = constrain(brightness, 0.0, 1.0);

  // Gamma correction
  brightness = pow(brightness, gammaVal);

  int pwmValue = (int)(brightness * 255.0 + 0.5);
  analogWrite(ledPin, pwmValue);

  delay(8);   // slightly slower update = calmer feel
}