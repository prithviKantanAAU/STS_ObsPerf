/* ------------------------------------------------------------
name: "energyTest"
Code generated with Faust 2.30.8 (https://faust.grame.fr)
Compilation options: -lang cpp -es 1 -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

#include <cmath>
#include <cstring>

/************************** BEGIN dsp.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>
#include <vector>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'openTabBox', 'addButton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Return the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void init(int sample_rate) = 0;

        /**
         * Init instance state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceInit(int sample_rate) = 0;
    
        /**
         * Init instance constant state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceConstants(int sample_rate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (like delay lines...) but keep the control parameter values */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with successive in/out audio buffers.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         *
         */
        virtual void compute(double /*date_usec*/, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = nullptr):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int sample_rate) { fDSP->init(sample_rate); }
        virtual void instanceInit(int sample_rate) { fDSP->instanceInit(sample_rate); }
        virtual void instanceConstants(int sample_rate) { fDSP->instanceConstants(sample_rate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class, used with LLVM and Interpreter backends
 * to create DSP instances from a compiled DSP program.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
        virtual std::string getCompileOptions() = 0;
        virtual std::vector<std::string> getLibraryList() = 0;
        virtual std::vector<std::string> getIncludePathnames() = 0;
    
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/**************************  END  dsp.h **************************/
/************************** BEGIN MapUI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_MAPUI_H
#define FAUST_MAPUI_H

#include <vector>
#include <map>
#include <string>

/************************** BEGIN UI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2020 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __UI_H__
#define __UI_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

template <typename REAL>
struct UIReal
{
    UIReal() {}
    virtual ~UIReal() {}
    
    // -- widget's layouts
    
    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;
    
    // -- active widgets
    
    virtual void addButton(const char* label, REAL* zone) = 0;
    virtual void addCheckButton(const char* label, REAL* zone) = 0;
    virtual void addVerticalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addHorizontalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addNumEntry(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    
    // -- passive widgets
    
    virtual void addHorizontalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    virtual void addVerticalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    
    // -- soundfiles
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;
    
    // -- metadata declarations
    
    virtual void declare(REAL* zone, const char* key, const char* val) {}
};

struct UI : public UIReal<FAUSTFLOAT>
{
    UI() {}
    virtual ~UI() {}
};

#endif
/**************************  END  UI.h **************************/
/************************** BEGIN PathBuilder.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_PATHBUILDER_H
#define FAUST_PATHBUILDER_H

#include <vector>
#include <string>
#include <algorithm>

/*******************************************************************************
 * PathBuilder : Faust User Interface
 * Helper class to build complete hierarchical path for UI items.
 ******************************************************************************/

class PathBuilder
{

    protected:
    
        std::vector<std::string> fControlsLevel;
       
    public:
    
        PathBuilder() {}
        virtual ~PathBuilder() {}
    
        std::string buildPath(const std::string& label) 
        {
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res += fControlsLevel[i];
                res += "/";
            }
            res += label;
            std::replace(res.begin(), res.end(), ' ', '_');
            return res;
        }
    
        std::string buildLabel(std::string label)
        {
            std::replace(label.begin(), label.end(), ' ', '_');
            return label;
        }
    
        void pushLabel(const std::string& label) { fControlsLevel.push_back(label); }
        void popLabel() { fControlsLevel.pop_back(); }
    
};

#endif  // FAUST_PATHBUILDER_H
/**************************  END  PathBuilder.h **************************/

/*******************************************************************************
 * MapUI : Faust User Interface
 * This class creates a map of complete hierarchical path and zones for each UI items.
 ******************************************************************************/

class MapUI : public UI, public PathBuilder
{
    
    protected:
    
        // Complete path map
        std::map<std::string, FAUSTFLOAT*> fPathZoneMap;
    
        // Label zone map
        std::map<std::string, FAUSTFLOAT*> fLabelZoneMap;
    
        std::string fNullStr = "";
    
    public:
        
        MapUI() {}
        virtual ~MapUI() {}
        
        // -- widget's layouts
        void openTabBox(const char* label)
        {
            pushLabel(label);
        }
        void openHorizontalBox(const char* label)
        {
            pushLabel(label);
        }
        void openVerticalBox(const char* label)
        {
            pushLabel(label);
        }
        void closeBox()
        {
            popLabel();
        }
        
        // -- active widgets
        void addButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        
        // -- passive widgets
        void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
    
        // -- soundfiles
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
        
        // -- metadata declarations
        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {}
        
        // set/get
        void setParamValue(const std::string& path, FAUSTFLOAT value)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                *fPathZoneMap[path] = value;
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                *fLabelZoneMap[path] = value;
            }
        }
        
        FAUSTFLOAT getParamValue(const std::string& path)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                return *fPathZoneMap[path];
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                return *fLabelZoneMap[path];
            } else {
                return FAUSTFLOAT(0);
            }
        }
    
        // map access 
        std::map<std::string, FAUSTFLOAT*>& getMap() { return fPathZoneMap; }
        
        int getParamsCount() { return int(fPathZoneMap.size()); }
        
        const std::string& getParamAddress(int index)
        {
            if (index < 0 || index > int(fPathZoneMap.size())) {
                return fNullStr;
            } else {
                auto it = fPathZoneMap.begin();
                while (index-- > 0 && it++ != fPathZoneMap.end()) {}
                return it->first;
            }
        }
    
        const std::string& getParamAddress(FAUSTFLOAT* zone)
        {
            for (auto& it : fPathZoneMap) {
                if (it.second == zone) return it.first;
            }
            return fNullStr;
        }
    
        FAUSTFLOAT* getParamZone(const std::string& str)
        {
            if (fPathZoneMap.find(str) != fPathZoneMap.end()) {
                return fPathZoneMap[str];
            }
            if (fLabelZoneMap.find(str) != fLabelZoneMap.end()) {
                return fLabelZoneMap[str];
            }
            return nullptr;
        }
    
        FAUSTFLOAT* getParamZone(int index)
        {
            if (index < 0 || index > int(fPathZoneMap.size())) {
                return nullptr;
            } else {
                auto it = fPathZoneMap.begin();
                while (index-- > 0 && it++ != fPathZoneMap.end()) {}
                return it->second;
            }
        }
    
        static bool endsWith(const std::string& str, const std::string& end)
        {
            size_t l1 = str.length();
            size_t l2 = end.length();
            return (l1 >= l2) && (0 == str.compare(l1 - l2, l2, end));
        }
};


#endif // FAUST_MAPUI_H
/**************************  END  MapUI.h **************************/
/************************** BEGIN meta.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __meta__
#define __meta__

/**
 The base class of Meta handler to be used in dsp::metadata(Meta* m) method to retrieve (key, value) metadata.
 */
struct Meta
{
    virtual ~Meta() {};
    virtual void declare(const char* key, const char* value) = 0;
};

#endif
/**************************  END  meta.h **************************/

// BEGIN-FAUSTDSP

    // END-FAUSTDSP
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

static float mydsp_faustpower2_f(float value) {
	return (value * value);
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fHslider0;
	float fRec0[2];
	FAUSTFLOAT fHslider1;
	float fRec1[2];
	int iRec17[2];
	float fRec23[2];
	int fSampleRate;
	float fConst1;
	float fConst2;
	float fConst3;
	float fConst4;
	FAUSTFLOAT fHslider2;
	float fRec26[2];
	float fRec25[2];
	FAUSTFLOAT fHslider3;
	float fRec27[2];
	float fRec24[2];
	int iRec29[2];
	float fConst5;
	float fRec28[3];
	float fRec30[2];
	int IOTA;
	float fRec31[2048];
	float fConst6;
	float fConst7;
	float fVec0[2];
	float fVec1[2048];
	float fVec2[2];
	float fRec22[2];
	float fRec13[2048];
	float fRec9[2];
	float fRec5[2048];
	float fRec3[2];
	float fRec4[2];
	float fRec2[2];
	FAUSTFLOAT fHslider4;
	float fRec32[2];
	int iRec48[2];
	float fRec54[2];
	float fConst8;
	float fRec55[2];
	float fConst12;
	float fConst13;
	float fConst14;
	float fConst15;
	float fRec56[3];
	float fRec57[2];
	float fRec58[2048];
	float fConst16;
	float fVec3[2];
	float fVec4[2048];
	float fConst17;
	float fVec5[2];
	float fRec53[2];
	float fRec44[2048];
	float fRec40[2];
	float fRec36[2048];
	float fRec34[2];
	float fRec35[2];
	float fRec33[2];
	FAUSTFLOAT fHslider5;
	float fRec59[2];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("compile_options", "-lang cpp -es 1 -scal -ftz 0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("filename", "energyTest.dsp");
		m->declare("filters.lib/dcblocker:author", "Julius O. Smith III");
		m->declare("filters.lib/dcblocker:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/dcblocker:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/pole:author", "Julius O. Smith III");
		m->declare("filters.lib/pole:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/pole:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "0.3");
		m->declare("filters.lib/zero:author", "Julius O. Smith III");
		m->declare("filters.lib/zero:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/zero:license", "MIT-style STK-4.3 license");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.3");
		m->declare("misceffects.lib/name", "Misc Effects Library");
		m->declare("misceffects.lib/version", "2.0");
		m->declare("name", "energyTest");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "0.0");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.1");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.1");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "0.2");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch ((channel)) {
			case 0: {
				rate = 1;
				break;
			}
			case 1: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		float fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = std::exp((0.0f - (1000.0f / fConst0)));
		fConst2 = (1.0f - fConst1);
		fConst3 = std::exp((0.0f - (50.0f / fConst0)));
		fConst4 = (1.0f - fConst3);
		fConst5 = (3.14159274f / fConst0);
		fConst6 = (0.00882352982f * fConst0);
		fConst7 = (0.00147058826f * fConst0);
		fConst8 = (0.5f * fConst2);
		float fConst9 = std::tan((7696.90186f / fConst0));
		float fConst10 = (1.0f / fConst9);
		float fConst11 = (((fConst10 + 1.41421354f) / fConst9) + 1.0f);
		fConst12 = (0.0500000007f / fConst11);
		fConst13 = (1.0f / fConst11);
		fConst14 = (((fConst10 + -1.41421354f) / fConst9) + 1.0f);
		fConst15 = (2.0f * (1.0f - (1.0f / mydsp_faustpower2_f(fConst9))));
		fConst16 = (0.000397058815f * fConst0);
		fConst17 = (0.00107352936f * fConst0);
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.0f);
		fHslider1 = FAUSTFLOAT(0.25f);
		fHslider2 = FAUSTFLOAT(440.0f);
		fHslider3 = FAUSTFLOAT(0.5f);
		fHslider4 = FAUSTFLOAT(100.0f);
		fHslider5 = FAUSTFLOAT(0.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			fRec0[l0] = 0.0f;
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec1[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			iRec17[l2] = 0;
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec23[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec26[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec25[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec27[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec24[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			iRec29[l8] = 0;
		}
		for (int l9 = 0; (l9 < 3); l9 = (l9 + 1)) {
			fRec28[l9] = 0.0f;
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec30[l10] = 0.0f;
		}
		IOTA = 0;
		for (int l11 = 0; (l11 < 2048); l11 = (l11 + 1)) {
			fRec31[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fVec0[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 2048); l13 = (l13 + 1)) {
			fVec1[l13] = 0.0f;
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fVec2[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec22[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 2048); l16 = (l16 + 1)) {
			fRec13[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec9[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 2048); l18 = (l18 + 1)) {
			fRec5[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fRec3[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec4[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec2[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec32[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			iRec48[l23] = 0;
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec54[l24] = 0.0f;
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec55[l25] = 0.0f;
		}
		for (int l26 = 0; (l26 < 3); l26 = (l26 + 1)) {
			fRec56[l26] = 0.0f;
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			fRec57[l27] = 0.0f;
		}
		for (int l28 = 0; (l28 < 2048); l28 = (l28 + 1)) {
			fRec58[l28] = 0.0f;
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fVec3[l29] = 0.0f;
		}
		for (int l30 = 0; (l30 < 2048); l30 = (l30 + 1)) {
			fVec4[l30] = 0.0f;
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			fVec5[l31] = 0.0f;
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec53[l32] = 0.0f;
		}
		for (int l33 = 0; (l33 < 2048); l33 = (l33 + 1)) {
			fRec44[l33] = 0.0f;
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fRec40[l34] = 0.0f;
		}
		for (int l35 = 0; (l35 < 2048); l35 = (l35 + 1)) {
			fRec36[l35] = 0.0f;
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fRec34[l36] = 0.0f;
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec35[l37] = 0.0f;
		}
		for (int l38 = 0; (l38 < 2); l38 = (l38 + 1)) {
			fRec33[l38] = 0.0f;
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec59[l39] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("energyTest");
		ui_interface->addHorizontalSlider("MasterGain", &fHslider5, 0.0f, -40.0f, 0.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("SUBPARAM_blowPressure", &fHslider3, 0.5f, 0.0f, 0.5f, 0.00100000005f);
		ui_interface->addHorizontalSlider("SUBPARAM_breathgain", &fHslider4, 100.0f, 0.0f, 100.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("SUBPARAM_drive", &fHslider1, 0.25f, 0.0f, 0.5f, 0.00100000005f);
		ui_interface->addHorizontalSlider("SUBPARAM_freq_Hz", &fHslider2, 440.0f, 0.0f, 1200.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("energy", &fHslider0, 0.0f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = (0.00100000005f * float(fHslider0));
		float fSlow1 = (0.00100000005f * float(fHslider1));
		float fSlow2 = (0.00100000005f * float(fHslider2));
		float fSlow3 = (0.00100000005f * float(fHslider3));
		float fSlow4 = (0.00100000005f * float(fHslider4));
		float fSlow5 = (0.00100000005f * float(fHslider5));
		for (int i = 0; (i < count); i = (i + 1)) {
			fRec0[0] = (fSlow0 + (0.999000013f * fRec0[1]));
			fRec1[0] = (fSlow1 + (0.999000013f * fRec1[1]));
			iRec17[0] = 0;
			fRec23[0] = ((0.699999988f * fRec23[1]) + (0.284999996f * fRec22[1]));
			float fRec21 = (fRec23[0] + float(iRec17[1]));
			fRec26[0] = (fSlow2 + (0.999000013f * fRec26[1]));
			float fTemp0 = ((fRec0[0] * fRec26[0]) + 220.0f);
			fRec25[0] = ((fConst3 * fRec25[1]) + (fConst4 * float((fTemp0 > 200.0f))));
			fRec27[0] = (fSlow3 + (0.999000013f * fRec27[1]));
			float fTemp1 = ((std::sqrt(fRec0[0]) * fRec27[0]) + 0.5f);
			fRec24[0] = ((fConst1 * fRec24[1]) + (fConst2 * (fRec25[0] * fTemp1)));
			iRec29[0] = ((1103515245 * iRec29[1]) + 12345);
			float fTemp2 = (4.65661287e-10f * float(iRec29[0]));
			float fTemp3 = std::tan((fConst5 * ((2500.0f * fTemp1) + 1200.0f)));
			float fTemp4 = (1.0f / fTemp3);
			float fTemp5 = (((fTemp4 + 1.41421354f) / fTemp3) + 1.0f);
			fRec28[0] = (fTemp2 - (((fRec28[2] * (((fTemp4 + -1.41421354f) / fTemp3) + 1.0f)) + (2.0f * (fRec28[1] * (1.0f - (1.0f / mydsp_faustpower2_f(fTemp3)))))) / fTemp5));
			fRec30[0] = fRec3[1];
			fRec31[(IOTA & 2047)] = (0.949999988f * fRec30[1]);
			float fTemp6 = ((340.0f / fTemp0) + 0.270000011f);
			float fTemp7 = (0.800000012f * (fRec0[0] * mydsp_faustpower2_f(fRec27[0])));
			float fTemp8 = (fConst7 * (fTemp6 * (fTemp7 + 0.270000011f)));
			float fTemp9 = (fTemp8 + -1.49999499f);
			int iTemp10 = int(fTemp9);
			int iTemp11 = (int(std::min<float>(fConst6, float(std::max<int>(0, iTemp10)))) + 1);
			float fTemp12 = std::floor(fTemp9);
			float fTemp13 = (fTemp8 + (-1.0f - fTemp12));
			float fTemp14 = (0.0f - fTemp13);
			float fTemp15 = (fTemp8 + (-2.0f - fTemp12));
			float fTemp16 = (0.0f - (0.5f * fTemp15));
			float fTemp17 = (fTemp8 + (-3.0f - fTemp12));
			float fTemp18 = (0.0f - (0.333333343f * fTemp17));
			float fTemp19 = (fTemp8 + (-4.0f - fTemp12));
			float fTemp20 = (0.0f - (0.25f * fTemp19));
			float fTemp21 = (fTemp8 - fTemp12);
			int iTemp22 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp10 + 1))))) + 1);
			float fTemp23 = (0.0f - fTemp15);
			float fTemp24 = (0.0f - (0.5f * fTemp17));
			float fTemp25 = (0.0f - (0.333333343f * fTemp19));
			int iTemp26 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp10 + 2))))) + 1);
			float fTemp27 = (0.0f - fTemp17);
			float fTemp28 = (0.0f - (0.5f * fTemp19));
			float fTemp29 = (fTemp13 * fTemp15);
			int iTemp30 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp10 + 3))))) + 1);
			float fTemp31 = (0.0f - fTemp19);
			float fTemp32 = (fTemp29 * fTemp17);
			int iTemp33 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp10 + 4))))) + 1);
			fVec0[0] = (((((fRec31[((IOTA - iTemp11) & 2047)] * fTemp14) * fTemp16) * fTemp18) * fTemp20) + (fTemp21 * ((((((fRec31[((IOTA - iTemp22) & 2047)] * fTemp23) * fTemp24) * fTemp25) + (0.5f * (((fTemp13 * fRec31[((IOTA - iTemp26) & 2047)]) * fTemp27) * fTemp28))) + (0.166666672f * ((fTemp29 * fRec31[((IOTA - iTemp30) & 2047)]) * fTemp31))) + (0.0416666679f * (fTemp32 * fRec31[((IOTA - iTemp33) & 2047)])))));
			float fTemp34 = ((fRec24[0] * ((0.0500000007f * ((fRec28[2] + (fRec28[0] + (2.0f * fRec28[1]))) / fTemp5)) + 1.0f)) - (0.5f * fVec0[1]));
			float fTemp35 = ((0.5f * fRec9[1]) + std::max<float>(-1.0f, std::min<float>(1.0f, (fTemp34 * (mydsp_faustpower2_f(fTemp34) + -1.0f)))));
			fVec1[(IOTA & 2047)] = fTemp35;
			float fTemp36 = (fConst7 * (fTemp6 * (0.730000019f - fTemp7)));
			float fTemp37 = (fTemp36 + -1.49999499f);
			int iTemp38 = int(fTemp37);
			int iTemp39 = (int(std::min<float>(fConst6, float(std::max<int>(0, iTemp38)))) + 1);
			float fTemp40 = std::floor(fTemp37);
			float fTemp41 = (fTemp36 + (-1.0f - fTemp40));
			float fTemp42 = (0.0f - fTemp41);
			float fTemp43 = (fTemp36 + (-2.0f - fTemp40));
			float fTemp44 = (0.0f - (0.5f * fTemp43));
			float fTemp45 = (fTemp36 + (-3.0f - fTemp40));
			float fTemp46 = (0.0f - (0.333333343f * fTemp45));
			float fTemp47 = (fTemp36 + (-4.0f - fTemp40));
			float fTemp48 = (0.0f - (0.25f * fTemp47));
			float fTemp49 = (fTemp36 - fTemp40);
			int iTemp50 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp38 + 1))))) + 1);
			float fTemp51 = (0.0f - fTemp43);
			float fTemp52 = (0.0f - (0.5f * fTemp45));
			float fTemp53 = (0.0f - (0.333333343f * fTemp47));
			int iTemp54 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp38 + 2))))) + 1);
			float fTemp55 = (0.0f - fTemp45);
			float fTemp56 = (0.0f - (0.5f * fTemp47));
			float fTemp57 = (fTemp41 * fTemp43);
			int iTemp58 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp38 + 3))))) + 1);
			float fTemp59 = (0.0f - fTemp47);
			float fTemp60 = (fTemp57 * fTemp45);
			int iTemp61 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp38 + 4))))) + 1);
			fVec2[0] = (((((fVec1[((IOTA - iTemp39) & 2047)] * fTemp42) * fTemp44) * fTemp46) * fTemp48) + (fTemp49 * ((((((fVec1[((IOTA - iTemp50) & 2047)] * fTemp51) * fTemp52) * fTemp53) + (0.5f * (((fTemp41 * fVec1[((IOTA - iTemp54) & 2047)]) * fTemp55) * fTemp56))) + (0.166666672f * ((fTemp57 * fVec1[((IOTA - iTemp58) & 2047)]) * fTemp59))) + (0.0416666679f * (fTemp60 * fVec1[((IOTA - iTemp61) & 2047)])))));
			fRec22[0] = fVec2[1];
			float fRec18 = fRec21;
			float fRec19 = fRec22[0];
			float fRec20 = fRec22[0];
			fRec13[(IOTA & 2047)] = fRec18;
			float fRec14 = (((((fTemp42 * fTemp44) * fTemp46) * fTemp48) * fRec13[((IOTA - iTemp39) & 2047)]) + (fTemp49 * ((((((fTemp51 * fTemp52) * fTemp53) * fRec13[((IOTA - iTemp50) & 2047)]) + (0.5f * (((fTemp41 * fTemp55) * fTemp56) * fRec13[((IOTA - iTemp54) & 2047)]))) + (0.166666672f * ((fTemp57 * fTemp59) * fRec13[((IOTA - iTemp58) & 2047)]))) + (0.0416666679f * (fTemp60 * fRec13[((IOTA - iTemp61) & 2047)])))));
			float fRec15 = fRec19;
			float fRec16 = fRec20;
			fRec9[0] = fRec14;
			float fRec10 = fRec9[1];
			float fRec11 = fRec15;
			float fRec12 = fRec16;
			fRec5[(IOTA & 2047)] = fRec10;
			float fRec6 = (((((fTemp14 * fTemp16) * fTemp18) * fTemp20) * fRec5[((IOTA - iTemp11) & 2047)]) + (fTemp21 * ((((((fTemp23 * fTemp24) * fTemp25) * fRec5[((IOTA - iTemp22) & 2047)]) + (0.5f * (((fTemp13 * fTemp27) * fTemp28) * fRec5[((IOTA - iTemp26) & 2047)]))) + (0.166666672f * ((fTemp29 * fTemp31) * fRec5[((IOTA - iTemp30) & 2047)]))) + (0.0416666679f * (fTemp32 * fRec5[((IOTA - iTemp33) & 2047)])))));
			float fRec7 = fRec11;
			float fRec8 = fRec12;
			fRec3[0] = fRec6;
			fRec4[0] = fRec8;
			fRec2[0] = ((fRec4[0] + (0.995000005f * fRec2[1])) - fRec4[1]);
			fRec32[0] = (fSlow4 + (0.999000013f * fRec32[1]));
			iRec48[0] = 0;
			fRec54[0] = ((0.699999988f * fRec54[1]) + (0.284999996f * fRec53[1]));
			float fRec52 = (fRec54[0] + float(iRec48[1]));
			fRec55[0] = ((fConst1 * fRec55[1]) + (fConst8 * fRec25[0]));
			fRec56[0] = (fTemp2 - (fConst13 * ((fConst14 * fRec56[2]) + (fConst15 * fRec56[1]))));
			fRec57[0] = fRec34[1];
			fRec58[(IOTA & 2047)] = (0.949999988f * fRec57[1]);
			float fTemp62 = (fConst16 * fTemp6);
			float fTemp63 = (fTemp62 + -1.49999499f);
			int iTemp64 = int(fTemp63);
			int iTemp65 = (int(std::min<float>(fConst6, float(std::max<int>(0, iTemp64)))) + 1);
			float fTemp66 = std::floor(fTemp63);
			float fTemp67 = (fTemp62 + (-1.0f - fTemp66));
			float fTemp68 = (0.0f - fTemp67);
			float fTemp69 = (fTemp62 + (-2.0f - fTemp66));
			float fTemp70 = (0.0f - (0.5f * fTemp69));
			float fTemp71 = (fTemp62 + (-3.0f - fTemp66));
			float fTemp72 = (0.0f - (0.333333343f * fTemp71));
			float fTemp73 = (fTemp62 + (-4.0f - fTemp66));
			float fTemp74 = (0.0f - (0.25f * fTemp73));
			float fTemp75 = (fTemp62 - fTemp66);
			int iTemp76 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp64 + 1))))) + 1);
			float fTemp77 = (0.0f - fTemp69);
			float fTemp78 = (0.0f - (0.5f * fTemp71));
			float fTemp79 = (0.0f - (0.333333343f * fTemp73));
			int iTemp80 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp64 + 2))))) + 1);
			float fTemp81 = (0.0f - fTemp71);
			float fTemp82 = (0.0f - (0.5f * fTemp73));
			float fTemp83 = (fTemp67 * fTemp69);
			int iTemp84 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp64 + 3))))) + 1);
			float fTemp85 = (0.0f - fTemp73);
			float fTemp86 = (fTemp83 * fTemp71);
			int iTemp87 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp64 + 4))))) + 1);
			fVec3[0] = (((((fRec58[((IOTA - iTemp65) & 2047)] * fTemp68) * fTemp70) * fTemp72) * fTemp74) + (fTemp75 * ((((((fRec58[((IOTA - iTemp76) & 2047)] * fTemp77) * fTemp78) * fTemp79) + (0.5f * (((fTemp67 * fRec58[((IOTA - iTemp80) & 2047)]) * fTemp81) * fTemp82))) + (0.166666672f * ((fTemp83 * fRec58[((IOTA - iTemp84) & 2047)]) * fTemp85))) + (0.0416666679f * (fTemp86 * fRec58[((IOTA - iTemp87) & 2047)])))));
			float fTemp88 = ((fRec55[0] * ((fConst12 * (fRec56[2] + (fRec56[0] + (2.0f * fRec56[1])))) + 1.0f)) - (0.5f * fVec3[1]));
			float fTemp89 = ((0.5f * fRec40[1]) + std::max<float>(-1.0f, std::min<float>(1.0f, (fTemp88 * (mydsp_faustpower2_f(fTemp88) + -1.0f)))));
			fVec4[(IOTA & 2047)] = fTemp89;
			float fTemp90 = (fConst17 * fTemp6);
			float fTemp91 = (fTemp90 + -1.49999499f);
			int iTemp92 = int(fTemp91);
			int iTemp93 = (int(std::min<float>(fConst6, float(std::max<int>(0, iTemp92)))) + 1);
			float fTemp94 = std::floor(fTemp91);
			float fTemp95 = (fTemp90 + (-1.0f - fTemp94));
			float fTemp96 = (0.0f - fTemp95);
			float fTemp97 = (fTemp90 + (-2.0f - fTemp94));
			float fTemp98 = (0.0f - (0.5f * fTemp97));
			float fTemp99 = (fTemp90 + (-3.0f - fTemp94));
			float fTemp100 = (0.0f - (0.333333343f * fTemp99));
			float fTemp101 = (fTemp90 + (-4.0f - fTemp94));
			float fTemp102 = (0.0f - (0.25f * fTemp101));
			float fTemp103 = (fTemp90 - fTemp94);
			int iTemp104 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp92 + 1))))) + 1);
			float fTemp105 = (0.0f - fTemp97);
			float fTemp106 = (0.0f - (0.5f * fTemp99));
			float fTemp107 = (0.0f - (0.333333343f * fTemp101));
			int iTemp108 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp92 + 2))))) + 1);
			float fTemp109 = (0.0f - fTemp99);
			float fTemp110 = (0.0f - (0.5f * fTemp101));
			float fTemp111 = (fTemp95 * fTemp97);
			int iTemp112 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp92 + 3))))) + 1);
			float fTemp113 = (0.0f - fTemp101);
			float fTemp114 = (fTemp111 * fTemp99);
			int iTemp115 = (int(std::min<float>(fConst6, float(std::max<int>(0, (iTemp92 + 4))))) + 1);
			fVec5[0] = (((((fVec4[((IOTA - iTemp93) & 2047)] * fTemp96) * fTemp98) * fTemp100) * fTemp102) + (fTemp103 * ((((((fVec4[((IOTA - iTemp104) & 2047)] * fTemp105) * fTemp106) * fTemp107) + (0.5f * (((fTemp95 * fVec4[((IOTA - iTemp108) & 2047)]) * fTemp109) * fTemp110))) + (0.166666672f * ((fTemp111 * fVec4[((IOTA - iTemp112) & 2047)]) * fTemp113))) + (0.0416666679f * (fTemp114 * fVec4[((IOTA - iTemp115) & 2047)])))));
			fRec53[0] = fVec5[1];
			float fRec49 = fRec52;
			float fRec50 = fRec53[0];
			float fRec51 = fRec53[0];
			fRec44[(IOTA & 2047)] = fRec49;
			float fRec45 = (((((fTemp96 * fTemp98) * fTemp100) * fTemp102) * fRec44[((IOTA - iTemp93) & 2047)]) + (fTemp103 * ((((((fTemp105 * fTemp106) * fTemp107) * fRec44[((IOTA - iTemp104) & 2047)]) + (0.5f * (((fTemp95 * fTemp109) * fTemp110) * fRec44[((IOTA - iTemp108) & 2047)]))) + (0.166666672f * ((fTemp111 * fTemp113) * fRec44[((IOTA - iTemp112) & 2047)]))) + (0.0416666679f * (fTemp114 * fRec44[((IOTA - iTemp115) & 2047)])))));
			float fRec46 = fRec50;
			float fRec47 = fRec51;
			fRec40[0] = fRec45;
			float fRec41 = fRec40[1];
			float fRec42 = fRec46;
			float fRec43 = fRec47;
			fRec36[(IOTA & 2047)] = fRec41;
			float fRec37 = (((((fTemp68 * fTemp70) * fTemp72) * fTemp74) * fRec36[((IOTA - iTemp65) & 2047)]) + (fTemp75 * ((((((fTemp77 * fTemp78) * fTemp79) * fRec36[((IOTA - iTemp76) & 2047)]) + (0.5f * (((fTemp67 * fTemp81) * fTemp82) * fRec36[((IOTA - iTemp80) & 2047)]))) + (0.166666672f * ((fTemp83 * fTemp85) * fRec36[((IOTA - iTemp84) & 2047)]))) + (0.0416666679f * (fTemp86 * fRec36[((IOTA - iTemp87) & 2047)])))));
			float fRec38 = fRec42;
			float fRec39 = fRec43;
			fRec34[0] = fRec37;
			fRec35[0] = fRec39;
			fRec33[0] = ((fRec35[0] + (0.995000005f * fRec33[1])) - fRec35[1]);
			float fTemp116 = std::max<float>(-1.0f, std::min<float>(1.0f, (std::pow(10.0f, (2.0f * (fRec0[0] * fRec1[0]))) * ((fRec2[0] * mydsp_faustpower2_f(fTemp1)) + (0.25f * ((fRec0[0] * fRec32[0]) * fRec33[0]))))));
			fRec59[0] = (fSlow5 + (0.999000013f * fRec59[1]));
			float fTemp117 = ((fTemp116 * (1.0f - (0.333333343f * mydsp_faustpower2_f(fTemp116)))) * std::pow(10.0f, (0.0500000007f * fRec59[0])));
			output0[i] = FAUSTFLOAT(fTemp117);
			output1[i] = FAUSTFLOAT(fTemp117);
			fRec0[1] = fRec0[0];
			fRec1[1] = fRec1[0];
			iRec17[1] = iRec17[0];
			fRec23[1] = fRec23[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fRec27[1] = fRec27[0];
			fRec24[1] = fRec24[0];
			iRec29[1] = iRec29[0];
			fRec28[2] = fRec28[1];
			fRec28[1] = fRec28[0];
			fRec30[1] = fRec30[0];
			IOTA = (IOTA + 1);
			fVec0[1] = fVec0[0];
			fVec2[1] = fVec2[0];
			fRec22[1] = fRec22[0];
			fRec9[1] = fRec9[0];
			fRec3[1] = fRec3[0];
			fRec4[1] = fRec4[0];
			fRec2[1] = fRec2[0];
			fRec32[1] = fRec32[0];
			iRec48[1] = iRec48[0];
			fRec54[1] = fRec54[0];
			fRec55[1] = fRec55[0];
			fRec56[2] = fRec56[1];
			fRec56[1] = fRec56[0];
			fRec57[1] = fRec57[0];
			fVec3[1] = fVec3[0];
			fVec5[1] = fVec5[0];
			fRec53[1] = fRec53[0];
			fRec40[1] = fRec40[0];
			fRec34[1] = fRec34[0];
			fRec35[1] = fRec35[0];
			fRec33[1] = fRec33[0];
			fRec59[1] = fRec59[0];
		}
	}

};

#endif
