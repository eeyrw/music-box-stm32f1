.syntax unified
.section .text
.thumb_func
.global SynthAsm
.global GenDecayEnvlopeAsm
.global NoteOnAsm

@ typedef struct _SoundUnit
@ {
@ 	uint32_t wavetablePos;
@ 	uint32_t waveTableAddress;
@ 	uint32_t waveTableLen;
@ 	uint32_t waveTableLoopLen;
@ 	uint32_t waveTableAttackLen;
@ 	uint32_t envelopePos;
@ 	uint32_t increment;
@ 	int32_t val;
@ 	int32_t sampleVal;
@ 	uint32_t envelopeLevel;
@ }SoundUnit;

@ typedef struct _Synthesizer
@ {
@     SoundUnit SoundUnitList[POLY_NUM];
@ 	int32_t mixOut;
@     uint32_t lastSoundUnit;
@ }Synthesizer;

.equ pWavetablePos , 0
.equ pWaveTableAddress , 4
.equ pWaveTableLen , 8
.equ pWaveTableLoopLen , 12
.equ pWaveTableAttackLen , 16
.equ pEnvelopePos ,20
.equ pIncrement , 24
.equ pVal , 28
.equ pSampleVal , 32
.equ pEnvelopeLevel , 36
.equ SoundUnitSize,40


.equ ENVELOP_LEN,256
.equ POLY_NUM,5
.equ pMixOut,0
.equ pLastSoundUnit,4

.func SynthAsm
SynthAsm:
stmfd sp!,{r4-r11,lr}
pSoundUnit .req r0
loopIndex .req r4
mixOut .req r8

mov loopIndex,#POLY_NUM
mov mixOut,#0

loopSynth:
    ldr r5,[pSoundUnit,#pWaveTableAddress]
    ldr r6,[pSoundUnit,#pWavetablePos]
    lsr r6,r6,#7 @wavetablePos*=wavetablePos/256*2
    ldrh r6,[r5,r6] @ Load sample to r6
    sxth r6,r6 @Extend 16bit signed sample to 32bit signed.
    str r6,[pSoundUnit,#pSampleVal]
    ldr r7,[pSoundUnit,#pEnvelopeLevel]
    mul r7,r6,r7 @sample*envelope/256
    asr r7,r7,#8
    str r7,[pSoundUnit,#pVal]
    add mixOut,r7,mixOut @mixOut+=sample*envelope/256

    ldr r6,[pSoundUnit,#pWavetablePos]
    ldr r5,[pSoundUnit,#pIncrement]
    add r6,r5,r6
    ldr r5,[pSoundUnit,#pWaveTableLen]
    lsl r5,r5,#8 @pWaveTableLen*=pWaveTableLen*256    
    cmp r6,r5
    bhi wavePosUpdateEnd  @bhi : HI	C = 1 and Z = 0	Higher, unsigned
    sub r6,r6,r5
    wavePosUpdateEnd:
    str r6,[pSoundUnit,#pWavetablePos]

subs loopIndex,loopIndex,#1 @ set n = n-1
add pSoundUnit,pSoundUnit,#SoundUnitSize
bne loopSynth
str mixOut,[pSoundUnit,pMixOut]
ldmfd sp!,{r4-r11,pc}
.endfunc

.func GenDecayEnvlopeAsm
GenDecayEnvlopeAsm:
pSoundUnit .req r0
loopIndex .req r4
stmfd sp!,{r4-r11,lr}
mov loopIndex,#POLY_NUM
loopGenDecayEnvlope:
@ void GenDecayEnvlopeC(Synthesizer* synth)
@ {
@     SoundUnit* soundUnits = synth->SoundUnitList;
@ 	for (uint32_t i = 0; i < POLY_NUM; i++)
@ 	{
@ 		if((soundUnits[i].wavetablePos>>8) >=soundUnits[i].waveTableAttackLen &&
@ 				soundUnits[i].envelopePos <sizeof(EnvelopeTable)-1)
@ 		{
@ 			soundUnits[i].envelopeLevel = EnvelopeTable[soundUnits[i].envelopePos];
@ 			soundUnits[i].envelopePos += 1;
@ 		}
@ 	}
@ }
    ldr r5,[pSoundUnit,#pWavetablePos]
    ldr r6,[pSoundUnit,#pWaveTableAttackLen]
    lsl r6,r6,#8 @WaveTableAttackLen*=WaveTableAttackLen*256    
    cmp r5,r6
    bhi conditionEnd @bhi : HI	C = 1 and Z = 0	Higher, unsigned
    ldr r5,[pSoundUnit,#pEnvelopePos]
    ldr r6,=#(ENVELOP_LEN-1)
    cmp r5,r6
    bls conditionEnd @bls: LS	C = 0 or   Z = 1	Lower or same, unsigned
    ldr r6,=EnvelopeTable
    ldrb r6,[r6,r5]  @ Load envelope to r6
    uxtb r6,r6 @Extend 8bit unsigned sample to 32bit unsigned.
    str r6,[pSoundUnit,#pEnvelopeLevel]
    add r5,r5,#1
    conditionEnd:
subs loopIndex,loopIndex,#1 @ set n = n-1
add pSoundUnit,pSoundUnit,#SoundUnitSize
bne loopSynth
ldmfd sp!,{r4-r11,pc}
.endfunc

.func NoteOnAsm
NoteOnAsm:
bx lr
.endfunc