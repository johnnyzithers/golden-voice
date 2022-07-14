#include "daisysp.h"
#include "daisy_seed.h"

// Interleaved audio definitions
#define LEFT (i)
#define RIGHT (i + 1)

// Set max delay time to 0.75 of samplerate.
#define MAX_DELAY static_cast<size_t>(48000 * 0.75f)

using namespace daisysp;
using namespace daisy;

static DaisySeed seed;

#define TOGGLE_PIN_1 12
#define LED_PIN_1 28


// Helper Modules
static AdEnv      env;
static Oscillator osc;
static Metro      tick;

float delTime; 
float sample_rate;

// Declare a DelayLine of MAX_DELAY number of floats.
static DelayLine<float, MAX_DELAY> del;

bool shiftState;

Led led1;
static Parameter freqParam, resParam;
Switch toggle;

/** Knobs */
enum Knob
{
    KNOB_1,
    KNOB_2,
    KNOB_LAST,
};
AnalogControl knob1, knob2, *knobs[KNOB_LAST];


void updateLeds()
{
    // toggle.Debounce();
    // if(toggle.Pressed()){	    
        shiftState = !shiftState;
    // }
    led1.Set(shiftState);
    led1.Update();
}

static void AudioCallback(float *in, float *out, size_t size)
{
    updateLeds();

    float osc_out, env_out, feedback, del_out, sig_out;
    for(size_t i = 0; i < size; i += 2)
    {
        // When the Metro ticks:
        // trigger the envelope to start, and change freq of oscillator.
        if(tick.Process())
        {
            float freq = rand() % 200;
            osc.SetFreq(freq + 100.0f);
            env.Trigger();
        }

        // Use envelope to control the amplitude of the oscillator.
        env_out = env.Process();
        osc.SetAmp(env_out);
        osc_out = osc.Process();

        osc_out = in[RIGHT];
        // Read from delay line
        del_out = del.Read();
        delTime = delTime*1.1f;
        if(delTime > sample_rate * 2.0) {delTime = 1024;}
        // del.SetDelay(delTime);

        // Calculate output and feedback
        sig_out  = del_out + osc_out;
        feedback = (del_out * 0.75f) + osc_out;

        // Write to the delay
        del.Write(feedback);

        // Output
        out[LEFT]  = sig_out;
        out[RIGHT] = sig_out;
    }
}

int main(void)
{
    // initialize seed hardware and daisysp modules
    seed.Configure();
    seed.Init();
    sample_rate = seed.AudioSampleRate();
    env.Init(sample_rate);
    osc.Init(sample_rate);
    del.Init();

    // Set up Metro to pulse every second
    tick.Init(1.0f, sample_rate);

    led1.Init(seed.GetPin(LED_PIN_1), false);
    led1.Set(0.0);
    led1.Update();
    seed.SetLed(1.0);

    toggle.Init(seed.GetPin(TOGGLE_PIN_1), 1000);

    shiftState = false;



    // set adenv parameters
    env.SetTime(ADENV_SEG_ATTACK, 0.001);
    env.SetTime(ADENV_SEG_DECAY, 0.50);
    env.SetMin(0.0);
    env.SetMax(0.25);
    env.SetCurve(0); // linear

    // Set parameters for oscillator
    osc.SetWaveform(osc.WAVE_TRI);
    osc.SetFreq(220);
    osc.SetAmp(0.25);

    // Set Delay time to 0.75 seconds
    delTime = sample_rate * 0.75f;
    del.SetDelay(delTime);


    // start callback
    seed.StartAudio(AudioCallback);


    while(1) {}
}
