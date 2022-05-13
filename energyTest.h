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

const static float fmydspSIG0Wave0[350] = {0.525285006f,0.814173996f,0.483260989f,0.296745002f,0.975055993f,0.472243994f,0.409500986f,0.425363988f,0.687559009f,0.28838101f,0.309285015f,0.123053998f,0.286332995f,0.576705992f,0.908321977f,0.626973987f,0.0801851973f,0.309834987f,0.451429993f,0.132844999f,0.470634997f,0.417008013f,0.265112013f,0.075280197f,0.463470012f,0.471810013f,0.275323987f,0.547026992f,0.512519002f,0.394077986f,0.595404029f,0.941305995f,0.392500997f,0.381435007f,0.391232014f,0.118923999f,0.339495003f,0.101420999f,0.241754994f,0.0873254985f,0.37894401f,0.637705028f,0.171946004f,0.149858996f,0.233290002f,0.541809976f,1.0f,0.115553997f,0.244172007f,0.574329019f,0.606171012f,0.93839699f,0.392554998f,0.277359009f,0.86857003f,0.432489008f,0.408856004f,0.407932013f,0.299814999f,0.256659001f,0.549571991f,0.406347007f,0.312330991f,0.62757802f,0.670167029f,0.524648011f,0.406926006f,0.637524009f,0.555836976f,1.0f,0.818979025f,0.705347002f,0.678140998f,0.427381992f,0.674404025f,0.636105001f,0.643634975f,0.699136019f,0.836201012f,0.613084972f,0.31901899f,0.725259006f,0.545518994f,0.479860991f,0.498360008f,0.488653988f,0.861671984f,0.314287007f,0.671051979f,0.531904995f,0.421781003f,0.81506598f,0.772032022f,0.488721997f,0.0896674022f,0.291285992f,0.65872997f,0.635631979f,0.679356992f,0.459497005f,0.360240012f,0.58228898f,0.650605023f,0.490949988f,0.381909996f,0.157260999f,0.479624003f,0.477490991f,0.174435005f,0.0130939996f,0.879113019f,0.608069003f,0.268877f,0.604479015f,0.245130002f,0.170506999f,0.292887986f,0.545849025f,0.476646006f,0.922316015f,0.669192016f,0.578094006f,0.578796983f,0.311396003f,0.601209998f,0.54995501f,1.0f,0.66573f,0.980114996f,0.537847996f,0.0740531012f,0.252472013f,0.25575f,0.223974004f,0.0865103006f,0.138209f,0.198623002f,0.0453034006f,0.432453007f,0.292407006f,0.394410014f,0.857658982f,0.271667987f,0.201545f,0.583993971f,0.0602377988f,0.190617993f,0.849505007f,0.975542009f,0.173140004f,0.206471995f,0.344792992f,0.761011004f,0.558125019f,0.117245004f,0.0338485017f,0.337597013f,0.336645991f,0.174253002f,0.230169997f,0.934872985f,0.593647003f,0.393225014f,0.683704019f,0.056609299f,0.0405011997f,0.148972005f,0.338721991f,0.283419013f,0.394006997f,0.237474993f,0.26996401f,0.428312987f,0.177498996f,0.462585002f,0.443962991f,0.981792986f,0.408239007f,0.676527023f,0.402864993f,0.0163302999f,0.0515113994f,0.341390014f,0.311134994f,0.613276005f,0.805884004f,0.953289986f,0.406091005f,0.578705013f,0.386785001f,0.434103012f,0.775259972f,1.0f,0.635909021f,0.78205198f,0.0137182996f,0.0387725011f,0.618964016f,0.857070982f,0.131522f,0.184988007f,0.299495012f,0.789211988f,0.603114009f,0.0704988986f,0.0129338996f,0.252481014f,0.254121006f,0.189206004f,0.357713014f,0.950308025f,0.552573025f,0.466453999f,0.777360022f,0.0307886004f,0.0251943003f,0.378886014f,0.740186989f,0.247637004f,0.235201001f,0.493045002f,0.517849982f,0.883953989f,0.429473013f,0.409433007f,0.415266007f,0.940198004f,0.282334f,0.437889993f,0.375384986f,0.0157366004f,0.0171763003f,0.485554993f,0.461014986f,0.858958006f,0.907990992f,0.935190976f,0.375510007f,1.0f,0.585493028f,0.269980997f,0.423052996f,0.666067004f,0.435090005f,0.79025197f,0.00889586005f,0.0208844002f,0.449734986f,0.790808022f,0.159856007f,0.0895989984f,0.161546007f,0.528168023f,0.380641997f,0.0206275992f,0.0072642602f,0.0315352008f,0.0315840989f,0.197649002f,0.475057006f,0.517232001f,0.360922009f,0.421204001f,0.631340027f,0.00952139031f,0.0161049999f,0.499615014f,0.922958016f,0.214983001f,0.0655141026f,0.503970027f,0.514847994f,1.0f,0.483619004f,0.254027009f,0.228371993f,0.436105013f,0.233125001f,0.152242005f,0.279513001f,0.00906739011f,0.0132331997f,0.45125699f,0.388565987f,0.737226009f,0.479378015f,0.233036995f,0.103767f,0.845609009f,0.644127011f,0.261359006f,0.371457011f,0.527229011f,0.381372988f,0.334491998f,0.00833749026f,0.00861981977f,0.255919009f,0.254197001f,0.0872332975f,0.0461511984f,0.113017999f,0.345986009f,0.236343995f,0.0107800001f,0.00816505961f,0.405180991f,0.387180001f,0.343681008f,0.816492975f,0.25908199f,0.211906001f,0.432455003f,0.696886003f,0.00576223992f,0.0131310001f,0.455969006f,0.81160903f,0.426544011f,0.128489003f,0.215937003f,0.233934f,0.723070025f,0.351622999f,0.394230992f,0.323765993f,0.168803006f,0.276932001f,0.264683992f,0.227703005f,0.0068093501f,0.0170703009f,0.603017986f,0.476460993f,0.585924983f,0.716960013f,1.0f,0.576526999f,0.475524008f,0.447322011f,0.356902003f,0.597572982f,0.697246015f,0.505333006f,0.285421014f,0.0147193f,0.0141618f,0.136188f,0.0336536989f,0.216436997f};
class mydspSIG0 {
	
  private:
	
	int fmydspSIG0Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG0() {
		return 0;
	}
	int getNumOutputsmydspSIG0() {
		return 1;
	}
	int getInputRatemydspSIG0(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	int getOutputRatemydspSIG0(int channel) {
		int rate;
		switch ((channel)) {
			case 0: {
				rate = 0;
				break;
			}
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	
	void instanceInitmydspSIG0(int sample_rate) {
		fmydspSIG0Wave0_idx = 0;
	}
	
	void fillmydspSIG0(int count, float* table) {
		for (int i = 0; (i < count); i = (i + 1)) {
			table[i] = fmydspSIG0Wave0[fmydspSIG0Wave0_idx];
			fmydspSIG0Wave0_idx = ((1 + fmydspSIG0Wave0_idx) % 350);
		}
	}

};

static mydspSIG0* newmydspSIG0() { return (mydspSIG0*)new mydspSIG0(); }
static void deletemydspSIG0(mydspSIG0* dsp) { delete dsp; }

static float mydsp_faustpower2_f(float value) {
	return (value * value);
}
static float ftbl0mydspSIG0[350];

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	int iVec0[2];
	FAUSTFLOAT fHslider0;
	float fRec0[2];
	FAUSTFLOAT fHslider1;
	float fRec1[2];
	int fSampleRate;
	float fConst1;
	float fConst2;
	FAUSTFLOAT fHslider2;
	float fRec4[2];
	float fRec3[2];
	float fVec1[2];
	int IOTA;
	float fVec2[4096];
	float fConst3;
	float fRec2[2];
	FAUSTFLOAT fHslider3;
	float fRec5[2];
	int iRec21[2];
	float fRec27[2];
	float fConst4;
	float fConst5;
	float fConst6;
	float fConst7;
	float fConst8;
	float fRec29[2];
	float fRec28[2];
	float fConst12;
	int iRec31[2];
	float fConst13;
	float fConst14;
	float fConst15;
	float fRec30[3];
	float fRec32[2];
	float fRec33[2048];
	float fConst16;
	float fConst17;
	float fVec3[2];
	float fVec4[2048];
	float fConst18;
	float fVec5[2];
	float fRec26[2];
	float fRec17[2048];
	float fRec13[2];
	float fRec9[2048];
	float fRec7[2];
	float fRec8[2];
	float fRec6[2];
	int iRec49[2];
	float fRec55[2];
	FAUSTFLOAT fHslider4;
	float fRec57[2];
	float fRec56[2];
	float fConst19;
	float fRec58[3];
	float fRec59[2];
	float fRec60[2048];
	float fConst20;
	float fVec6[2];
	float fVec7[2048];
	float fVec8[2];
	float fRec54[2];
	float fRec45[2048];
	float fRec41[2];
	float fRec37[2048];
	float fRec35[2];
	float fRec36[2];
	float fRec34[2];
	float fConst21;
	float fConst24;
	float fConst27;
	float fConst29;
	float fConst30;
	float fConst31;
	float fRec63[3];
	float fConst32;
	float fConst33;
	float fConst34;
	float fRec62[3];
	float fConst35;
	FAUSTFLOAT fHslider5;
	float fVec9[2];
	int iRec64[2];
	float fConst37;
	float fConst38;
	float fRec61[3];
	float fConst39;
	float fConst41;
	float fConst42;
	float fRec65[3];
	float fConst43;
	float fConst45;
	float fConst46;
	float fRec66[3];
	float fConst47;
	float fConst49;
	float fConst50;
	float fRec67[3];
	float fConst51;
	float fConst53;
	float fConst54;
	float fRec68[3];
	float fConst55;
	float fConst57;
	float fConst58;
	float fRec69[3];
	float fConst59;
	float fConst61;
	float fConst62;
	float fRec70[3];
	float fConst63;
	float fConst65;
	float fConst66;
	float fRec71[3];
	float fConst67;
	float fConst69;
	float fConst70;
	float fRec72[3];
	float fConst71;
	float fConst73;
	float fConst74;
	float fRec73[3];
	float fConst75;
	float fConst77;
	float fConst78;
	float fRec74[3];
	float fConst79;
	float fConst81;
	float fConst82;
	float fRec75[3];
	float fConst83;
	float fConst85;
	float fConst86;
	float fRec76[3];
	float fConst87;
	float fConst89;
	float fConst90;
	float fRec77[3];
	float fConst91;
	float fConst93;
	float fConst94;
	float fRec78[3];
	float fConst95;
	float fConst97;
	float fConst98;
	float fRec79[3];
	float fConst99;
	float fConst101;
	float fConst102;
	float fRec80[3];
	float fConst103;
	float fConst105;
	float fConst106;
	float fRec81[3];
	float fConst107;
	float fConst109;
	float fConst110;
	float fRec82[3];
	float fConst111;
	float fConst113;
	float fConst114;
	float fRec83[3];
	float fConst115;
	float fConst117;
	float fConst118;
	float fRec84[3];
	float fConst119;
	float fConst121;
	float fConst122;
	float fRec85[3];
	float fConst123;
	float fConst125;
	float fConst126;
	float fRec86[3];
	float fConst127;
	float fConst129;
	float fConst130;
	float fRec87[3];
	float fConst131;
	float fConst133;
	float fConst134;
	float fRec88[3];
	float fConst135;
	float fConst137;
	float fConst138;
	float fRec89[3];
	float fConst139;
	float fConst141;
	float fConst142;
	float fRec90[3];
	float fConst143;
	float fConst145;
	float fConst146;
	float fRec91[3];
	float fConst147;
	float fConst149;
	float fConst150;
	float fRec92[3];
	float fConst151;
	float fConst153;
	float fConst154;
	float fRec93[3];
	float fConst155;
	float fConst157;
	float fConst158;
	float fRec94[3];
	float fConst159;
	float fConst161;
	float fConst162;
	float fRec95[3];
	float fConst163;
	float fConst165;
	float fConst166;
	float fRec96[3];
	float fConst167;
	float fConst169;
	float fConst170;
	float fRec97[3];
	float fConst171;
	float fConst173;
	float fConst174;
	float fRec98[3];
	float fConst175;
	float fConst177;
	float fConst178;
	float fRec99[3];
	float fConst179;
	float fConst181;
	float fConst182;
	float fRec100[3];
	float fConst183;
	float fConst185;
	float fConst186;
	float fRec101[3];
	float fConst187;
	float fConst189;
	float fConst190;
	float fRec102[3];
	float fConst191;
	float fConst193;
	float fConst194;
	float fRec103[3];
	float fConst195;
	float fConst197;
	float fConst198;
	float fRec104[3];
	float fConst199;
	float fConst201;
	float fConst202;
	float fRec105[3];
	float fConst203;
	float fConst205;
	float fConst206;
	float fRec106[3];
	float fConst207;
	float fConst209;
	float fConst210;
	float fRec107[3];
	float fConst211;
	float fConst213;
	float fConst214;
	float fRec108[3];
	float fConst215;
	float fConst217;
	float fConst218;
	float fRec109[3];
	float fConst219;
	float fConst221;
	float fConst222;
	float fRec110[3];
	float fConst223;
	float fConst225;
	float fConst226;
	float fRec111[3];
	float fConst227;
	float fConst229;
	float fConst230;
	float fRec112[3];
	float fConst231;
	float fConst233;
	float fConst234;
	float fRec113[3];
	float fConst235;
	float fConst236;
	float fConst237;
	FAUSTFLOAT fHslider6;
	float fRec114[2];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("compile_options", "-lang cpp -es 1 -scal -ftz 0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("envelopes.lib/ar:author", "Yann Orlarey, Stéphane Letz");
		m->declare("envelopes.lib/author", "GRAME");
		m->declare("envelopes.lib/copyright", "GRAME");
		m->declare("envelopes.lib/license", "LGPL with exception");
		m->declare("envelopes.lib/name", "Faust Envelope Library");
		m->declare("envelopes.lib/version", "0.1");
		m->declare("filename", "energyTest.dsp");
		m->declare("filters.lib/dcblocker:author", "Julius O. Smith III");
		m->declare("filters.lib/dcblocker:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/dcblocker:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/highpass:author", "Julius O. Smith III");
		m->declare("filters.lib/highpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
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
		mydspSIG0* sig0 = newmydspSIG0();
		sig0->instanceInitmydspSIG0(sample_rate);
		sig0->fillmydspSIG0(350, ftbl0mydspSIG0);
		deletemydspSIG0(sig0);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		float fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = (1.0f / fConst0);
		fConst2 = (0.25f * fConst0);
		fConst3 = (0.5f * fConst0);
		fConst4 = std::exp((0.0f - (1000.0f / fConst0)));
		fConst5 = (1.0f - fConst4);
		fConst6 = (0.5f * fConst5);
		fConst7 = std::exp((0.0f - (50.0f / fConst0)));
		fConst8 = (1.0f - fConst7);
		float fConst9 = std::tan((7696.90186f / fConst0));
		float fConst10 = (1.0f / fConst9);
		float fConst11 = (((fConst10 + 1.41421354f) / fConst9) + 1.0f);
		fConst12 = (0.0500000007f / fConst11);
		fConst13 = (1.0f / fConst11);
		fConst14 = (((fConst10 + -1.41421354f) / fConst9) + 1.0f);
		fConst15 = (2.0f * (1.0f - (1.0f / mydsp_faustpower2_f(fConst9))));
		fConst16 = (0.00882352982f * fConst0);
		fConst17 = (0.000397058815f * fConst0);
		fConst18 = (0.00107352936f * fConst0);
		fConst19 = (3.14159274f / fConst0);
		fConst20 = (0.00147058826f * fConst0);
		fConst21 = ftbl0mydspSIG0[50];
		float fConst22 = std::tan((31415.9258f / fConst0));
		float fConst23 = (1.0f / fConst22);
		fConst24 = (1.0f / (((fConst23 + 1.41421354f) / fConst22) + 1.0f));
		float fConst25 = std::tan((31.415926f / fConst0));
		float fConst26 = (1.0f / fConst25);
		fConst27 = (1.0f / (((fConst26 + 1.41421354f) / fConst25) + 1.0f));
		float fConst28 = mydsp_faustpower2_f(fConst25);
		fConst29 = (1.0f / fConst28);
		fConst30 = (((fConst26 + -1.41421354f) / fConst25) + 1.0f);
		fConst31 = (2.0f * (1.0f - fConst29));
		fConst32 = (0.0f - (2.0f / fConst28));
		fConst33 = (((fConst23 + -1.41421354f) / fConst22) + 1.0f);
		fConst34 = (2.0f * (1.0f - (1.0f / mydsp_faustpower2_f(fConst22))));
		fConst35 = (1.0f / std::max<float>(1.0f, (0.00159999996f * fConst0)));
		float fConst36 = std::pow(0.00100000005f, (0.0448570587f / fConst0));
		fConst37 = (std::cos((2839.48462f / fConst0)) * (0.0f - (2.0f * fConst36)));
		fConst38 = mydsp_faustpower2_f(fConst36);
		fConst39 = ftbl0mydspSIG0[51];
		float fConst40 = std::pow(0.00100000005f, (0.0449536517f / fConst0));
		fConst41 = (std::cos((2858.84937f / fConst0)) * (0.0f - (2.0f * fConst40)));
		fConst42 = mydsp_faustpower2_f(fConst40);
		fConst43 = ftbl0mydspSIG0[52];
		float fConst44 = std::pow(0.00100000005f, (0.0609094799f / fConst0));
		fConst45 = (std::cos((5432.7124f / fConst0)) * (0.0f - (2.0f * fConst44)));
		fConst46 = mydsp_faustpower2_f(fConst44);
		fConst47 = ftbl0mydspSIG0[53];
		float fConst48 = std::pow(0.00100000005f, (0.0612352863f / fConst0));
		fConst49 = (std::cos((5475.18018f / fConst0)) * (0.0f - (2.0f * fConst48)));
		fConst50 = mydsp_faustpower2_f(fConst48);
		fConst51 = ftbl0mydspSIG0[54];
		float fConst52 = std::pow(0.00100000005f, (0.0721585602f / fConst0));
		fConst53 = (std::cos((6738.52783f / fConst0)) * (0.0f - (2.0f * fConst52)));
		fConst54 = mydsp_faustpower2_f(fConst52);
		fConst55 = ftbl0mydspSIG0[55];
		float fConst56 = std::pow(0.00100000005f, (0.0722505748f / fConst0));
		fConst57 = (std::cos((6748.01514f / fConst0)) * (0.0f - (2.0f * fConst56)));
		fConst58 = mydsp_faustpower2_f(fConst56);
		fConst59 = ftbl0mydspSIG0[56];
		float fConst60 = std::pow(0.00100000005f, (0.0874899849f / fConst0));
		fConst61 = (std::cos((8119.32031f / fConst0)) * (0.0f - (2.0f * fConst60)));
		fConst62 = mydsp_faustpower2_f(fConst60);
		fConst63 = ftbl0mydspSIG0[57];
		float fConst64 = std::pow(0.00100000005f, (0.0875099227f / fConst0));
		fConst65 = (std::cos((8120.89111f / fConst0)) * (0.0f - (2.0f * fConst64)));
		fConst66 = mydsp_faustpower2_f(fConst64);
		fConst67 = ftbl0mydspSIG0[58];
		float fConst68 = std::pow(0.00100000005f, (0.107014902f / fConst0));
		fConst69 = (std::cos((9453.68066f / fConst0)) * (0.0f - (2.0f * fConst68)));
		fConst70 = mydsp_faustpower2_f(fConst68);
		fConst71 = ftbl0mydspSIG0[59];
		float fConst72 = std::pow(0.00100000005f, (0.110012196f / fConst0));
		fConst73 = (std::cos((9628.41602f / fConst0)) * (0.0f - (2.0f * fConst72)));
		fConst74 = mydsp_faustpower2_f(fConst72);
		fConst75 = ftbl0mydspSIG0[60];
		float fConst76 = std::pow(0.00100000005f, (0.123582378f / fConst0));
		fConst77 = (std::cos((10343.3799f / fConst0)) * (0.0f - (2.0f * fConst76)));
		fConst78 = mydsp_faustpower2_f(fConst76);
		fConst79 = ftbl0mydspSIG0[61];
		float fConst80 = std::pow(0.00100000005f, (0.123685889f / fConst0));
		fConst81 = (std::cos((10348.4062f / fConst0)) * (0.0f - (2.0f * fConst80)));
		fConst82 = mydsp_faustpower2_f(fConst80);
		fConst83 = ftbl0mydspSIG0[62];
		float fConst84 = std::pow(0.00100000005f, (0.127769172f / fConst0));
		fConst85 = (std::cos((10542.1172f / fConst0)) * (0.0f - (2.0f * fConst84)));
		fConst86 = mydsp_faustpower2_f(fConst84);
		fConst87 = ftbl0mydspSIG0[63];
		float fConst88 = std::pow(0.00100000005f, (0.127809823f / fConst0));
		fConst89 = (std::cos((10544.002f / fConst0)) * (0.0f - (2.0f * fConst88)));
		fConst90 = mydsp_faustpower2_f(fConst88);
		fConst91 = ftbl0mydspSIG0[64];
		float fConst92 = std::pow(0.00100000005f, (0.157417208f / fConst0));
		fConst93 = (std::cos((11728.3818f / fConst0)) * (0.0f - (2.0f * fConst92)));
		fConst94 = mydsp_faustpower2_f(fConst92);
		fConst95 = ftbl0mydspSIG0[65];
		float fConst96 = std::pow(0.00100000005f, (0.160256028f / fConst0));
		fConst97 = (std::cos((11825.457f / fConst0)) * (0.0f - (2.0f * fConst96)));
		fConst98 = mydsp_faustpower2_f(fConst96);
		fConst99 = ftbl0mydspSIG0[66];
		float fConst100 = std::pow(0.00100000005f, (0.18117331f / fConst0));
		fConst101 = (std::cos((12473.3799f / fConst0)) * (0.0f - (2.0f * fConst100)));
		fConst102 = mydsp_faustpower2_f(fConst100);
		fConst103 = ftbl0mydspSIG0[67];
		float fConst104 = std::pow(0.00100000005f, (0.182209119f / fConst0));
		fConst105 = (std::cos((12502.7217f / fConst0)) * (0.0f - (2.0f * fConst104)));
		fConst106 = mydsp_faustpower2_f(fConst104);
		fConst107 = ftbl0mydspSIG0[68];
		float fConst108 = std::pow(0.00100000005f, (0.2133362f / fConst0));
		fConst109 = (std::cos((13288.4971f / fConst0)) * (0.0f - (2.0f * fConst108)));
		fConst110 = mydsp_faustpower2_f(fConst108);
		fConst111 = ftbl0mydspSIG0[69];
		float fConst112 = std::pow(0.00100000005f, (0.298688531f / fConst0));
		fConst113 = (std::cos((14808.2744f / fConst0)) * (0.0f - (2.0f * fConst112)));
		fConst114 = mydsp_faustpower2_f(fConst112);
		fConst115 = ftbl0mydspSIG0[70];
		float fConst116 = std::pow(0.00100000005f, (0.341466933f / fConst0));
		fConst117 = (std::cos((15358.1152f / fConst0)) * (0.0f - (2.0f * fConst116)));
		fConst118 = mydsp_faustpower2_f(fConst116);
		fConst119 = ftbl0mydspSIG0[71];
		float fConst120 = std::pow(0.00100000005f, (0.342488438f / fConst0));
		fConst121 = (std::cos((15370.0537f / fConst0)) * (0.0f - (2.0f * fConst120)));
		fConst122 = mydsp_faustpower2_f(fConst120);
		fConst123 = ftbl0mydspSIG0[72];
		float fConst124 = std::pow(0.00100000005f, (0.391345769f / fConst0));
		fConst125 = (std::cos((15888.7305f / fConst0)) * (0.0f - (2.0f * fConst124)));
		fConst126 = mydsp_faustpower2_f(fConst124);
		fConst127 = ftbl0mydspSIG0[73];
		float fConst128 = std::pow(0.00100000005f, (0.392745376f / fConst0));
		fConst129 = (std::cos((15902.2393f / fConst0)) * (0.0f - (2.0f * fConst128)));
		fConst130 = mydsp_faustpower2_f(fConst128);
		fConst131 = ftbl0mydspSIG0[74];
		float fConst132 = std::pow(0.00100000005f, (0.499480247f / fConst0));
		fConst133 = (std::cos((16768.9414f / fConst0)) * (0.0f - (2.0f * fConst132)));
		fConst134 = mydsp_faustpower2_f(fConst132);
		fConst135 = ftbl0mydspSIG0[75];
		float fConst136 = std::pow(0.00100000005f, (0.500184536f / fConst0));
		fConst137 = (std::cos((16773.7793f / fConst0)) * (0.0f - (2.0f * fConst136)));
		fConst138 = mydsp_faustpower2_f(fConst136);
		fConst139 = ftbl0mydspSIG0[76];
		float fConst140 = std::pow(0.00100000005f, (0.56877172f / fConst0));
		fConst141 = (std::cos((17203.7383f / fConst0)) * (0.0f - (2.0f * fConst140)));
		fConst142 = mydsp_faustpower2_f(fConst140);
		fConst143 = ftbl0mydspSIG0[77];
		float fConst144 = std::pow(0.00100000005f, (0.580974996f / fConst0));
		fConst145 = (std::cos((17272.6641f / fConst0)) * (0.0f - (2.0f * fConst144)));
		fConst146 = mydsp_faustpower2_f(fConst144);
		fConst147 = ftbl0mydspSIG0[78];
		float fConst148 = std::pow(0.00100000005f, (0.58267206f / fConst0));
		fConst149 = (std::cos((17282.0898f / fConst0)) * (0.0f - (2.0f * fConst148)));
		fConst150 = mydsp_faustpower2_f(fConst148);
		fConst151 = ftbl0mydspSIG0[79];
		float fConst152 = std::pow(0.00100000005f, (0.58560729f / fConst0));
		fConst153 = (std::cos((17298.3008f / fConst0)) * (0.0f - (2.0f * fConst152)));
		fConst154 = mydsp_faustpower2_f(fConst152);
		fConst155 = ftbl0mydspSIG0[80];
		float fConst156 = std::pow(0.00100000005f, (0.679538369f / fConst0));
		fConst157 = (std::cos((17763.7578f / fConst0)) * (0.0f - (2.0f * fConst156)));
		fConst158 = mydsp_faustpower2_f(fConst156);
		fConst159 = ftbl0mydspSIG0[81];
		float fConst160 = std::pow(0.00100000005f, (0.701286793f / fConst0));
		fConst161 = (std::cos((17858.8242f / fConst0)) * (0.0f - (2.0f * fConst160)));
		fConst162 = mydsp_faustpower2_f(fConst160);
		fConst163 = ftbl0mydspSIG0[82];
		float fConst164 = std::pow(0.00100000005f, (0.981793463f / fConst0));
		fConst165 = (std::cos((18802.8086f / fConst0)) * (0.0f - (2.0f * fConst164)));
		fConst166 = mydsp_faustpower2_f(fConst164);
		fConst167 = ftbl0mydspSIG0[83];
		float fConst168 = std::pow(0.00100000005f, (0.991939485f / fConst0));
		fConst169 = (std::cos((18829.7012f / fConst0)) * (0.0f - (2.0f * fConst168)));
		fConst170 = mydsp_faustpower2_f(fConst168);
		fConst171 = ftbl0mydspSIG0[84];
		float fConst172 = std::pow(0.00100000005f, (1.57685912f / fConst0));
		fConst173 = (std::cos((19934.0957f / fConst0)) * (0.0f - (2.0f * fConst172)));
		fConst174 = mydsp_faustpower2_f(fConst172);
		fConst175 = ftbl0mydspSIG0[85];
		float fConst176 = std::pow(0.00100000005f, (2.61620831f / fConst0));
		fConst177 = (std::cos((20927.9707f / fConst0)) * (0.0f - (2.0f * fConst176)));
		fConst178 = mydsp_faustpower2_f(fConst176);
		fConst179 = ftbl0mydspSIG0[86];
		float fConst180 = std::pow(0.00100000005f, (3.26228237f / fConst0));
		fConst181 = (std::cos((21302.0723f / fConst0)) * (0.0f - (2.0f * fConst180)));
		fConst182 = mydsp_faustpower2_f(fConst180);
		fConst183 = ftbl0mydspSIG0[87];
		float fConst184 = std::pow(0.00100000005f, (3.42948842f / fConst0));
		fConst185 = (std::cos((21382.3086f / fConst0)) * (0.0f - (2.0f * fConst184)));
		fConst186 = mydsp_faustpower2_f(fConst184);
		fConst187 = ftbl0mydspSIG0[88];
		float fConst188 = std::pow(0.00100000005f, (5.61909866f / fConst0));
		fConst189 = (std::cos((22094.3809f / fConst0)) * (0.0f - (2.0f * fConst188)));
		fConst190 = mydsp_faustpower2_f(fConst188);
		fConst191 = ftbl0mydspSIG0[89];
		float fConst192 = std::pow(0.00100000005f, (6.26517916f / fConst0));
		fConst193 = (std::cos((22233.3027f / fConst0)) * (0.0f - (2.0f * fConst192)));
		fConst194 = mydsp_faustpower2_f(fConst192);
		fConst195 = ftbl0mydspSIG0[90];
		float fConst196 = std::pow(0.00100000005f, (7.91042233f / fConst0));
		fConst197 = (std::cos((22511.334f / fConst0)) * (0.0f - (2.0f * fConst196)));
		fConst198 = mydsp_faustpower2_f(fConst196);
		fConst199 = ftbl0mydspSIG0[91];
		float fConst200 = std::pow(0.00100000005f, (7.95560217f / fConst0));
		fConst201 = (std::cos((22517.8047f / fConst0)) * (0.0f - (2.0f * fConst200)));
		fConst202 = mydsp_faustpower2_f(fConst200);
		fConst203 = ftbl0mydspSIG0[92];
		float fConst204 = std::pow(0.00100000005f, (21.1559925f / fConst0));
		fConst205 = (std::cos((23436.5332f / fConst0)) * (0.0f - (2.0f * fConst204)));
		fConst206 = mydsp_faustpower2_f(fConst204);
		fConst207 = ftbl0mydspSIG0[93];
		float fConst208 = std::pow(0.00100000005f, (22.8584423f / fConst0));
		fConst209 = (std::cos((23495.0293f / fConst0)) * (0.0f - (2.0f * fConst208)));
		fConst210 = mydsp_faustpower2_f(fConst208);
		fConst211 = ftbl0mydspSIG0[94];
		float fConst212 = std::pow(0.00100000005f, (27.0563507f / fConst0));
		fConst213 = (std::cos((23616.3574f / fConst0)) * (0.0f - (2.0f * fConst212)));
		fConst214 = mydsp_faustpower2_f(fConst212);
		fConst215 = ftbl0mydspSIG0[95];
		float fConst216 = std::pow(0.00100000005f, (173.368958f / fConst0));
		fConst217 = (std::cos((24528.1738f / fConst0)) * (0.0f - (2.0f * fConst216)));
		fConst218 = mydsp_faustpower2_f(fConst216);
		fConst219 = ftbl0mydspSIG0[96];
		float fConst220 = std::pow(0.00100000005f, (415.098541f / fConst0));
		fConst221 = (std::cos((24772.0234f / fConst0)) * (0.0f - (2.0f * fConst220)));
		fConst222 = mydsp_faustpower2_f(fConst220);
		fConst223 = ftbl0mydspSIG0[97];
		float fConst224 = std::pow(0.00100000005f, (1631.8125f / fConst0));
		fConst225 = (std::cos((25018.0098f / fConst0)) * (0.0f - (2.0f * fConst224)));
		fConst226 = mydsp_faustpower2_f(fConst224);
		fConst227 = ftbl0mydspSIG0[98];
		float fConst228 = std::pow(0.00100000005f, (1752.41809f / fConst0));
		fConst229 = (std::cos((25027.498f / fConst0)) * (0.0f - (2.0f * fConst228)));
		fConst230 = mydsp_faustpower2_f(fConst228);
		fConst231 = ftbl0mydspSIG0[99];
		float fConst232 = std::pow(0.00100000005f, (13161720.0f / fConst0));
		fConst233 = (std::cos((25346.1816f / fConst0)) * (0.0f - (2.0f * fConst232)));
		fConst234 = mydsp_faustpower2_f(fConst232);
		fConst235 = std::max<float>(1.0f, (0.00100000005f * fConst0));
		fConst236 = (1.0f / fConst235);
		fConst237 = (1.0f / std::max<float>(1.0f, (2.0f * fConst0)));
	}
	
	virtual void instanceResetUserInterface() {
		fHslider0 = FAUSTFLOAT(0.0f);
		fHslider1 = FAUSTFLOAT(0.25f);
		fHslider2 = FAUSTFLOAT(440.0f);
		fHslider3 = FAUSTFLOAT(0.0f);
		fHslider4 = FAUSTFLOAT(0.5f);
		fHslider5 = FAUSTFLOAT(0.0f);
		fHslider6 = FAUSTFLOAT(0.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			iVec0[l0] = 0;
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec0[l1] = 0.0f;
		}
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			fRec1[l2] = 0.0f;
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec4[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec3[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fVec1[l5] = 0.0f;
		}
		IOTA = 0;
		for (int l6 = 0; (l6 < 4096); l6 = (l6 + 1)) {
			fVec2[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec2[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec5[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			iRec21[l9] = 0;
		}
		for (int l10 = 0; (l10 < 2); l10 = (l10 + 1)) {
			fRec27[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec29[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec28[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			iRec31[l13] = 0;
		}
		for (int l14 = 0; (l14 < 3); l14 = (l14 + 1)) {
			fRec30[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fRec32[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 2048); l16 = (l16 + 1)) {
			fRec33[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fVec3[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 2048); l18 = (l18 + 1)) {
			fVec4[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fVec5[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 2); l20 = (l20 + 1)) {
			fRec26[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 2048); l21 = (l21 + 1)) {
			fRec17[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec13[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 2048); l23 = (l23 + 1)) {
			fRec9[l23] = 0.0f;
		}
		for (int l24 = 0; (l24 < 2); l24 = (l24 + 1)) {
			fRec7[l24] = 0.0f;
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec8[l25] = 0.0f;
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			fRec6[l26] = 0.0f;
		}
		for (int l27 = 0; (l27 < 2); l27 = (l27 + 1)) {
			iRec49[l27] = 0;
		}
		for (int l28 = 0; (l28 < 2); l28 = (l28 + 1)) {
			fRec55[l28] = 0.0f;
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			fRec57[l29] = 0.0f;
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			fRec56[l30] = 0.0f;
		}
		for (int l31 = 0; (l31 < 3); l31 = (l31 + 1)) {
			fRec58[l31] = 0.0f;
		}
		for (int l32 = 0; (l32 < 2); l32 = (l32 + 1)) {
			fRec59[l32] = 0.0f;
		}
		for (int l33 = 0; (l33 < 2048); l33 = (l33 + 1)) {
			fRec60[l33] = 0.0f;
		}
		for (int l34 = 0; (l34 < 2); l34 = (l34 + 1)) {
			fVec6[l34] = 0.0f;
		}
		for (int l35 = 0; (l35 < 2048); l35 = (l35 + 1)) {
			fVec7[l35] = 0.0f;
		}
		for (int l36 = 0; (l36 < 2); l36 = (l36 + 1)) {
			fVec8[l36] = 0.0f;
		}
		for (int l37 = 0; (l37 < 2); l37 = (l37 + 1)) {
			fRec54[l37] = 0.0f;
		}
		for (int l38 = 0; (l38 < 2048); l38 = (l38 + 1)) {
			fRec45[l38] = 0.0f;
		}
		for (int l39 = 0; (l39 < 2); l39 = (l39 + 1)) {
			fRec41[l39] = 0.0f;
		}
		for (int l40 = 0; (l40 < 2048); l40 = (l40 + 1)) {
			fRec37[l40] = 0.0f;
		}
		for (int l41 = 0; (l41 < 2); l41 = (l41 + 1)) {
			fRec35[l41] = 0.0f;
		}
		for (int l42 = 0; (l42 < 2); l42 = (l42 + 1)) {
			fRec36[l42] = 0.0f;
		}
		for (int l43 = 0; (l43 < 2); l43 = (l43 + 1)) {
			fRec34[l43] = 0.0f;
		}
		for (int l44 = 0; (l44 < 3); l44 = (l44 + 1)) {
			fRec63[l44] = 0.0f;
		}
		for (int l45 = 0; (l45 < 3); l45 = (l45 + 1)) {
			fRec62[l45] = 0.0f;
		}
		for (int l46 = 0; (l46 < 2); l46 = (l46 + 1)) {
			fVec9[l46] = 0.0f;
		}
		for (int l47 = 0; (l47 < 2); l47 = (l47 + 1)) {
			iRec64[l47] = 0;
		}
		for (int l48 = 0; (l48 < 3); l48 = (l48 + 1)) {
			fRec61[l48] = 0.0f;
		}
		for (int l49 = 0; (l49 < 3); l49 = (l49 + 1)) {
			fRec65[l49] = 0.0f;
		}
		for (int l50 = 0; (l50 < 3); l50 = (l50 + 1)) {
			fRec66[l50] = 0.0f;
		}
		for (int l51 = 0; (l51 < 3); l51 = (l51 + 1)) {
			fRec67[l51] = 0.0f;
		}
		for (int l52 = 0; (l52 < 3); l52 = (l52 + 1)) {
			fRec68[l52] = 0.0f;
		}
		for (int l53 = 0; (l53 < 3); l53 = (l53 + 1)) {
			fRec69[l53] = 0.0f;
		}
		for (int l54 = 0; (l54 < 3); l54 = (l54 + 1)) {
			fRec70[l54] = 0.0f;
		}
		for (int l55 = 0; (l55 < 3); l55 = (l55 + 1)) {
			fRec71[l55] = 0.0f;
		}
		for (int l56 = 0; (l56 < 3); l56 = (l56 + 1)) {
			fRec72[l56] = 0.0f;
		}
		for (int l57 = 0; (l57 < 3); l57 = (l57 + 1)) {
			fRec73[l57] = 0.0f;
		}
		for (int l58 = 0; (l58 < 3); l58 = (l58 + 1)) {
			fRec74[l58] = 0.0f;
		}
		for (int l59 = 0; (l59 < 3); l59 = (l59 + 1)) {
			fRec75[l59] = 0.0f;
		}
		for (int l60 = 0; (l60 < 3); l60 = (l60 + 1)) {
			fRec76[l60] = 0.0f;
		}
		for (int l61 = 0; (l61 < 3); l61 = (l61 + 1)) {
			fRec77[l61] = 0.0f;
		}
		for (int l62 = 0; (l62 < 3); l62 = (l62 + 1)) {
			fRec78[l62] = 0.0f;
		}
		for (int l63 = 0; (l63 < 3); l63 = (l63 + 1)) {
			fRec79[l63] = 0.0f;
		}
		for (int l64 = 0; (l64 < 3); l64 = (l64 + 1)) {
			fRec80[l64] = 0.0f;
		}
		for (int l65 = 0; (l65 < 3); l65 = (l65 + 1)) {
			fRec81[l65] = 0.0f;
		}
		for (int l66 = 0; (l66 < 3); l66 = (l66 + 1)) {
			fRec82[l66] = 0.0f;
		}
		for (int l67 = 0; (l67 < 3); l67 = (l67 + 1)) {
			fRec83[l67] = 0.0f;
		}
		for (int l68 = 0; (l68 < 3); l68 = (l68 + 1)) {
			fRec84[l68] = 0.0f;
		}
		for (int l69 = 0; (l69 < 3); l69 = (l69 + 1)) {
			fRec85[l69] = 0.0f;
		}
		for (int l70 = 0; (l70 < 3); l70 = (l70 + 1)) {
			fRec86[l70] = 0.0f;
		}
		for (int l71 = 0; (l71 < 3); l71 = (l71 + 1)) {
			fRec87[l71] = 0.0f;
		}
		for (int l72 = 0; (l72 < 3); l72 = (l72 + 1)) {
			fRec88[l72] = 0.0f;
		}
		for (int l73 = 0; (l73 < 3); l73 = (l73 + 1)) {
			fRec89[l73] = 0.0f;
		}
		for (int l74 = 0; (l74 < 3); l74 = (l74 + 1)) {
			fRec90[l74] = 0.0f;
		}
		for (int l75 = 0; (l75 < 3); l75 = (l75 + 1)) {
			fRec91[l75] = 0.0f;
		}
		for (int l76 = 0; (l76 < 3); l76 = (l76 + 1)) {
			fRec92[l76] = 0.0f;
		}
		for (int l77 = 0; (l77 < 3); l77 = (l77 + 1)) {
			fRec93[l77] = 0.0f;
		}
		for (int l78 = 0; (l78 < 3); l78 = (l78 + 1)) {
			fRec94[l78] = 0.0f;
		}
		for (int l79 = 0; (l79 < 3); l79 = (l79 + 1)) {
			fRec95[l79] = 0.0f;
		}
		for (int l80 = 0; (l80 < 3); l80 = (l80 + 1)) {
			fRec96[l80] = 0.0f;
		}
		for (int l81 = 0; (l81 < 3); l81 = (l81 + 1)) {
			fRec97[l81] = 0.0f;
		}
		for (int l82 = 0; (l82 < 3); l82 = (l82 + 1)) {
			fRec98[l82] = 0.0f;
		}
		for (int l83 = 0; (l83 < 3); l83 = (l83 + 1)) {
			fRec99[l83] = 0.0f;
		}
		for (int l84 = 0; (l84 < 3); l84 = (l84 + 1)) {
			fRec100[l84] = 0.0f;
		}
		for (int l85 = 0; (l85 < 3); l85 = (l85 + 1)) {
			fRec101[l85] = 0.0f;
		}
		for (int l86 = 0; (l86 < 3); l86 = (l86 + 1)) {
			fRec102[l86] = 0.0f;
		}
		for (int l87 = 0; (l87 < 3); l87 = (l87 + 1)) {
			fRec103[l87] = 0.0f;
		}
		for (int l88 = 0; (l88 < 3); l88 = (l88 + 1)) {
			fRec104[l88] = 0.0f;
		}
		for (int l89 = 0; (l89 < 3); l89 = (l89 + 1)) {
			fRec105[l89] = 0.0f;
		}
		for (int l90 = 0; (l90 < 3); l90 = (l90 + 1)) {
			fRec106[l90] = 0.0f;
		}
		for (int l91 = 0; (l91 < 3); l91 = (l91 + 1)) {
			fRec107[l91] = 0.0f;
		}
		for (int l92 = 0; (l92 < 3); l92 = (l92 + 1)) {
			fRec108[l92] = 0.0f;
		}
		for (int l93 = 0; (l93 < 3); l93 = (l93 + 1)) {
			fRec109[l93] = 0.0f;
		}
		for (int l94 = 0; (l94 < 3); l94 = (l94 + 1)) {
			fRec110[l94] = 0.0f;
		}
		for (int l95 = 0; (l95 < 3); l95 = (l95 + 1)) {
			fRec111[l95] = 0.0f;
		}
		for (int l96 = 0; (l96 < 3); l96 = (l96 + 1)) {
			fRec112[l96] = 0.0f;
		}
		for (int l97 = 0; (l97 < 3); l97 = (l97 + 1)) {
			fRec113[l97] = 0.0f;
		}
		for (int l98 = 0; (l98 < 2); l98 = (l98 + 1)) {
			fRec114[l98] = 0.0f;
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
		ui_interface->addHorizontalSlider("MasterGain", &fHslider6, 0.0f, -40.0f, 0.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("SUBPARAM_blowPressure", &fHslider4, 0.5f, 0.0f, 0.5f, 0.00100000005f);
		ui_interface->addHorizontalSlider("SUBPARAM_drive", &fHslider1, 0.25f, 0.0f, 0.5f, 0.00100000005f);
		ui_interface->addHorizontalSlider("SUBPARAM_freq_Hz", &fHslider2, 440.0f, 0.0f, 1200.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("SUBPARAM_subAmp", &fHslider3, 0.0f, 0.0f, 2.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("T_Bell", &fHslider5, 0.0f, 0.0f, 0.699999988f, 0.699999988f);
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
		float fSlow5 = float(fHslider5);
		float fSlow6 = (0.00100000005f * float(fHslider6));
		for (int i = 0; (i < count); i = (i + 1)) {
			iVec0[0] = 1;
			fRec0[0] = (fSlow0 + (0.999000013f * fRec0[1]));
			fRec1[0] = (fSlow1 + (0.999000013f * fRec1[1]));
			fRec4[0] = (fSlow2 + (0.999000013f * fRec4[1]));
			float fTemp0 = ((fRec0[0] * fRec4[0]) + 220.0f);
			float fTemp1 = std::max<float>((0.25f * fTemp0), 23.4489498f);
			float fTemp2 = std::max<float>(20.0f, std::fabs(fTemp1));
			float fTemp3 = (fRec3[1] + (fConst1 * fTemp2));
			fRec3[0] = (fTemp3 - std::floor(fTemp3));
			float fTemp4 = mydsp_faustpower2_f(((2.0f * fRec3[0]) + -1.0f));
			fVec1[0] = fTemp4;
			float fTemp5 = ((float(iVec0[1]) * (fTemp4 - fVec1[1])) / fTemp2);
			fVec2[(IOTA & 4095)] = fTemp5;
			float fTemp6 = std::max<float>(0.0f, std::min<float>(2047.0f, (fConst3 / fTemp1)));
			int iTemp7 = int(fTemp6);
			float fTemp8 = std::floor(fTemp6);
			fRec2[0] = ((0.999000013f * fRec2[1]) + (fConst2 * ((fTemp5 - (fVec2[((IOTA - iTemp7) & 4095)] * (fTemp8 + (1.0f - fTemp6)))) - ((fTemp6 - fTemp8) * fVec2[((IOTA - (iTemp7 + 1)) & 4095)]))));
			fRec5[0] = (fSlow3 + (0.999000013f * fRec5[1]));
			iRec21[0] = 0;
			fRec27[0] = ((0.699999988f * fRec27[1]) + (0.284999996f * fRec26[1]));
			float fRec25 = (fRec27[0] + float(iRec21[1]));
			fRec29[0] = ((fConst7 * fRec29[1]) + (fConst8 * float((fTemp0 > 200.0f))));
			fRec28[0] = ((fConst4 * fRec28[1]) + (fConst6 * fRec29[0]));
			iRec31[0] = ((1103515245 * iRec31[1]) + 12345);
			float fTemp9 = (4.65661287e-10f * float(iRec31[0]));
			fRec30[0] = (fTemp9 - (fConst13 * ((fConst14 * fRec30[2]) + (fConst15 * fRec30[1]))));
			fRec32[0] = fRec7[1];
			fRec33[(IOTA & 2047)] = (0.949999988f * fRec32[1]);
			float fTemp10 = ((340.0f / fTemp0) + 0.270000011f);
			float fTemp11 = (fConst17 * fTemp10);
			float fTemp12 = (fTemp11 + -1.49999499f);
			int iTemp13 = int(fTemp12);
			int iTemp14 = (int(std::min<float>(fConst16, float(std::max<int>(0, iTemp13)))) + 1);
			float fTemp15 = std::floor(fTemp12);
			float fTemp16 = (fTemp11 + (-1.0f - fTemp15));
			float fTemp17 = (0.0f - fTemp16);
			float fTemp18 = (fTemp11 + (-2.0f - fTemp15));
			float fTemp19 = (0.0f - (0.5f * fTemp18));
			float fTemp20 = (fTemp11 + (-3.0f - fTemp15));
			float fTemp21 = (0.0f - (0.333333343f * fTemp20));
			float fTemp22 = (fTemp11 + (-4.0f - fTemp15));
			float fTemp23 = (0.0f - (0.25f * fTemp22));
			float fTemp24 = (fTemp11 - fTemp15);
			int iTemp25 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp13 + 1))))) + 1);
			float fTemp26 = (0.0f - fTemp18);
			float fTemp27 = (0.0f - (0.5f * fTemp20));
			float fTemp28 = (0.0f - (0.333333343f * fTemp22));
			int iTemp29 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp13 + 2))))) + 1);
			float fTemp30 = (0.0f - fTemp20);
			float fTemp31 = (0.0f - (0.5f * fTemp22));
			float fTemp32 = (fTemp16 * fTemp18);
			int iTemp33 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp13 + 3))))) + 1);
			float fTemp34 = (0.0f - fTemp22);
			float fTemp35 = (fTemp32 * fTemp20);
			int iTemp36 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp13 + 4))))) + 1);
			fVec3[0] = (((((fRec33[((IOTA - iTemp14) & 2047)] * fTemp17) * fTemp19) * fTemp21) * fTemp23) + (fTemp24 * ((((((fRec33[((IOTA - iTemp25) & 2047)] * fTemp26) * fTemp27) * fTemp28) + (0.5f * (((fTemp16 * fRec33[((IOTA - iTemp29) & 2047)]) * fTemp30) * fTemp31))) + (0.166666672f * ((fTemp32 * fRec33[((IOTA - iTemp33) & 2047)]) * fTemp34))) + (0.0416666679f * (fTemp35 * fRec33[((IOTA - iTemp36) & 2047)])))));
			float fTemp37 = ((fRec28[0] * ((fConst12 * (fRec30[2] + (fRec30[0] + (2.0f * fRec30[1])))) + 1.0f)) - (0.5f * fVec3[1]));
			float fTemp38 = ((0.5f * fRec13[1]) + std::max<float>(-1.0f, std::min<float>(1.0f, (fTemp37 * (mydsp_faustpower2_f(fTemp37) + -1.0f)))));
			fVec4[(IOTA & 2047)] = fTemp38;
			float fTemp39 = (fConst18 * fTemp10);
			float fTemp40 = (fTemp39 + -1.49999499f);
			int iTemp41 = int(fTemp40);
			int iTemp42 = (int(std::min<float>(fConst16, float(std::max<int>(0, iTemp41)))) + 1);
			float fTemp43 = std::floor(fTemp40);
			float fTemp44 = (fTemp39 + (-1.0f - fTemp43));
			float fTemp45 = (0.0f - fTemp44);
			float fTemp46 = (fTemp39 + (-2.0f - fTemp43));
			float fTemp47 = (0.0f - (0.5f * fTemp46));
			float fTemp48 = (fTemp39 + (-3.0f - fTemp43));
			float fTemp49 = (0.0f - (0.333333343f * fTemp48));
			float fTemp50 = (fTemp39 + (-4.0f - fTemp43));
			float fTemp51 = (0.0f - (0.25f * fTemp50));
			float fTemp52 = (fTemp39 - fTemp43);
			int iTemp53 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp41 + 1))))) + 1);
			float fTemp54 = (0.0f - fTemp46);
			float fTemp55 = (0.0f - (0.5f * fTemp48));
			float fTemp56 = (0.0f - (0.333333343f * fTemp50));
			int iTemp57 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp41 + 2))))) + 1);
			float fTemp58 = (0.0f - fTemp48);
			float fTemp59 = (0.0f - (0.5f * fTemp50));
			float fTemp60 = (fTemp44 * fTemp46);
			int iTemp61 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp41 + 3))))) + 1);
			float fTemp62 = (0.0f - fTemp50);
			float fTemp63 = (fTemp60 * fTemp48);
			int iTemp64 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp41 + 4))))) + 1);
			fVec5[0] = (((((fVec4[((IOTA - iTemp42) & 2047)] * fTemp45) * fTemp47) * fTemp49) * fTemp51) + (fTemp52 * ((((((fVec4[((IOTA - iTemp53) & 2047)] * fTemp54) * fTemp55) * fTemp56) + (0.5f * (((fTemp44 * fVec4[((IOTA - iTemp57) & 2047)]) * fTemp58) * fTemp59))) + (0.166666672f * ((fTemp60 * fVec4[((IOTA - iTemp61) & 2047)]) * fTemp62))) + (0.0416666679f * (fTemp63 * fVec4[((IOTA - iTemp64) & 2047)])))));
			fRec26[0] = fVec5[1];
			float fRec22 = fRec25;
			float fRec23 = fRec26[0];
			float fRec24 = fRec26[0];
			fRec17[(IOTA & 2047)] = fRec22;
			float fRec18 = (((((fTemp45 * fTemp47) * fTemp49) * fTemp51) * fRec17[((IOTA - iTemp42) & 2047)]) + (fTemp52 * ((((((fTemp54 * fTemp55) * fTemp56) * fRec17[((IOTA - iTemp53) & 2047)]) + (0.5f * (((fTemp44 * fTemp58) * fTemp59) * fRec17[((IOTA - iTemp57) & 2047)]))) + (0.166666672f * ((fTemp60 * fTemp62) * fRec17[((IOTA - iTemp61) & 2047)]))) + (0.0416666679f * (fTemp63 * fRec17[((IOTA - iTemp64) & 2047)])))));
			float fRec19 = fRec23;
			float fRec20 = fRec24;
			fRec13[0] = fRec18;
			float fRec14 = fRec13[1];
			float fRec15 = fRec19;
			float fRec16 = fRec20;
			fRec9[(IOTA & 2047)] = fRec14;
			float fRec10 = (((((fTemp17 * fTemp19) * fTemp21) * fTemp23) * fRec9[((IOTA - iTemp14) & 2047)]) + (fTemp24 * ((((((fTemp26 * fTemp27) * fTemp28) * fRec9[((IOTA - iTemp25) & 2047)]) + (0.5f * (((fTemp16 * fTemp30) * fTemp31) * fRec9[((IOTA - iTemp29) & 2047)]))) + (0.166666672f * ((fTemp32 * fTemp34) * fRec9[((IOTA - iTemp33) & 2047)]))) + (0.0416666679f * (fTemp35 * fRec9[((IOTA - iTemp36) & 2047)])))));
			float fRec11 = fRec15;
			float fRec12 = fRec16;
			fRec7[0] = fRec10;
			fRec8[0] = fRec12;
			fRec6[0] = ((fRec8[0] + (0.995000005f * fRec6[1])) - fRec8[1]);
			iRec49[0] = 0;
			fRec55[0] = ((0.699999988f * fRec55[1]) + (0.284999996f * fRec54[1]));
			float fRec53 = (fRec55[0] + float(iRec49[1]));
			fRec57[0] = (fSlow4 + (0.999000013f * fRec57[1]));
			float fTemp65 = ((std::sqrt(fRec0[0]) * fRec57[0]) + 0.5f);
			fRec56[0] = ((fConst4 * fRec56[1]) + (fConst5 * (fRec29[0] * fTemp65)));
			float fTemp66 = std::tan((fConst19 * ((2500.0f * fTemp65) + 1200.0f)));
			float fTemp67 = (1.0f / fTemp66);
			float fTemp68 = (((fTemp67 + 1.41421354f) / fTemp66) + 1.0f);
			fRec58[0] = (fTemp9 - (((fRec58[2] * (((fTemp67 + -1.41421354f) / fTemp66) + 1.0f)) + (2.0f * (fRec58[1] * (1.0f - (1.0f / mydsp_faustpower2_f(fTemp66)))))) / fTemp68));
			fRec59[0] = fRec35[1];
			fRec60[(IOTA & 2047)] = (0.949999988f * fRec59[1]);
			float fTemp69 = (0.800000012f * (fRec0[0] * mydsp_faustpower2_f(fRec57[0])));
			float fTemp70 = (fConst20 * (fTemp10 * (fTemp69 + 0.270000011f)));
			float fTemp71 = (fTemp70 + -1.49999499f);
			int iTemp72 = int(fTemp71);
			int iTemp73 = (int(std::min<float>(fConst16, float(std::max<int>(0, iTemp72)))) + 1);
			float fTemp74 = std::floor(fTemp71);
			float fTemp75 = (fTemp70 + (-1.0f - fTemp74));
			float fTemp76 = (0.0f - fTemp75);
			float fTemp77 = (fTemp70 + (-2.0f - fTemp74));
			float fTemp78 = (0.0f - (0.5f * fTemp77));
			float fTemp79 = (fTemp70 + (-3.0f - fTemp74));
			float fTemp80 = (0.0f - (0.333333343f * fTemp79));
			float fTemp81 = (fTemp70 + (-4.0f - fTemp74));
			float fTemp82 = (0.0f - (0.25f * fTemp81));
			float fTemp83 = (fTemp70 - fTemp74);
			int iTemp84 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp72 + 1))))) + 1);
			float fTemp85 = (0.0f - fTemp77);
			float fTemp86 = (0.0f - (0.5f * fTemp79));
			float fTemp87 = (0.0f - (0.333333343f * fTemp81));
			int iTemp88 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp72 + 2))))) + 1);
			float fTemp89 = (0.0f - fTemp79);
			float fTemp90 = (0.0f - (0.5f * fTemp81));
			float fTemp91 = (fTemp75 * fTemp77);
			int iTemp92 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp72 + 3))))) + 1);
			float fTemp93 = (0.0f - fTemp81);
			float fTemp94 = (fTemp91 * fTemp79);
			int iTemp95 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp72 + 4))))) + 1);
			fVec6[0] = (((((fRec60[((IOTA - iTemp73) & 2047)] * fTemp76) * fTemp78) * fTemp80) * fTemp82) + (fTemp83 * ((((((fRec60[((IOTA - iTemp84) & 2047)] * fTemp85) * fTemp86) * fTemp87) + (0.5f * (((fTemp75 * fRec60[((IOTA - iTemp88) & 2047)]) * fTemp89) * fTemp90))) + (0.166666672f * ((fTemp91 * fRec60[((IOTA - iTemp92) & 2047)]) * fTemp93))) + (0.0416666679f * (fTemp94 * fRec60[((IOTA - iTemp95) & 2047)])))));
			float fTemp96 = ((fRec56[0] * ((0.0500000007f * ((fRec58[2] + (fRec58[0] + (2.0f * fRec58[1]))) / fTemp68)) + 1.0f)) - (0.5f * fVec6[1]));
			float fTemp97 = ((0.5f * fRec41[1]) + std::max<float>(-1.0f, std::min<float>(1.0f, (fTemp96 * (mydsp_faustpower2_f(fTemp96) + -1.0f)))));
			fVec7[(IOTA & 2047)] = fTemp97;
			float fTemp98 = (fConst20 * (fTemp10 * (0.730000019f - fTemp69)));
			float fTemp99 = (fTemp98 + -1.49999499f);
			int iTemp100 = int(fTemp99);
			int iTemp101 = (int(std::min<float>(fConst16, float(std::max<int>(0, iTemp100)))) + 1);
			float fTemp102 = std::floor(fTemp99);
			float fTemp103 = (fTemp98 + (-1.0f - fTemp102));
			float fTemp104 = (0.0f - fTemp103);
			float fTemp105 = (fTemp98 + (-2.0f - fTemp102));
			float fTemp106 = (0.0f - (0.5f * fTemp105));
			float fTemp107 = (fTemp98 + (-3.0f - fTemp102));
			float fTemp108 = (0.0f - (0.333333343f * fTemp107));
			float fTemp109 = (fTemp98 + (-4.0f - fTemp102));
			float fTemp110 = (0.0f - (0.25f * fTemp109));
			float fTemp111 = (fTemp98 - fTemp102);
			int iTemp112 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp100 + 1))))) + 1);
			float fTemp113 = (0.0f - fTemp105);
			float fTemp114 = (0.0f - (0.5f * fTemp107));
			float fTemp115 = (0.0f - (0.333333343f * fTemp109));
			int iTemp116 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp100 + 2))))) + 1);
			float fTemp117 = (0.0f - fTemp107);
			float fTemp118 = (0.0f - (0.5f * fTemp109));
			float fTemp119 = (fTemp103 * fTemp105);
			int iTemp120 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp100 + 3))))) + 1);
			float fTemp121 = (0.0f - fTemp109);
			float fTemp122 = (fTemp119 * fTemp107);
			int iTemp123 = (int(std::min<float>(fConst16, float(std::max<int>(0, (iTemp100 + 4))))) + 1);
			fVec8[0] = (((((fVec7[((IOTA - iTemp101) & 2047)] * fTemp104) * fTemp106) * fTemp108) * fTemp110) + (fTemp111 * ((((((fVec7[((IOTA - iTemp112) & 2047)] * fTemp113) * fTemp114) * fTemp115) + (0.5f * (((fTemp103 * fVec7[((IOTA - iTemp116) & 2047)]) * fTemp117) * fTemp118))) + (0.166666672f * ((fTemp119 * fVec7[((IOTA - iTemp120) & 2047)]) * fTemp121))) + (0.0416666679f * (fTemp122 * fVec7[((IOTA - iTemp123) & 2047)])))));
			fRec54[0] = fVec8[1];
			float fRec50 = fRec53;
			float fRec51 = fRec54[0];
			float fRec52 = fRec54[0];
			fRec45[(IOTA & 2047)] = fRec50;
			float fRec46 = (((((fTemp104 * fTemp106) * fTemp108) * fTemp110) * fRec45[((IOTA - iTemp101) & 2047)]) + (fTemp111 * ((((((fTemp113 * fTemp114) * fTemp115) * fRec45[((IOTA - iTemp112) & 2047)]) + (0.5f * (((fTemp103 * fTemp117) * fTemp118) * fRec45[((IOTA - iTemp116) & 2047)]))) + (0.166666672f * ((fTemp119 * fTemp121) * fRec45[((IOTA - iTemp120) & 2047)]))) + (0.0416666679f * (fTemp122 * fRec45[((IOTA - iTemp123) & 2047)])))));
			float fRec47 = fRec51;
			float fRec48 = fRec52;
			fRec41[0] = fRec46;
			float fRec42 = fRec41[1];
			float fRec43 = fRec47;
			float fRec44 = fRec48;
			fRec37[(IOTA & 2047)] = fRec42;
			float fRec38 = (((((fTemp76 * fTemp78) * fTemp80) * fTemp82) * fRec37[((IOTA - iTemp73) & 2047)]) + (fTemp83 * ((((((fTemp85 * fTemp86) * fTemp87) * fRec37[((IOTA - iTemp84) & 2047)]) + (0.5f * (((fTemp75 * fTemp89) * fTemp90) * fRec37[((IOTA - iTemp88) & 2047)]))) + (0.166666672f * ((fTemp91 * fTemp93) * fRec37[((IOTA - iTemp92) & 2047)]))) + (0.0416666679f * (fTemp94 * fRec37[((IOTA - iTemp95) & 2047)])))));
			float fRec39 = fRec43;
			float fRec40 = fRec44;
			fRec35[0] = fRec38;
			fRec36[0] = fRec40;
			fRec34[0] = ((fRec36[0] + (0.995000005f * fRec34[1])) - fRec36[1]);
			fRec63[0] = (fTemp9 - (fConst27 * ((fConst30 * fRec63[2]) + (fConst31 * fRec63[1]))));
			fRec62[0] = ((fConst27 * (((fConst29 * fRec63[0]) + (fConst32 * fRec63[1])) + (fConst29 * fRec63[2]))) - (fConst24 * ((fConst33 * fRec62[2]) + (fConst34 * fRec62[1]))));
			fVec9[0] = fSlow5;
			iRec64[0] = (((iRec64[1] + (iRec64[1] > 0)) * (fSlow5 <= fVec9[1])) + (fSlow5 > fVec9[1]));
			float fTemp124 = float(iRec64[0]);
			float fTemp125 = (fConst35 * fTemp124);
			float fTemp126 = (fConst24 * ((fRec62[2] + (fRec62[0] + (2.0f * fRec62[1]))) * std::max<float>(0.0f, std::min<float>(fTemp125, (2.0f - fTemp125)))));
			fRec61[0] = (fTemp126 - ((fConst37 * fRec61[1]) + (fConst38 * fRec61[2])));
			fRec65[0] = (fTemp126 - ((fConst41 * fRec65[1]) + (fConst42 * fRec65[2])));
			fRec66[0] = (fTemp126 - ((fConst45 * fRec66[1]) + (fConst46 * fRec66[2])));
			fRec67[0] = (fTemp126 - ((fConst49 * fRec67[1]) + (fConst50 * fRec67[2])));
			fRec68[0] = (fTemp126 - ((fConst53 * fRec68[1]) + (fConst54 * fRec68[2])));
			fRec69[0] = (fTemp126 - ((fConst57 * fRec69[1]) + (fConst58 * fRec69[2])));
			fRec70[0] = (fTemp126 - ((fConst61 * fRec70[1]) + (fConst62 * fRec70[2])));
			fRec71[0] = (fTemp126 - ((fConst65 * fRec71[1]) + (fConst66 * fRec71[2])));
			fRec72[0] = (fTemp126 - ((fConst69 * fRec72[1]) + (fConst70 * fRec72[2])));
			fRec73[0] = (fTemp126 - ((fConst73 * fRec73[1]) + (fConst74 * fRec73[2])));
			fRec74[0] = (fTemp126 - ((fConst77 * fRec74[1]) + (fConst78 * fRec74[2])));
			fRec75[0] = (fTemp126 - ((fConst81 * fRec75[1]) + (fConst82 * fRec75[2])));
			fRec76[0] = (fTemp126 - ((fConst85 * fRec76[1]) + (fConst86 * fRec76[2])));
			fRec77[0] = (fTemp126 - ((fConst89 * fRec77[1]) + (fConst90 * fRec77[2])));
			fRec78[0] = (fTemp126 - ((fConst93 * fRec78[1]) + (fConst94 * fRec78[2])));
			fRec79[0] = (fTemp126 - ((fConst97 * fRec79[1]) + (fConst98 * fRec79[2])));
			fRec80[0] = (fTemp126 - ((fConst101 * fRec80[1]) + (fConst102 * fRec80[2])));
			fRec81[0] = (fTemp126 - ((fConst105 * fRec81[1]) + (fConst106 * fRec81[2])));
			fRec82[0] = (fTemp126 - ((fConst109 * fRec82[1]) + (fConst110 * fRec82[2])));
			fRec83[0] = (fTemp126 - ((fConst113 * fRec83[1]) + (fConst114 * fRec83[2])));
			fRec84[0] = (fTemp126 - ((fConst117 * fRec84[1]) + (fConst118 * fRec84[2])));
			fRec85[0] = (fTemp126 - ((fConst121 * fRec85[1]) + (fConst122 * fRec85[2])));
			fRec86[0] = (fTemp126 - ((fConst125 * fRec86[1]) + (fConst126 * fRec86[2])));
			fRec87[0] = (fTemp126 - ((fConst129 * fRec87[1]) + (fConst130 * fRec87[2])));
			fRec88[0] = (fTemp126 - ((fConst133 * fRec88[1]) + (fConst134 * fRec88[2])));
			fRec89[0] = (fTemp126 - ((fConst137 * fRec89[1]) + (fConst138 * fRec89[2])));
			fRec90[0] = (fTemp126 - ((fConst141 * fRec90[1]) + (fConst142 * fRec90[2])));
			fRec91[0] = (fTemp126 - ((fConst145 * fRec91[1]) + (fConst146 * fRec91[2])));
			fRec92[0] = (fTemp126 - ((fConst149 * fRec92[1]) + (fConst150 * fRec92[2])));
			fRec93[0] = (fTemp126 - ((fConst153 * fRec93[1]) + (fConst154 * fRec93[2])));
			fRec94[0] = (fTemp126 - ((fConst157 * fRec94[1]) + (fConst158 * fRec94[2])));
			fRec95[0] = (fTemp126 - ((fConst161 * fRec95[1]) + (fConst162 * fRec95[2])));
			fRec96[0] = (fTemp126 - ((fConst165 * fRec96[1]) + (fConst166 * fRec96[2])));
			fRec97[0] = (fTemp126 - ((fConst169 * fRec97[1]) + (fConst170 * fRec97[2])));
			fRec98[0] = (fTemp126 - ((fConst173 * fRec98[1]) + (fConst174 * fRec98[2])));
			fRec99[0] = (fTemp126 - ((fConst177 * fRec99[1]) + (fConst178 * fRec99[2])));
			fRec100[0] = (fTemp126 - ((fConst181 * fRec100[1]) + (fConst182 * fRec100[2])));
			fRec101[0] = (fTemp126 - ((fConst185 * fRec101[1]) + (fConst186 * fRec101[2])));
			fRec102[0] = (fTemp126 - ((fConst189 * fRec102[1]) + (fConst190 * fRec102[2])));
			fRec103[0] = (fTemp126 - ((fConst193 * fRec103[1]) + (fConst194 * fRec103[2])));
			fRec104[0] = (fTemp126 - ((fConst197 * fRec104[1]) + (fConst198 * fRec104[2])));
			fRec105[0] = (fTemp126 - ((fConst201 * fRec105[1]) + (fConst202 * fRec105[2])));
			fRec106[0] = (fTemp126 - ((fConst205 * fRec106[1]) + (fConst206 * fRec106[2])));
			fRec107[0] = (fTemp126 - ((fConst209 * fRec107[1]) + (fConst210 * fRec107[2])));
			fRec108[0] = (fTemp126 - ((fConst213 * fRec108[1]) + (fConst214 * fRec108[2])));
			fRec109[0] = (fTemp126 - ((fConst217 * fRec109[1]) + (fConst218 * fRec109[2])));
			fRec110[0] = (fTemp126 - ((fConst221 * fRec110[1]) + (fConst222 * fRec110[2])));
			fRec111[0] = (fTemp126 - ((fConst225 * fRec111[1]) + (fConst226 * fRec111[2])));
			fRec112[0] = (fTemp126 - ((fConst229 * fRec112[1]) + (fConst230 * fRec112[2])));
			fRec113[0] = (fTemp126 - ((fConst233 * fRec113[1]) + (fConst234 * fRec113[2])));
			float fTemp127 = std::max<float>(-1.0f, std::min<float>(1.0f, (std::pow(10.0f, (2.0f * (fRec0[0] * fRec1[0]))) * ((fRec0[0] * ((fConst1 * ((fRec2[0] * fRec5[0]) * fTemp0)) + (0.25f * (fRec1[0] * fRec6[0])))) + ((fRec34[0] * mydsp_faustpower2_f(fTemp65)) + (0.0199999996f * (((((((((((((((((((((((((((((((((((((((((((((((((((fConst21 * (fRec61[0] - fRec61[2])) + (fConst39 * (fRec65[0] - fRec65[2]))) + (fConst43 * (fRec66[0] - fRec66[2]))) + (fConst47 * (fRec67[0] - fRec67[2]))) + (fConst51 * (fRec68[0] - fRec68[2]))) + (fConst55 * (fRec69[0] - fRec69[2]))) + (fConst59 * (fRec70[0] - fRec70[2]))) + (fConst63 * (fRec71[0] - fRec71[2]))) + (fConst67 * (fRec72[0] - fRec72[2]))) + (fConst71 * (fRec73[0] - fRec73[2]))) + (fConst75 * (fRec74[0] - fRec74[2]))) + (fConst79 * (fRec75[0] - fRec75[2]))) + (fConst83 * (fRec76[0] - fRec76[2]))) + (fConst87 * (fRec77[0] - fRec77[2]))) + (fConst91 * (fRec78[0] - fRec78[2]))) + (fConst95 * (fRec79[0] - fRec79[2]))) + (fConst99 * (fRec80[0] - fRec80[2]))) + (fConst103 * (fRec81[0] - fRec81[2]))) + (fConst107 * (fRec82[0] - fRec82[2]))) + (fConst111 * (fRec83[0] - fRec83[2]))) + (fConst115 * (fRec84[0] - fRec84[2]))) + (fConst119 * (fRec85[0] - fRec85[2]))) + (fConst123 * (fRec86[0] - fRec86[2]))) + (fConst127 * (fRec87[0] - fRec87[2]))) + (fConst131 * (fRec88[0] - fRec88[2]))) + (fConst135 * (fRec89[0] - fRec89[2]))) + (fConst139 * (fRec90[0] - fRec90[2]))) + (fConst143 * (fRec91[0] - fRec91[2]))) + (fConst147 * (fRec92[0] - fRec92[2]))) + (fConst151 * (fRec93[0] - fRec93[2]))) + (fConst155 * (fRec94[0] - fRec94[2]))) + (fConst159 * (fRec95[0] - fRec95[2]))) + (fConst163 * (fRec96[0] - fRec96[2]))) + (fConst167 * (fRec97[0] - fRec97[2]))) + (fConst171 * (fRec98[0] - fRec98[2]))) + (fConst175 * (fRec99[0] - fRec99[2]))) + (fConst179 * (fRec100[0] - fRec100[2]))) + (fConst183 * (fRec101[0] - fRec101[2]))) + (fConst187 * (fRec102[0] - fRec102[2]))) + (fConst191 * (fRec103[0] - fRec103[2]))) + (fConst195 * (fRec104[0] - fRec104[2]))) + (fConst199 * (fRec105[0] - fRec105[2]))) + (fConst203 * (fRec106[0] - fRec106[2]))) + (fConst207 * (fRec107[0] - fRec107[2]))) + (fConst211 * (fRec108[0] - fRec108[2]))) + (fConst215 * (fRec109[0] - fRec109[2]))) + (fConst219 * (fRec110[0] - fRec110[2]))) + (fConst223 * (fRec111[0] - fRec111[2]))) + (fConst227 * (fRec112[0] - fRec112[2]))) + (fConst231 * (fRec113[0] - fRec113[2]))) * std::max<float>(0.0f, std::min<float>((fConst236 * fTemp124), ((fConst237 * (fConst235 - fTemp124)) + 1.0f))))))))));
			fRec114[0] = (fSlow6 + (0.999000013f * fRec114[1]));
			float fTemp128 = ((fTemp127 * (1.0f - (0.333333343f * mydsp_faustpower2_f(fTemp127)))) * std::pow(10.0f, (0.0500000007f * fRec114[0])));
			output0[i] = FAUSTFLOAT(fTemp128);
			output1[i] = FAUSTFLOAT(fTemp128);
			iVec0[1] = iVec0[0];
			fRec0[1] = fRec0[0];
			fRec1[1] = fRec1[0];
			fRec4[1] = fRec4[0];
			fRec3[1] = fRec3[0];
			fVec1[1] = fVec1[0];
			IOTA = (IOTA + 1);
			fRec2[1] = fRec2[0];
			fRec5[1] = fRec5[0];
			iRec21[1] = iRec21[0];
			fRec27[1] = fRec27[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			iRec31[1] = iRec31[0];
			fRec30[2] = fRec30[1];
			fRec30[1] = fRec30[0];
			fRec32[1] = fRec32[0];
			fVec3[1] = fVec3[0];
			fVec5[1] = fVec5[0];
			fRec26[1] = fRec26[0];
			fRec13[1] = fRec13[0];
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec6[1] = fRec6[0];
			iRec49[1] = iRec49[0];
			fRec55[1] = fRec55[0];
			fRec57[1] = fRec57[0];
			fRec56[1] = fRec56[0];
			fRec58[2] = fRec58[1];
			fRec58[1] = fRec58[0];
			fRec59[1] = fRec59[0];
			fVec6[1] = fVec6[0];
			fVec8[1] = fVec8[0];
			fRec54[1] = fRec54[0];
			fRec41[1] = fRec41[0];
			fRec35[1] = fRec35[0];
			fRec36[1] = fRec36[0];
			fRec34[1] = fRec34[0];
			fRec63[2] = fRec63[1];
			fRec63[1] = fRec63[0];
			fRec62[2] = fRec62[1];
			fRec62[1] = fRec62[0];
			fVec9[1] = fVec9[0];
			iRec64[1] = iRec64[0];
			fRec61[2] = fRec61[1];
			fRec61[1] = fRec61[0];
			fRec65[2] = fRec65[1];
			fRec65[1] = fRec65[0];
			fRec66[2] = fRec66[1];
			fRec66[1] = fRec66[0];
			fRec67[2] = fRec67[1];
			fRec67[1] = fRec67[0];
			fRec68[2] = fRec68[1];
			fRec68[1] = fRec68[0];
			fRec69[2] = fRec69[1];
			fRec69[1] = fRec69[0];
			fRec70[2] = fRec70[1];
			fRec70[1] = fRec70[0];
			fRec71[2] = fRec71[1];
			fRec71[1] = fRec71[0];
			fRec72[2] = fRec72[1];
			fRec72[1] = fRec72[0];
			fRec73[2] = fRec73[1];
			fRec73[1] = fRec73[0];
			fRec74[2] = fRec74[1];
			fRec74[1] = fRec74[0];
			fRec75[2] = fRec75[1];
			fRec75[1] = fRec75[0];
			fRec76[2] = fRec76[1];
			fRec76[1] = fRec76[0];
			fRec77[2] = fRec77[1];
			fRec77[1] = fRec77[0];
			fRec78[2] = fRec78[1];
			fRec78[1] = fRec78[0];
			fRec79[2] = fRec79[1];
			fRec79[1] = fRec79[0];
			fRec80[2] = fRec80[1];
			fRec80[1] = fRec80[0];
			fRec81[2] = fRec81[1];
			fRec81[1] = fRec81[0];
			fRec82[2] = fRec82[1];
			fRec82[1] = fRec82[0];
			fRec83[2] = fRec83[1];
			fRec83[1] = fRec83[0];
			fRec84[2] = fRec84[1];
			fRec84[1] = fRec84[0];
			fRec85[2] = fRec85[1];
			fRec85[1] = fRec85[0];
			fRec86[2] = fRec86[1];
			fRec86[1] = fRec86[0];
			fRec87[2] = fRec87[1];
			fRec87[1] = fRec87[0];
			fRec88[2] = fRec88[1];
			fRec88[1] = fRec88[0];
			fRec89[2] = fRec89[1];
			fRec89[1] = fRec89[0];
			fRec90[2] = fRec90[1];
			fRec90[1] = fRec90[0];
			fRec91[2] = fRec91[1];
			fRec91[1] = fRec91[0];
			fRec92[2] = fRec92[1];
			fRec92[1] = fRec92[0];
			fRec93[2] = fRec93[1];
			fRec93[1] = fRec93[0];
			fRec94[2] = fRec94[1];
			fRec94[1] = fRec94[0];
			fRec95[2] = fRec95[1];
			fRec95[1] = fRec95[0];
			fRec96[2] = fRec96[1];
			fRec96[1] = fRec96[0];
			fRec97[2] = fRec97[1];
			fRec97[1] = fRec97[0];
			fRec98[2] = fRec98[1];
			fRec98[1] = fRec98[0];
			fRec99[2] = fRec99[1];
			fRec99[1] = fRec99[0];
			fRec100[2] = fRec100[1];
			fRec100[1] = fRec100[0];
			fRec101[2] = fRec101[1];
			fRec101[1] = fRec101[0];
			fRec102[2] = fRec102[1];
			fRec102[1] = fRec102[0];
			fRec103[2] = fRec103[1];
			fRec103[1] = fRec103[0];
			fRec104[2] = fRec104[1];
			fRec104[1] = fRec104[0];
			fRec105[2] = fRec105[1];
			fRec105[1] = fRec105[0];
			fRec106[2] = fRec106[1];
			fRec106[1] = fRec106[0];
			fRec107[2] = fRec107[1];
			fRec107[1] = fRec107[0];
			fRec108[2] = fRec108[1];
			fRec108[1] = fRec108[0];
			fRec109[2] = fRec109[1];
			fRec109[1] = fRec109[0];
			fRec110[2] = fRec110[1];
			fRec110[1] = fRec110[0];
			fRec111[2] = fRec111[1];
			fRec111[1] = fRec111[0];
			fRec112[2] = fRec112[1];
			fRec112[1] = fRec112[0];
			fRec113[2] = fRec113[1];
			fRec113[1] = fRec113[0];
			fRec114[1] = fRec114[0];
		}
	}

};

#endif
