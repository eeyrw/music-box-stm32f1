#ifndef __SYNTH_CORE_H__
#define __SYNTH_CORE_H__

#include <stdint.h>

#define POLY_NUM 4

typedef struct _SoundUnit
{
	uint32_t  wavetablePos;
	uint8_t envelopeLevel;
	uint16_t envelopePos;
	uint16_t increment;
	int16_t val;
	int16_t sampleVal;
	uint32_t waveTableAddress;
	uint32_t waveTableLen;
	uint32_t waveTableLoopLen;
	uint32_t waveTableAttackLen;

}SoundUnit;



typedef struct _Synthesizer
{
    SoundUnit SoundUnitList[POLY_NUM];
	int32_t mixOut;
    uint32_t lastSoundUnit;
}Synthesizer;//__attribute__ ((packed)) Synthesizer;


extern void SynthInit(Synthesizer* synth);

//#ifdef RUN_TEST
extern void NoteOnC(Synthesizer* synth,uint8_t note);
extern void SynthC(Synthesizer* synth);
extern void GenDecayEnvlopeC(Synthesizer* synth);
//#endif

extern void NoteOnAsm(Synthesizer* synth,uint8_t note);
extern void GenDecayEnvlopeAsm(Synthesizer* synth);
extern void SynthAsm(Synthesizer* synth);


#endif