import("stdfaust.lib");

energy = hslider("energy",0,0,1,0.001) : si.smoo;

f_Hz = 220 + energy * RANGE_f_Hz;
gain = 0.5 + sqrt(energy) * RANGE_gain;
breathGain = energy * RANGE_breathGain;
drive = energy * RANGE_drive;
overblow = 0.5 + energy * RANGE_gain * RANGE_gain * 2;
  
RANGE_f_Hz = hslider("SUBPARAM_freq_Hz",440,0,1200,0.01) : si.smoo;
RANGE_gain = hslider("SUBPARAM_blowPressure",0.5,0,0.5,0.001) : si.smoo;
RANGE_breathGain = hslider("SUBPARAM_breathgain",100,0,100,0.001) : si.smoo;
RANGE_drive = hslider("SUBPARAM_drive",0.25,0,0.5,0.001) : si.smoo;
masterGain_dB = hslider("MasterGain",0.0,-40,0,0.001) : si.smoo : ba.db2linear;

//setTempo = hslider("tempo",500,400,1000,0.001);
//at = hslider("attack",0.001,0.001,0.1,0.001);
//rl = hslider("release",0.2,0.001,1,0.001);

mouthPos = 0.5;
vib = 0;

fluteSimple(f_Hz,gain,mouthPos,vib) = pm.fluteModel(tubeLength,mouthPos,blow) * gain * gain
with{
  	gate = (f_Hz > 200) : si.smooth(ba.tau2pole(0.02));
    envelope = gate*gain : si.smooth(ba.tau2pole(0.001));
  	freqDiff = f_Hz - 220;
	//freqFactor = 0.999999999999 + freqDiff * 0.0080951 - freqDiff*freqDiff * 0.00002777 + freqDiff*freqDiff*freqDiff * 0.00000004097;
    //tubeLength = 220 + (f_Hz - 220) * freqFactor : pm.f2l;
  	tubeLength = f_Hz : pm.f2l;
    pressure = envelope;
    vibFreq = 5 * (1 + 0.4 * ((vib - 0.036)/ 0.036));
    fCut_Hz = 1200 + gain * 2500;
    blow = pm.blower(pressure,0.05,fCut_Hz,vibFreq,vib);
};

trig = ba.pulse(ma.SR/setTempo * 60);

process = (fluteSimple(f_Hz,gain,overblow,vib) + breathGain * fluteSimple(f_Hz,0.5,0.5,0)) : ef.cubicnl(drive,0) * masterGain_dB <: _,_;
			