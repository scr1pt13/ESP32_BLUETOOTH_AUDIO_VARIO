#include <Arduino.h>
#include <config.h>
#include <nvd.h>
#include <audio.h>
#include "vaudio.h"

typedef struct BEEP_ {
    int periodTicks;  // on-time + off-time
    int endTick; // on-time
} BEEP;

#define CLAMP(x,mn,mx)       {if (x <= (mn)) x = (mn); else if (x >= (mx)) x = (mx);}

// clamp climbrate/sinkrate for audio feedback to +/- 10 m/s
#define VARIO_MAX_CPS         1000

static int32_t CurrentCps; // internal state : current climb/sink rate
static int32_t CurrentFreqHz; // internal state : current frequency being generated
// sinktone indicates sinking air, this is a warning tone
static int32_t SinkToneCps; // threshold in cm per second
// climbtone indicates lift is strong enough to try turning to stay in it
static int32_t ClimbToneCps; // threshold in cm per second
// zeroestone indicates weak lift, possibility of stronger lift/thermal nearby
static int32_t ZeroesToneCps; // threshold in cm per second
// allocate roughly 1 decade (10:1) of speaker frequency bandwidth to climbrates below
// crossoverCps, and 1 octave (2:1) of frequency bandwidth to climbrates above
// crossoverCps. So if you are flying in strong conditions, increase crossoverCps.
// If you are flying in weak conditions, decrease crossoverCps.
static int32_t CrossoverCps;

static int BeepPeriodTicks; // internal state : current beep interval in ticks
static int BeepEndTick; // internal state : current beep  on-time in ticks
static int Tick; // internal state : current tick ( 1 tick ~= 20mS)
// {beep_period, beep_on_time} based on vertical climbrate in 1m/s intervals
static const BEEP BeepTbl[10] = {
{16,10}, // 0m/s to +1m/s
{14,9},  
{12,8},
{10,7},
{9,6},
{8,5},
{7,4},
{6,3},
{5,2},
{4,2}, // +9m/s to +10m/s
};	

static void vaudio_reset();


void vaudio_config() {
    SinkToneCps    =  (int32_t)Config.vario.sinkThresholdCps;
    ClimbToneCps   =  (int32_t)Config.vario.climbThresholdCps;
    ZeroesToneCps  =  (int32_t)Config.vario.zeroThresholdCps;
    CrossoverCps   =  (int32_t)Config.vario.crossoverCps;
#ifdef VARIO_DEBUG  
    dbg_printf(("climbToneCps = %d\r\n", ClimbToneCps));
    dbg_printf(("zeroesToneCps = %d\r\n", ZeroesToneCps));
    dbg_printf(("sinkToneCps = %d\r\n", SinkToneCps));
    dbg_printf(("crossoverCps = %d\r\n", CrossoverCps));
#endif  
    BeepPeriodTicks	= 0;
    BeepEndTick 	= 0;
    CurrentCps 		= 0;
    CurrentFreqHz  	= 0;
	Tick 			= 0;
    }


static void vaudio_reset(int32_t nCps) {
	CurrentCps = nCps;
	Tick = 0;
	// if sinking significantly faster than glider sink rate in still air, generate warning sink tone
	if (CurrentCps <= SinkToneCps) {
			BeepPeriodTicks = 40; 
			BeepEndTick  = 30;			
			CurrentFreqHz = 220;
			audio_set_frequency(CurrentFreqHz);
			}		
	//if climbing, generate beeps
	else {
		if (CurrentCps >= ClimbToneCps) {
				int index = CurrentCps/100;
				if (index > 9) index = 9;
				BeepPeriodTicks = BeepTbl[index].periodTicks;
				BeepEndTick = BeepTbl[index].endTick;
				if (CurrentCps > CrossoverCps) {
					CurrentFreqHz = VARIO_CROSSOVER_FREQHZ + ((CurrentCps - CrossoverCps)*(VARIO_SPKR_MAX_FREQHZ - VARIO_CROSSOVER_FREQHZ))/(VARIO_MAX_CPS - CrossoverCps);
					}
				else {
					CurrentFreqHz = VARIO_SPKR_MIN_FREQHZ + ((CurrentCps - ZeroesToneCps)*(VARIO_CROSSOVER_FREQHZ - VARIO_SPKR_MIN_FREQHZ))/(CrossoverCps - ZeroesToneCps);
					}
				CLAMP(CurrentFreqHz, VARIO_SPKR_MIN_FREQHZ, VARIO_SPKR_MAX_FREQHZ);
				audio_set_frequency(CurrentFreqHz);				
			}
		else   // in "zeroes" band, indicate with a short pulse and long interval
		if (CurrentCps >= ZeroesToneCps) {
			BeepPeriodTicks = 60;
			BeepEndTick = 10;
			CurrentFreqHz = 300;
			audio_set_frequency(CurrentFreqHz);
			}            
		// between zeroes threshold and sink threshold, chillout
		else {
			BeepPeriodTicks = 0;
			BeepEndTick  = 0;
			CurrentFreqHz = 0;
			audio_set_frequency(CurrentFreqHz);
			}
		}
	}


void vaudio_tick_handler(int32_t nCps) {
    // generate new beep/tone only if current beep/tone has ended, OR climb threshold exceeded
    if ((BeepPeriodTicks <= 0)  || ((nCps >= ClimbToneCps) && (CurrentCps < ClimbToneCps)) ) {
		vaudio_reset(nCps);
        }
    else { // still processing current beep/tone
		int32_t newFreqHz;
        Tick++;
        BeepPeriodTicks--;
        if (Tick >= BeepEndTick) { // shut off climb beep after 'on' time ends
            newFreqHz = 0;
        }
        else {
            newFreqHz = CurrentFreqHz; // no change   
        }
        if (newFreqHz != CurrentFreqHz) {
            CurrentFreqHz = newFreqHz;
            audio_set_frequency(CurrentFreqHz);
            }
        }
    }
