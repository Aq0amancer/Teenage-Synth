#ifndef SYNTHARCH_H
#define SYNTHARCH_H
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform LFO_PWM;             //xy=233,605
AudioSynthWaveformDc dcEnvelope;        //xy=233,741
AudioSynthWaveform LFO_Pitch;           //xy=244,474
AudioMixer4 pwmmixer2;                  //xy=396,641
AudioMixer4 pwmmixer1;                  //xy=397,574
AudioEffectEnvelope filterEnvelope;     //xy=400,736
AudioMixer4 pitchmixer;                 //xy=403,467
AudioSynthNoiseWhite noise1;            //xy=516,140
AudioSynthNoisePink pink1;              //xy=517,105
AudioSynthWaveformModulated waveform2;  //xy=629,260
AudioSynthWaveformModulated waveform1;  //xy=630,223
AudioSynthWaveformModulated waveform8;  //xy=631,607
AudioSynthWaveformModulated waveform4;  //xy=632,383
AudioSynthWaveformModulated waveform11; //xy=631,790
AudioSynthWaveformModulated waveform12; //xy=631,823
AudioSynthWaveformModulated waveform14; //xy=631,941
AudioSynthWaveformModulated waveform15; //xy=631,1030
AudioSynthWaveformModulated waveform16; //xy=631,1062
AudioSynthWaveformModulated waveform7;  //xy=633,573
AudioSynthWaveformModulated waveform3;  //xy=635,349
AudioSynthWaveformModulated waveform13; //xy=634,910
AudioSynthWaveformModulated waveform10; //xy=635,712
AudioSynthWaveformModulated waveform5;  //xy=636,463
AudioSynthWaveformModulated waveform6;  //xy=637,499
AudioSynthWaveformModulated waveform9;  //xy=637,682
AudioMixer4 noisemixer;                 //xy=652,134
AudioMixer4 mixer9;                     //xy=807,315
AudioSynthWaveform LFO_Filter;          //xy=809,599
AudioMixer4 mixer11;                    //xy=811,443
AudioMixer4 mixer10;                    //xy=812,379
AudioMixer4 mixer12;                    //xy=813,509
AudioMixer4 mixer13;                    //xy=813,685
AudioMixer4 mixer14;                    //xy=816,751
AudioMixer4 mixer15;                    //xy=817,816
AudioMixer4 mixer16;                    //xy=818,880
AudioSynthWaveformDc keytracking1;      //xy=818,982
AudioSynthWaveformDc keytracking2;      //xy=819,1019
AudioSynthWaveformDc keytracking3;      //xy=820,1057
AudioSynthWaveformDc keytracking4;      //xy=821,1092
AudioSynthWaveformDc keytracking5;      //xy=824,1126
AudioSynthWaveformDc keytracking6;      //xy=827,1164
AudioSynthWaveformDc keytracking8;      //xy=831,1229
AudioSynthWaveformDc keytracking7;      //xy=835,1197
AudioMixer4 filtermixer2;               //xy=1003,403
AudioMixer4 filtermixer3;               //xy=1003,471
AudioMixer4 filtermixer1;               //xy=1004,335
AudioMixer4 filtermixer5;               //xy=1010,606
AudioMixer4 filtermixer4;               //xy=1011,536
AudioMixer4 filtermixer6;               //xy=1012,680
AudioMixer4 filtermixer7;               //xy=1013,757
AudioMixer4 filtermixer8;               //xy=1015,830
AudioFilterStateVariable filter2;       //xy=1143,395
AudioFilterStateVariable filter7;       //xy=1144,723
AudioFilterStateVariable filter4;       //xy=1145,523
AudioFilterStateVariable filter3;       //xy=1146,458
AudioFilterStateVariable filter8;       //xy=1145,789
AudioFilterStateVariable filter5;       //xy=1147,588
AudioFilterStateVariable filter6;       //xy=1147,656
AudioFilterStateVariable filter1;       //xy=1154,332
AudioMixer4 mixer1;                     //xy=1273,332
AudioMixer4 mixer2;                     //xy=1273,398
AudioMixer4 mixer3;                     //xy=1273,464
AudioMixer4 mixer4;                     //xy=1273,528
AudioMixer4 mixer5;                     //xy=1273,594
AudioMixer4 mixer6;                     //xy=1273,660
AudioMixer4 mixer7;                     //xy=1273,727
AudioMixer4 mixer8;                     //xy=1273,794
AudioEffectEnvelope envelope5;          //xy=1402,588
AudioEffectEnvelope envelope2;          //xy=1407,400
AudioEffectEnvelope envelope3;          //xy=1408,470
AudioEffectEnvelope envelope1;          //xy=1409,330
AudioEffectEnvelope envelope4;          //xy=1409,533
AudioEffectEnvelope envelope6;          //xy=1409,666
AudioEffectEnvelope envelope8;          //xy=1411,798
AudioEffectEnvelope envelope7;          //xy=1413,732
AudioMixer4 envmixer2;                  //xy=1574,503
AudioMixer4 envmixer4;                  //xy=1579,675
AudioMixer4 envmixer1;                  //xy=1583,412
AudioMixer4 envmixer3;                  //xy=1583,584
AudioMixer4 mixerL;                     //xy=1796,441
AudioMixer4 mixerR;                     //xy=1802,575
AudioEffectFlange flangerR;             //xy=2002,574
AudioEffectFlange flangerL;             //xy=2003,443
AudioOutputI2S i2s1;                    //xy=2328,493
AudioConnection patchCord1(LFO_PWM, 0, pwmmixer2, 0);
AudioConnection patchCord2(LFO_PWM, 0, pwmmixer1, 0);
AudioConnection patchCord3(dcEnvelope, filterEnvelope);
AudioConnection patchCord4(LFO_Pitch, 0, pitchmixer, 0);
AudioConnection patchCord5(pwmmixer2, 0, waveform2, 1);
AudioConnection patchCord6(pwmmixer2, 0, waveform4, 1);
AudioConnection patchCord7(pwmmixer2, 0, waveform6, 1);
AudioConnection patchCord8(pwmmixer2, 0, waveform8, 1);
AudioConnection patchCord9(pwmmixer2, 0, waveform10, 1);
AudioConnection patchCord10(pwmmixer2, 0, waveform12, 1);
AudioConnection patchCord11(pwmmixer2, 0, waveform14, 1);
AudioConnection patchCord12(pwmmixer2, 0, waveform16, 1);
AudioConnection patchCord13(pwmmixer1, 0, waveform1, 1);
AudioConnection patchCord14(pwmmixer1, 0, waveform3, 1);
AudioConnection patchCord15(pwmmixer1, 0, waveform5, 1);
AudioConnection patchCord16(pwmmixer1, 0, waveform7, 1);
AudioConnection patchCord17(pwmmixer1, 0, waveform9, 1);
AudioConnection patchCord18(pwmmixer1, 0, waveform11, 1);
AudioConnection patchCord19(pwmmixer1, 0, waveform13, 1);
AudioConnection patchCord20(pwmmixer1, 0, waveform15, 1);
AudioConnection patchCord21(filterEnvelope, 0, filtermixer5, 1);
AudioConnection patchCord22(filterEnvelope, 0, filtermixer8, 1);
AudioConnection patchCord23(filterEnvelope, 0, filtermixer7, 1);
AudioConnection patchCord24(filterEnvelope, 0, filtermixer6, 1);
AudioConnection patchCord25(filterEnvelope, 0, filtermixer4, 1);
AudioConnection patchCord26(filterEnvelope, 0, filtermixer3, 1);
AudioConnection patchCord27(filterEnvelope, 0, filtermixer2, 1);
AudioConnection patchCord28(filterEnvelope, 0, filtermixer1, 1);
AudioConnection patchCord29(pitchmixer, 0, waveform1, 0);
AudioConnection patchCord30(pitchmixer, 0, waveform2, 0);
AudioConnection patchCord31(pitchmixer, 0, waveform3, 0);
AudioConnection patchCord32(pitchmixer, 0, waveform4, 0);
AudioConnection patchCord33(pitchmixer, 0, waveform5, 0);
AudioConnection patchCord34(pitchmixer, 0, waveform6, 0);
AudioConnection patchCord35(pitchmixer, 0, waveform7, 0);
AudioConnection patchCord36(pitchmixer, 0, waveform8, 0);
AudioConnection patchCord37(pitchmixer, 0, waveform9, 0);
AudioConnection patchCord38(pitchmixer, 0, waveform10, 0);
AudioConnection patchCord39(pitchmixer, 0, waveform11, 0);
AudioConnection patchCord40(pitchmixer, 0, waveform12, 0);
AudioConnection patchCord41(pitchmixer, 0, waveform14, 0);
AudioConnection patchCord42(pitchmixer, 0, waveform13, 0);
AudioConnection patchCord43(pitchmixer, 0, waveform15, 0);
AudioConnection patchCord44(pitchmixer, 0, waveform16, 0);
AudioConnection patchCord45(noise1, 0, noisemixer, 1);
AudioConnection patchCord46(pink1, 0, noisemixer, 0);
AudioConnection patchCord47(waveform2, 0, mixer9, 1);
AudioConnection patchCord48(waveform1, 0, mixer9, 0);
AudioConnection patchCord49(waveform8, 0, mixer12, 1);
AudioConnection patchCord50(waveform4, 0, mixer10, 1);
AudioConnection patchCord51(waveform11, 0, mixer14, 0);
AudioConnection patchCord52(waveform12, 0, mixer14, 1);
AudioConnection patchCord53(waveform14, 0, mixer15, 1);
AudioConnection patchCord54(waveform15, 0, mixer16, 0);
AudioConnection patchCord55(waveform16, 0, mixer16, 1);
AudioConnection patchCord56(waveform7, 0, mixer12, 0);
AudioConnection patchCord57(waveform3, 0, mixer10, 0);
AudioConnection patchCord58(waveform13, 0, mixer15, 0);
AudioConnection patchCord59(waveform10, 0, mixer13, 1);
AudioConnection patchCord60(waveform5, 0, mixer11, 0);
AudioConnection patchCord61(waveform6, 0, mixer11, 1);
AudioConnection patchCord62(waveform9, 0, mixer13, 0);
AudioConnection patchCord63(noisemixer, 0, mixer9, 2);
AudioConnection patchCord64(noisemixer, 0, mixer10, 2);
AudioConnection patchCord65(noisemixer, 0, mixer11, 2);
AudioConnection patchCord66(noisemixer, 0, mixer12, 2);
AudioConnection patchCord67(noisemixer, 0, mixer13, 2);
AudioConnection patchCord68(noisemixer, 0, mixer14, 2);
AudioConnection patchCord69(noisemixer, 0, mixer15, 2);
AudioConnection patchCord70(noisemixer, 0, mixer16, 2);
AudioConnection patchCord71(mixer9, 0, filter1, 0);
AudioConnection patchCord72(LFO_Filter, 0, filtermixer5, 0);
AudioConnection patchCord73(LFO_Filter, 0, filtermixer4, 0);
AudioConnection patchCord74(LFO_Filter, 0, filtermixer3, 0);
AudioConnection patchCord75(LFO_Filter, 0, filtermixer2, 0);
AudioConnection patchCord76(LFO_Filter, 0, filtermixer1, 0);
AudioConnection patchCord77(LFO_Filter, 0, filtermixer6, 0);
AudioConnection patchCord78(LFO_Filter, 0, filtermixer7, 0);
AudioConnection patchCord79(LFO_Filter, 0, filtermixer8, 0);
AudioConnection patchCord80(mixer11, 0, filter3, 0);
AudioConnection patchCord81(mixer10, 0, filter2, 0);
AudioConnection patchCord82(mixer12, 0, filter4, 0);
AudioConnection patchCord83(mixer13, 0, filter5, 0);
AudioConnection patchCord84(mixer14, 0, filter6, 0);
AudioConnection patchCord85(mixer15, 0, filter7, 0);
AudioConnection patchCord86(mixer16, 0, filter8, 0);
AudioConnection patchCord87(keytracking1, 0, filtermixer1, 2);
AudioConnection patchCord88(keytracking2, 0, filtermixer2, 2);
AudioConnection patchCord89(keytracking3, 0, filtermixer3, 2);
AudioConnection patchCord90(keytracking4, 0, filtermixer4, 2);
AudioConnection patchCord91(keytracking5, 0, filtermixer5, 2);
AudioConnection patchCord92(keytracking6, 0, filtermixer6, 2);
AudioConnection patchCord93(keytracking8, 0, filtermixer8, 2);
AudioConnection patchCord94(keytracking7, 0, filtermixer7, 2);
AudioConnection patchCord95(filtermixer2, 0, filter2, 1);
AudioConnection patchCord96(filtermixer3, 0, filter3, 1);
AudioConnection patchCord97(filtermixer1, 0, filter1, 1);
AudioConnection patchCord98(filtermixer5, 0, filter5, 1);
AudioConnection patchCord99(filtermixer4, 0, filter4, 1);
AudioConnection patchCord100(filtermixer6, 0, filter6, 1);
AudioConnection patchCord101(filtermixer7, 0, filter7, 1);
AudioConnection patchCord102(filtermixer8, 0, filter8, 1);
AudioConnection patchCord103(filter2, 0, mixer2, 0);
AudioConnection patchCord104(filter2, 1, mixer2, 1);
AudioConnection patchCord105(filter2, 2, mixer2, 2);
AudioConnection patchCord106(filter7, 0, mixer7, 0);
AudioConnection patchCord107(filter7, 1, mixer7, 1);
AudioConnection patchCord108(filter7, 2, mixer7, 2);
AudioConnection patchCord109(filter4, 0, mixer4, 0);
AudioConnection patchCord110(filter4, 1, mixer4, 1);
AudioConnection patchCord111(filter4, 2, mixer4, 2);
AudioConnection patchCord112(filter3, 0, mixer3, 0);
AudioConnection patchCord113(filter3, 1, mixer3, 1);
AudioConnection patchCord114(filter3, 2, mixer3, 2);
AudioConnection patchCord115(filter8, 0, mixer8, 0);
AudioConnection patchCord116(filter8, 1, mixer8, 1);
AudioConnection patchCord117(filter8, 2, mixer8, 2);
AudioConnection patchCord118(filter5, 0, mixer5, 0);
AudioConnection patchCord119(filter5, 1, mixer5, 1);
AudioConnection patchCord120(filter5, 2, mixer5, 2);
AudioConnection patchCord121(filter6, 0, mixer6, 0);
AudioConnection patchCord122(filter6, 1, mixer6, 1);
AudioConnection patchCord123(filter6, 2, mixer6, 2);
AudioConnection patchCord124(filter1, 0, mixer1, 0);
AudioConnection patchCord125(filter1, 1, mixer1, 1);
AudioConnection patchCord126(filter1, 2, mixer1, 2);
AudioConnection patchCord127(mixer1, envelope1);
AudioConnection patchCord128(mixer1, 0, envmixer1, 2);
AudioConnection patchCord129(mixer2, envelope2);
AudioConnection patchCord130(mixer2, 0, envmixer1, 3);
AudioConnection patchCord131(mixer3, envelope3);
AudioConnection patchCord132(mixer3, 0, envmixer2, 2);
AudioConnection patchCord133(mixer4, envelope4);
AudioConnection patchCord134(mixer4, 0, envmixer2, 3);
AudioConnection patchCord135(mixer5, envelope5);
AudioConnection patchCord136(mixer5, 0, envmixer3, 2);
AudioConnection patchCord137(mixer6, envelope6);
AudioConnection patchCord138(mixer6, 0, envmixer3, 3);
AudioConnection patchCord139(mixer7, envelope7);
AudioConnection patchCord140(mixer7, 0, envmixer4, 2);
AudioConnection patchCord141(mixer8, envelope8);
AudioConnection patchCord142(mixer8, 0, envmixer4, 3);
AudioConnection patchCord143(envelope5, 0, envmixer3, 0);
AudioConnection patchCord144(envelope2, 0, envmixer1, 1);
AudioConnection patchCord145(envelope3, 0, envmixer2, 0);
AudioConnection patchCord146(envelope1, 0, envmixer1, 0);
AudioConnection patchCord147(envelope4, 0, envmixer2, 1);
AudioConnection patchCord148(envelope6, 0, envmixer3, 1);
AudioConnection patchCord149(envelope8, 0, envmixer4, 1);
AudioConnection patchCord150(envelope7, 0, envmixer4, 0);
AudioConnection patchCord151(envmixer2, 0, mixerL, 1);
AudioConnection patchCord152(envmixer2, 0, mixerR, 1);
AudioConnection patchCord153(envmixer4, 0, mixerL, 3);
AudioConnection patchCord154(envmixer4, 0, mixerR, 3);
AudioConnection patchCord155(envmixer1, 0, mixerL, 0);
AudioConnection patchCord156(envmixer1, 0, mixerR, 0);
AudioConnection patchCord157(envmixer3, 0, mixerL, 2);
AudioConnection patchCord158(envmixer3, 0, mixerR, 2);
AudioConnection patchCord159(mixerL, flangerL);
AudioConnection patchCord160(mixerR, flangerR);
AudioConnection patchCord161(flangerR, 0, i2s1, 1);
AudioConnection patchCord162(flangerL, 0, i2s1, 0);
AudioControlSGTL5000 sgtl5000_1; //xy=915,200
// GUItool: end automatically generated code

#endif
