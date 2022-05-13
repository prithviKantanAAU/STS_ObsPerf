#pragma once
using namespace juce;
#include <JuceHeader.h>
#include "IMU_Sensor.h"
#include <ctime>
#include "windows.h"

class UI_ControlGroup
{
public:
	String* name = new String("");
	short* numControls = 0;

	// Border Labels
	Label* border_L, * border_R, * border_D, * border_UL, * border_UR, * groupName;
	int border_Thickness = 2;
	Colour* borderColour;
	juce::Rectangle<int>* boundingRec;

	int px_Xstart = 0;
	int px_Ystart = 0;
	int px_wd = 0;
	int px_ht = 0;

	UI_ControlGroup(String navn, Colour col)
	{
		*name = navn;
		borderColour = &col;
		groupName = new Label;
		border_D = new Label;
		border_L = new Label;
		border_R = new Label;
		border_UL = new Label;
		border_UR = new Label;
		boundingRec = new juce::Rectangle<int>;

		groupName->setText(*name, dontSendNotification);
		groupName->setJustificationType(Justification::centred);
		groupName->setColour(groupName->backgroundColourId, col);
		border_L->setColour(border_L->backgroundColourId, col);
		border_R->setColour(border_R->backgroundColourId, col);
		border_D->setColour(border_D->backgroundColourId, col);
		border_UL->setColour(border_UL->backgroundColourId, col);
		border_UR->setColour(border_UR->backgroundColourId, col);
	}

	~UI_ControlGroup()
	{
	}

	void getBoundingRec(int win_Wd, int win_Ht, float frac_Xstart, float frac_Ystart, float frac_wd, float frac_ht)
	{
		px_Xstart = frac_Xstart * win_Wd;
		px_Ystart = frac_Ystart * win_Ht;
		px_wd = frac_wd * win_Wd;
		px_ht = frac_ht * win_Ht;

		*boundingRec = juce::Rectangle<int>(px_Xstart, px_Ystart, px_wd, px_ht);
		setRecBounds();
	}

	void setRecBounds()
	{
		border_L->setBounds(
			boundingRec->getTopLeft().getX(),
			boundingRec->getTopLeft().getY(),
			border_Thickness,
			boundingRec->getHeight()
		);

		border_R->setBounds(
			boundingRec->getTopRight().getX(),
			boundingRec->getTopRight().getY(),
			border_Thickness,
			boundingRec->getHeight()
		);

		border_D->setBounds(
			boundingRec->getBottomLeft().getX(),
			boundingRec->getBottomLeft().getY(),
			boundingRec->getWidth(),
			border_Thickness
		);

		border_UL->setBounds(
			boundingRec->getTopLeft().getX(),
			boundingRec->getTopLeft().getY(),
			boundingRec->getWidth() / 3,
			border_Thickness
		);

		border_UR->setBounds(
			boundingRec->getTopLeft().getX() + 2 * boundingRec->getWidth() / 3,
			boundingRec->getTopLeft().getY(),
			boundingRec->getWidth() / 3,
			border_Thickness
		);

		groupName->setBounds(
			boundingRec->getTopLeft().getX() + 1 * boundingRec->getWidth() / 3,
			boundingRec->getTopLeft().getY() - 8,
			boundingRec->getWidth() / 3,
			16
		);
	}

	void frac_to_Px(int* startX_Px, int* startY_Px, int* wd_Px, int* ht_Px, float startX_FRAC, float startY_FRAC, float wd_FRAC, float ht_FRAC)
	{
		*startX_Px = (int)px_Xstart + px_wd * startX_FRAC;
		*startY_Px = (int)px_Ystart + px_ht * startY_FRAC;
		*wd_Px = (int)px_wd * wd_FRAC;
		*ht_Px = (int)px_ht * ht_FRAC;
	}

	void memberComponent_setBounds(Component* comp, float startX_FRAC, float startY_FRAC, float wd_FRAC, float ht_FRAC)
	{
		int startX_Px = px_Xstart + px_wd * startX_FRAC;
		int startY_Px = px_Ystart + px_ht * startY_FRAC;
		int wd_Px = px_wd * wd_FRAC;
		int ht_Px = px_ht * ht_FRAC;

		comp->setBounds(startX_Px, startY_Px, wd_Px, ht_Px);
	}

	void setVisible(bool on)
	{
		border_L->setVisible(on);
		border_R->setVisible(on);
		border_D->setVisible(on);
		border_UL->setVisible(on);
		border_UR->setVisible(on);
		groupName->setVisible(on);
	}

	void getRelativePos_TopCorner_fromCenter(float labelWd_Px, float labelHt_Px, float centerX_Rel, float centerY_Rel, float* topCornerX_Rel, float* topCornerY_Rel, float* wd_Rel, float* ht_Rel)
	{
		*wd_Rel = labelWd_Px / px_wd;
		*ht_Rel = labelHt_Px / px_ht;
		float offset_X = -0.5 * *wd_Rel;
		float offset_Y = -0.5 * *ht_Rel;
		*topCornerX_Rel = centerX_Rel + offset_X;
		*topCornerY_Rel = centerY_Rel + offset_Y;
	}

	void getRelativePos_TopCenter_fromCenter(float labelWd_Px, float labelHt_Px, float centerX_Rel, float centerY_Rel, float* topCenterX_Rel, float* topCenterY_Rel)
	{
		float offset_Y = -0.5 * (labelHt_Px / px_ht);
		*topCenterX_Rel = centerX_Rel;
		*topCenterY_Rel = centerY_Rel + offset_Y;
	}

	void getRelativePos_BottomCenter_fromCenter(float labelWd_Px, float labelHt_Px, float centerX_Rel, float centerY_Rel, float* bottomCenterX_Rel, float* bottomCenterY_Rel)
	{
		float offset_Y = 0.5 * (labelHt_Px / px_ht);
		*bottomCenterX_Rel = centerX_Rel;
		*bottomCenterY_Rel = centerY_Rel + offset_Y;
	}

	void getRelativePos_BottomFrac_from_Center(float labelWd_Px, float labelHt_Px, float centerX_Rel, float centerY_Rel, float* bottomPtX_Rel, float* bottomPtY_Rel, float fracX)
	{
		float wd_Rel = labelWd_Px / px_wd;
		float ht_Rel = labelHt_Px / px_ht;
		float offset_X = (-0.5 + fracX) * wd_Rel;
		float offset_Y = 0.5 * ht_Rel;
		*bottomPtX_Rel = centerX_Rel + offset_X;
		*bottomPtY_Rel = centerY_Rel + offset_Y;
	}
};

class MovementFeature
{
public:

	MovementFeature(String name, float mini, float maxi, float value, bool isVis = true)
	{
		mpName = name;
		minVal = mini;
		maxVal = maxi;
		isVisible = isVis;
		val = value;

		nameDisp = new Label("",name);
		nameDisp->setJustificationType(Justification::centred);
	}

	~MovementFeature()
	{

	}

	// MEMBER VARIABLES
	double minVal = 0;
	double maxVal = 0;
	String mpName = "PLACEHOLDER";
	bool isVisible = false;
	bool inRange = false;
	float val = 0;
	Label* nameDisp;

	void storeValue(double newVal)
	{
		if (!isnan(newVal))
			val = jlimit(minVal, maxVal, newVal);
		else val = minVal;
	}
};

class AudioParameter_FAUST
{
public:
	String name = "Placeholder";
	float minVal = 0;
	float maxVal = 1;
	float defaultVal = 0;
	float val = 0;
	float mapVals[4] = { 0.0,0.0,0.0,0.0 };
	int numSliders = 1;
	int sliderIdx;
	bool isVisible = false;
	Label* nameDisp;

	// IF DIRECT MAPPING OF OUTPUT VALS, THEN FINAL MAPPING VALS
	// IF TRANSFORMED MAPPING, INTEGER STEPS (e.g. SCALE POSITION)

	AudioParameter_FAUST(String navn, float mini, float maxi, float defValue, int idx_FaustSlider, int numSld = 1, bool isVis = true)
	{
		name = navn;
		minVal = mini;
		maxVal = maxi;
		defaultVal = defValue;
		sliderIdx = idx_FaustSlider;
		isVisible = isVis;
		nameDisp = new Label("",name);
		numSliders = numSld;
		nameDisp->setJustificationType(Justification::centred);
	}

	~AudioParameter_FAUST()
	{

	}


	void storeValue(float newVal)
	{
		if (!isnan(newVal))
			val = jlimit(minVal, maxVal, newVal);
		else val = minVal;
	}
};

class MovementRangeSlider
{
public:
	MovementRangeSlider(float minVal, float maxVal)
	{
		// INITIALIZE SLIDER BEHAVIOR
		range = new Slider;
		bounds[0] = new Label;
		bounds[1] = new Label;
		indicator = new Label;
		range->setSliderStyle(Slider::SliderStyle::TwoValueHorizontal);
		range->setColour(range->trackColourId, Colours::yellow);
		range->setColour(range->backgroundColourId, Colours::blue);
		range->setRange(minVal, maxVal);
		range->setMinAndMaxValues(minVal, maxVal, sendNotificationSync);
		range->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		indicator->setColour(indicator->backgroundColourId, Colours::yellow);

		bounds[0]->setText(String(range->getRange().getStart(), 2), dontSendNotification);
		bounds[1]->setText(String(range->getRange().getEnd(), 2), dontSendNotification);

		range->onValueChange = [this]
		{
			VAL_bounds[0] = range->getMinValue();
			VAL_bounds[1] = range->getMaxValue();

			bounds[0]->setText(String(VAL_bounds[0], 2), dontSendNotification);
			bounds[1]->setText(String(VAL_bounds[1], 2), dontSendNotification);
		};
	}

	~MovementRangeSlider()
	{

	}

	void updateIndicator(float currentVal)				// IN REAL TIME
	{
		float lengthFrac = (currentVal - range->getRange().getStart()) / 
			(range->getRange().getEnd() - range->getRange().getStart());

		float indicatorX = slider_startX + lengthFrac * (slider_endX - slider_startX) - indicator_Side_Px / 2;

		indicator->setBounds(indicatorX, slider_startY - indicator_Side_Px, indicator_Side_Px, indicator_Side_Px);
	}

	void setVisible(bool on)
	{
		range->setVisible(on);
		bounds[0]->setVisible(on);
		bounds[1]->setVisible(on);
		indicator->setVisible(on);
	}

	void setBounds(float startX, float startY, float wd, float ht)
	{
		slider_startX = startX + 0.2 * wd;
		slider_startY = startY;
		slider_endX = startX + 0.8 * wd;

		bounds[0]->setBounds(startX + 10, startY, 0.2 * wd, ht);
		range->setBounds(slider_startX, startY, 0.6 * wd, ht);
		bounds[1]->setBounds(startX + 0.8 * wd, startY, 0.2 * wd, ht);
	}

	Slider* range;
	Label* bounds[2];
	Label* indicator;

	float VAL_bounds[2] = { 0.0,0.0 };

	float indicator_Side_Px = 4;
	float slider_startX = 0;
	float slider_startY = 0;
	float slider_endX = 0;
};

class MovementRangeSlider_wOFFSET
{
public:
	MovementRangeSlider_wOFFSET(float minVal, float maxVal)
	{
		// INITIALIZE SLIDER BEHAVIOR
		range = new Slider;
		bounds[0] = new Label;
		bounds[1] = new Label;
		indicator = new Label;
		indicator_ZERO = new Label;
		range->setSliderStyle(Slider::SliderStyle::ThreeValueHorizontal);
		range->setColour(range->trackColourId, Colours::yellow);
		range->setColour(range->backgroundColourId, Colours::yellow);
		range->setRange(minVal, maxVal);
		range->setMinAndMaxValues(minVal, maxVal, sendNotificationSync);
		range->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		indicator->setColour(indicator->backgroundColourId, Colours::yellow);
		indicator_ZERO->setColour(indicator_ZERO->backgroundColourId, Colours::white);

		bounds[0]->setText(String(range->getRange().getStart(), 2), dontSendNotification);
		bounds[1]->setText(String(range->getRange().getEnd(), 2), dontSendNotification);

		range->onValueChange = [this]
		{
			VAL_bounds[0] = range->getMinValue();
			VAL_bounds[1] = range->getMaxValue();
			offset = range->getValue();

			bounds[0]->setText(String(VAL_bounds[0], 2), dontSendNotification);
			bounds[1]->setText(String(VAL_bounds[1], 2), dontSendNotification);
		};
	}

	~MovementRangeSlider_wOFFSET()
	{

	}

	void updateIndicator(float currentVal)				// IN REAL TIME
	{
		float lengthFrac = (currentVal - range->getRange().getStart()) /
			(range->getRange().getEnd() - range->getRange().getStart());

		float indicatorX = slider_startX + lengthFrac * (slider_endX - slider_startX) - indicator_Side_Px / 2;
		float indicatorX_ZERO = slider_startX + 0.5 * (slider_endX - slider_startX) - indicator_Side_Px / 2;

		indicator->setBounds(indicatorX, slider_startY - indicator_Side_Px, indicator_Side_Px, indicator_Side_Px);
		indicator_ZERO->setBounds(indicatorX_ZERO, slider_startY - indicator_Side_Px, indicator_Side_Px, indicator_Side_Px);
	}

	void setVisible(bool on)
	{
		range->setVisible(on);
		bounds[0]->setVisible(on);
		bounds[1]->setVisible(on);
		indicator->setVisible(on);
		indicator_ZERO->setVisible(on);
	}

	void setBounds(float startX, float startY, float wd, float ht)
	{
		slider_startX = startX + 0.2 * wd;
		slider_startY = startY;
		slider_endX = startX + 0.8 * wd;

		bounds[0]->setBounds(startX + 10, startY, 0.2 * wd, ht);
		range->setBounds(slider_startX, startY, 0.6 * wd, ht);
		bounds[1]->setBounds(startX + 0.8 * wd, startY, 0.2 * wd, ht);
	}

	Slider* range;
	Label* bounds[2];
	Label* indicator;
	Label* indicator_ZERO;

	float VAL_bounds[2] = { 0.0,0.0 };
	float offset = 0;
	float indicator_Side_Px = 4;
	float slider_startX = 0;
	float slider_startY = 0;
	float slider_endX = 0;
};

class SensorLog_Handling 
{
public:
	SensorLog_Handling() : progBar(progress_Val)
	{
		record = new TextButton;
		load = new TextButton;
		unload = new TextButton;
		playPause = new TextButton;
		rwd = new TextButton;
		fwd = new TextButton;
		smpl_Idx_Strt = new TextEditor;
		smpl_Idx_End = new TextEditor;

		smpl_Idx_Strt->setJustification(Justification::centred);
		smpl_Idx_Strt->setColour(smpl_Idx_Strt->backgroundColourId, Colours::black);
		smpl_Idx_Strt->onTextChange = [this]
		{
			smpl_Strt = smpl_Idx_Strt->getText().getIntValue();
		};

		smpl_Idx_End->setJustification(Justification::centred);
		smpl_Idx_End->setColour(smpl_Idx_End->backgroundColourId, Colours::black);
		smpl_Idx_End->onTextChange = [this]
		{
			smpl_End = smpl_Idx_End->getText().getIntValue();
		};
	}

	~SensorLog_Handling()
	{}

	TextButton* record, * load, * unload, * playPause, * rwd, * fwd;
	ProgressBar progBar;
	TextEditor* smpl_Idx_Strt, *smpl_Idx_End;
	String videoURL = "";
	double progress_Val = 0;
	bool toPlay_Bell = false;

	short numCol = 6;
	long numSmpl_TOT = 100000000;
	long numSmpl_NOW = 0;
	long smpl_Strt = 0;
	long smpl_End = 100000000;

	enum modes { SENSOR_NO_FILE_LOAD = 1, SENSOR_FILE_LOAD = 2, FILE_LOADED = 3 };

	short mode = 1;

	String rec_LogPath = "";

	std::vector<String> TR;
	std::vector<String> TH;
	std::vector<String> SH;
	std::vector<long> logSizes;
	std::vector<String> offsetsVector;

	std::vector<String> pathExtensions;
	bool logFound[10];
	
	void addExtension(String* ext)
	{
		pathExtensions.push_back(*ext);
	}

	void flushAllVectors()
	{
		TR.clear();
		TH.clear();
		SH.clear();
		numSmpl_NOW = 0;
		numSmpl_TOT = 100000000;
	}

	bool loadSensorLogs_FromDir(String dirPath)
	{
		bool success = false;
		std::vector<File> imuLogFiles;
		logSizes.clear();
		numSmpl_TOT = 100000000;

		// LOAD OFFSETS
		File* offsetsFilePtr;
		String offsetsFilePath = dirPath + "\\Sensor Offsets Log.csv";
		offsetsFilePtr = new File(offsetsFilePath);
		FileInputStream offsetsLogStream(*offsetsFilePtr);
		if (offsetsLogStream.openedOk())
		{
			while (!offsetsLogStream.isExhausted())
			{
				String offsetLogLine = offsetsLogStream.readNextLine();
				offsetsVector.push_back(offsetLogLine);
			}
		}

		// CREATE FILE INSTANCES FOR ALL POSSIBLE LOGS
		for (String& iter : pathExtensions)
		{
			File* filePtr;
			String fullPath = dirPath + iter;
			filePtr = new File(fullPath);
			imuLogFiles.push_back(*filePtr);
			logSizes.push_back(0);
		}

		TR.clear();
		TH.clear();
		SH.clear();
		numSmpl_NOW = 0;

		for (int i = 0; i < 10; i++) logFound[i] = false;
		short fileIdx = 0;

		// ITERATE ACROSS ALL FILES
		for (File& iter : imuLogFiles)
		{
			FileInputStream imuLogStream(iter);

			if (iter.getFullPathName().contains("Trunk"))
				fileIdx = 1;
			if (iter.getFullPathName().contains("Thigh"))
				fileIdx = 2;
			if (iter.getFullPathName().contains("Shank"))
				fileIdx = 3;
			if (iter.getFullPathName().contains("Motion Video"))
			{
				fileIdx = 4;
				videoURL = iter.getFullPathName();
			}

			if (imuLogStream.openedOk() && fileIdx != 4)
			{
				logFound[fileIdx - 1] = true;;
				success = true;
				// PUT LINES INTO VECTORS
				while (!imuLogStream.isExhausted())
				{
					String newLine = imuLogStream.readNextLine();
					
					switch (fileIdx)
					{
					case 1:
						TR.push_back(newLine);
						logSizes[0]++;
						break;
					case 2:
						TH.push_back(newLine);
						logSizes[1]++;
						break;
					case 3:
						SH.push_back(newLine);
						logSizes[2]++;
						break;
					}
				}
			}

			else
			{
				logFound[fileIdx - 1] = false;
			}
		}

		for (long& iter : logSizes)
		{
			if (iter > 0 && iter <= numSmpl_TOT)
				numSmpl_TOT = iter;
		}
		
		smpl_Strt = 0;
		smpl_End = numSmpl_TOT;

		smpl_Idx_Strt->setText(String(smpl_Strt));
		smpl_Idx_End->setText(String(smpl_End));

		return success;
	}

	String streamSensorLog_Line(String location)
	{
		String out = "";
		
		if (numSmpl_TOT > 1)
		{
			if (location == "Trunk" && logFound[0]) out = TR[numSmpl_NOW];
			if (location == "Thigh" && logFound[1]) out = TH[numSmpl_NOW];
			if (location == "Shank" && logFound[2]) out = SH[numSmpl_NOW];
		}
		return out;
	}

	void incrementLogIdx()
	{
		numSmpl_NOW = (numSmpl_NOW + 1) % numSmpl_TOT;
		if (numSmpl_NOW >= smpl_End) numSmpl_NOW = smpl_Strt;
		if (numSmpl_NOW < smpl_Strt) numSmpl_NOW = smpl_Strt;
		progress_Val = (double)numSmpl_NOW / (double)numSmpl_TOT;
	}

	void setVisible(bool on)
	{
		record->setVisible(on);
		load->setVisible(on);
		unload->setVisible(on);
		playPause->setVisible(on);
		rwd->setVisible(on);
		fwd->setVisible(on);
		progBar.setVisible(on);
		smpl_Idx_Strt->setVisible(on);
		smpl_Idx_End->setVisible(on);

		if (on)
		{
			switch (mode)
			{
			case SENSOR_NO_FILE_LOAD:
				unload->setVisible(false);
				playPause->setVisible(false);
				rwd->setVisible(false);
				fwd->setVisible(false);
				progBar.setVisible(false);
				smpl_Idx_Strt->setVisible(false);
				smpl_Idx_End->setVisible(false);
				break;
			case SENSOR_FILE_LOAD:
				load->setVisible(false);
				break;
			case FILE_LOADED:
				load->setVisible(false);
				unload->setVisible(false);
				break;
			}
		}
	}
};



class Differentiator
{
public:
	Differentiator()
	{
	}

	~Differentiator()
	{

	}

	float fs = 100;
	int order = 1;
	double ordered_Differences[10];

	void setParams(int ord, float sampleRate = 100)
	{
		fs = sampleRate;
		order = ord;
	}

	double process(float inputVal)
	{
		double stageOutput = 0;
		double stageInput = inputVal;
		for (int i = 0; i < order; i++)
		{
			stageOutput = (stageInput - ordered_Differences[i]) * fs;
			ordered_Differences[i] = stageInput;
			if (i < (order - 1))
				stageInput = stageOutput;
		}

		return stageOutput;
	}
};

class Norm_Denorm_Metering
{
public:

	// TYPE OF OPERATOR
	short *type_Norm_Denorm = new short(1);

	// ABSOLUTE BOUNDS OF TARGET VARIABLE
	double *targetVar_Bound_Abs_L = new double(0);
	double *targetVar_Bound_Abs_U = new double(1);

	// RELATIVE BOUNDS OF TARGET VARIABLE	(always b/w 0 and 1)
	double *targetVar_Bound_Rel_L = new double(0);
	double *targetVar_Bound_Rel_U = new double(1);

	// ABSOLUTE BOUNDS OF SUBSET RANGE
	double *subsetRange_Abs_L = new double(0);
	double *subsetRange_Abs_U = new double(1);

	// UI
	Slider* subsetRange;
	Label* meter_Full;
	Label* meter_Empty;
	Label* paramName;

	// REUSABLE UI VARIABLES
	UI_ControlGroup* UI_Grp;
	float *frac_StartX = new float(0), *frac_StartY = new float(0), *frac_totalWidth = new float(0), *frac_totalHeight = new float(0);
	int *px_StartX = new int(0), *px_StartY = new int(0), *px_totalWidth = new int(0), *px_totalHeight = new int(0);

	double *inputVal = new double(0);
	double *outputVal = new double(0);

	Norm_Denorm_Metering(String paramNavn, short type, double bound_Abs_L, double bound_Abs_U)
	{
		*type_Norm_Denorm = type;
		*targetVar_Bound_Abs_L = bound_Abs_L;
		*targetVar_Bound_Abs_U = bound_Abs_U;

		// SUBSET OF FULL PARAM RANGE
		subsetRange = new Slider(Slider::SliderStyle::TwoValueHorizontal, Slider::NoTextBox);
		subsetRange->setColour(subsetRange->trackColourId, Colours::yellow);
		subsetRange->setColour(subsetRange->backgroundColourId, Colours::blue);
		subsetRange->setRange(0, 1);
		subsetRange->setMinAndMaxValues(0, 1, sendNotificationSync);
		subsetRange->onValueChange = [this]
		{
			*targetVar_Bound_Rel_L = subsetRange->getMinValue();
			*targetVar_Bound_Rel_U = subsetRange->getMaxValue();
		};

		// METERS
		meter_Empty = new Label;
		meter_Empty->setColour(meter_Empty->backgroundColourId, Colours::blue);
		meter_Full = new Label;
		meter_Full->setColour(meter_Full->backgroundColourId, Colours::yellow);
		meter_Full->setColour(meter_Full->textColourId, Colours::black);

		// Name
		paramName = new Label(paramNavn);
	}

	~Norm_Denorm_Metering()
	{}

	void processInput(double in)
	{
		*inputVal = in;
		*subsetRange_Abs_L = *targetVar_Bound_Abs_L + (*targetVar_Bound_Abs_U - *targetVar_Bound_Abs_L) * *targetVar_Bound_Rel_L;
		*subsetRange_Abs_U = *targetVar_Bound_Abs_L + (*targetVar_Bound_Abs_U - *targetVar_Bound_Abs_L) * *targetVar_Bound_Rel_U;

		switch (*type_Norm_Denorm)
		{
		case 1:					// NORMALIZE
			*outputVal = (*inputVal - *subsetRange_Abs_L) / (*subsetRange_Abs_U - *subsetRange_Abs_L);
			if ((*inputVal < *subsetRange_Abs_L))		*outputVal = 0;
			if ((*inputVal > *subsetRange_Abs_U))		*outputVal = 1;
			break;
		case 2:					// DENORMALIZE
			*outputVal = *subsetRange_Abs_L + *inputVal * (*subsetRange_Abs_U - *subsetRange_Abs_L);
			break;
		}
	}

	void updateMeter()
	{
		float valueToDisplay = (*type_Norm_Denorm == 1) ? *inputVal : *outputVal;
		meter_Full->setText(String(valueToDisplay, 1), dontSendNotification);

		double filledFrac = fmax(0,(valueToDisplay - *targetVar_Bound_Abs_L) / (*targetVar_Bound_Abs_U - *targetVar_Bound_Abs_L));
		float emptyFrac = 1 - filledFrac;

		double meterWidth = 0.9 * *px_totalWidth;
		float meterHeight = 0.5 * *px_totalHeight;

		meter_Full->setBounds(*px_StartX + 0.05 * *px_totalWidth,
			*px_StartY + 0.1 * *px_totalHeight,
			meterWidth * filledFrac,
			meterHeight);

		meter_Empty->setBounds(*px_StartX + 0.05 * *px_totalWidth + meterWidth * filledFrac,
			*px_StartY + 0.1 * *px_totalHeight,
			meterWidth * emptyFrac,
			meterHeight);
	}

	// FRACTIONAL POSITION IN UI GROUP
	void setLayout(UI_ControlGroup* group, float startX, float startY, float width, float height)
	{
		UI_Grp = group;		*frac_StartX = startX;		*frac_StartY = startY;
		*frac_totalWidth = width;		*frac_totalHeight = height;

		// GET COORDINATES IN PIXELS
		UI_Grp->frac_to_Px(px_StartX, px_StartY, px_totalWidth, px_totalHeight, *frac_StartX, *frac_StartY, *frac_totalWidth, *frac_totalHeight);

		// PLACE ELEMENTS
		subsetRange->setBounds(*px_StartX + 0.05 * *px_totalWidth,
			*px_StartY + *px_totalHeight * 0.6,
			0.9 * *px_totalWidth,
			0.4 * *px_totalHeight);

		updateMeter();
	}

	void setVisible(bool on)
	{
		meter_Empty->setVisible(on);
		meter_Full->setVisible(on);
		subsetRange->setVisible(on);
	}
};

class MF_Multiplier : ComboBox::Listener
{
public:

	std::vector<MovementFeature>* featureVec;
	std::vector<Norm_Denorm_Metering>* normVec;
	ComboBox* multFactor;
	short* idx_SelectedFeature = new short(1);
	double* outputVal = new double(0);

	MF_Multiplier(std::vector<MovementFeature>* featureVec_Ptr, std::vector<Norm_Denorm_Metering>* normVector)
	{
		featureVec = featureVec_Ptr;
		normVec = normVector;
		multFactor = new ComboBox;
		multFactor->addItem("NONE", 1);
		short i = 2;
		for (MovementFeature& iter : *featureVec_Ptr)
		{
			multFactor->addItem(iter.mpName, i);
			i++;
		}
		multFactor->addListener(this);
		multFactor->setSelectedId(1);
		
	}

	~MF_Multiplier()
	{}

	void applyFeatureMult(double input)
	{
		short convertedIdx = max(0, *idx_SelectedFeature - 2);
		//MovementFeature* temp = &featureVec->at(convertedIdx);
		Norm_Denorm_Metering* temp = &normVec->at(convertedIdx);
		*outputVal =  jlimit(0.0, 1.0, (*idx_SelectedFeature == 1) ? input : input * *temp->outputVal);
	}

	void comboBoxChanged(juce::ComboBox* box)
	{
		if (box == multFactor)
		{
			*idx_SelectedFeature = box->getSelectedId();
		}
	}	
};

class Mapping_Quantization : ComboBox::Listener
{
public:

	// SMOOTHING
	BiQuad paramSmooth;
	Slider* set_paramSmooth_Fc;
	float* smoothing_Fc = new float(49);
	void set_paramSmooth_Cutoff(double fc)
	{
		*smoothing_Fc = fc;
		paramSmooth.calculateLPFCoeffs(fc, 0.7, 100);
	}

	// POLARITY
	ComboBox* set_Polarity;
	bool *isPolarity_Pos = new bool(true);
	double applyPolarity(double inputVal)
	{
		return *isPolarity_Pos ? inputVal : 1 - inputVal;
	}

	// MAPPING FUNCTION
	ComboBox* set_MapFunc;
	short *mapFunc = new short(1);
	double applyMapFunc(double inputVal)
	{
		double out = 0;
		switch (*mapFunc)
		{
		case 1:
			out = inputVal;
			break;
		case 2:
			out = pow(inputVal, 1.5);
			break;
		case 3:
			out = pow(inputVal, 2);
			break;
		case 4:
			out = pow(inputVal, 0.75);
			break;
		case 5:
			out = pow(inputVal, 0.45);
			break;
		case 6:
			out = exp((inputVal - 0.5) * 12) / (1 + exp((inputVal - 0.5) * 12));
			break;
		case 7:
			break;
		}
		return out;
	}

	// QUANTIZATION
	Slider* set_QuantLevels;
	short *quantLevels = new short(0);
	double applyQuantization(float currentParamValue, int quantLevels, float range, float min_Final)
	{
		float quantizedParam = 0;
		if (quantLevels < 1)
			return currentParamValue;
		if (quantLevels == 1) quantLevels++;

		else
		{
			currentParamValue -= min_Final;
			int numQuantizationSteps = quantLevels;
			float quantizationStepSize = range / (float)numQuantizationSteps;

			float diff = 1000;
			bool stepFound = false;
			for (int i = 0; i < numQuantizationSteps + 1; i++)
			{
				float currentStepForTest = i * quantizationStepSize;
				if (fabs(currentParamValue - currentStepForTest) <= diff && currentParamValue > currentStepForTest)
				{
					stepFound = true;
					diff = currentParamValue - currentStepForTest;
				}
			}
			if (stepFound) quantizedParam = currentParamValue - diff + min_Final;
			return quantizedParam;
		}
	};

	// PROCESSING CHAIN
	double *outputVal = new double(0);	
	void process(double inputVal)
	{
		if (*smoothing_Fc < 48.999) inputVal = paramSmooth.doBiQuad(inputVal, 0);
		inputVal = applyPolarity(inputVal);
		inputVal = applyMapFunc(inputVal);
		*outputVal = applyQuantization(inputVal, *quantLevels, 1, 0);
	}

	// HANDLE COMBO BOX CHANGE
	void comboBoxChanged(juce::ComboBox* box)
	{
		if (box == set_Polarity)
		{
			*isPolarity_Pos = box->getSelectedId() == 1 ? true : false;
		}

		if (box == set_MapFunc)
		{
			*mapFunc = box->getSelectedId();
		}
	}

	void setVisible(bool on)
	{
		set_paramSmooth_Fc->setVisible(on);
		set_Polarity->setVisible(on);
		set_MapFunc->setVisible(on);
		set_QuantLevels->setVisible(on);
	}

	void setLayout(UI_ControlGroup* ui_Grp, float frac_startX, float frac_startY, float frac_Width, float frac_Height)
	{
		ui_Grp->memberComponent_setBounds(
			set_paramSmooth_Fc,
			frac_startX + 0.05 * frac_Width,
			frac_startY,
			frac_Width * 0.9,
			frac_Height * 0.2
		);

		ui_Grp->memberComponent_setBounds(
			set_Polarity,
			frac_startX + 0.05 * frac_Width,
			frac_startY + 0.25 * frac_Height,
			frac_Width * 0.9,
			frac_Height * 0.2
		);

		ui_Grp->memberComponent_setBounds(
			set_MapFunc,
			frac_startX + 0.05 * frac_Width,
			frac_startY + 0.5 * frac_Height,
			frac_Width * 0.9,
			frac_Height * 0.2
		);

		ui_Grp->memberComponent_setBounds(
			set_QuantLevels,
			frac_startX + 0.05 * frac_Width,
			frac_startY + 0.75 * frac_Height,
			frac_Width * 0.9,
			frac_Height * 0.2
		);
	}

	Mapping_Quantization(float fc, bool isPol_Pos, short mapFunc, short quant_Levels)
	{
		set_paramSmooth_Fc = new Slider;
		set_paramSmooth_Fc->setRange(2, 49);
		set_paramSmooth_Fc->setValue(fc);
		set_paramSmooth_Fc->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		set_paramSmooth_Fc->setColour(set_paramSmooth_Fc->trackColourId, Colours::yellow);
		set_paramSmooth_Fc->setColour(set_paramSmooth_Fc->backgroundColourId, Colours::blue);
		set_paramSmooth_Fc->onValueChange = [this]
		{
			set_paramSmooth_Cutoff(set_paramSmooth_Fc->getValue());
		};

		set_Polarity = new ComboBox;
		set_Polarity->addListener(this);
		set_Polarity->addItem("+", 1);
		set_Polarity->addItem("-", 2);
		set_Polarity->setSelectedId(isPol_Pos ? 1 : 2);

		set_MapFunc = new ComboBox;
		set_MapFunc->addListener(this);
		set_MapFunc->addItem("Lin", 1);
		set_MapFunc->addItem("Ex1", 2);
		set_MapFunc->addItem("Ex2", 3);
		set_MapFunc->addItem("Lg1", 4);
		set_MapFunc->addItem("Lg2", 5);
		set_MapFunc->addItem("Sgm", 6);
		set_MapFunc->setSelectedId(mapFunc);

		set_QuantLevels = new Slider;
		set_QuantLevels->setRange(0, 16);
		set_QuantLevels->setSkewFactor(0.75);
		set_QuantLevels->setValue(quant_Levels);
		set_QuantLevels->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		set_QuantLevels->setColour(set_QuantLevels->trackColourId, Colours::yellow);
		set_QuantLevels->setColour(set_QuantLevels->backgroundColourId, Colours::blue);
		set_QuantLevels->onValueChange = [this]
		{
			*quantLevels = (short)set_QuantLevels->getValue();
		};
	}

	~Mapping_Quantization()
	{}
};

class MappingMatrix_Node
{
public:
	ToggleButton* onOff;
	bool *isOn = new bool(false);

	MappingMatrix_Node()
	{
		onOff = new ToggleButton;
		onOff->onClick = [this]
		{
			*isOn = onOff->getToggleState();
		};
	}

	~MappingMatrix_Node()
	{}
};

class TrackGain_Control
{
public:
	Slider* set_Gain;
	Label* track_Label;
	String name;
	float *val = new float(0);

	TrackGain_Control(String trackName, float dB_lowerBound, float dB_upperBound, float dB_default)
	{
		set_Gain = new Slider;
		set_Gain->setRange(-50,30);
		set_Gain->setTextBoxStyle(Slider::NoTextBox,true,0,0);
		set_Gain->setColour(set_Gain->trackColourId, Colours::yellow);
		set_Gain->setColour(set_Gain->backgroundColourId, Colours::blue);
		set_Gain->setRange(dB_lowerBound, dB_upperBound);
		set_Gain->setSkewFactor(2.5);
		setVal(jlimit(dB_lowerBound, dB_upperBound,dB_default));
		set_Gain->setValue(*val);

		track_Label = new Label("",trackName);
		track_Label->setJustificationType(Justification::centred);
	}

	~TrackGain_Control()
	{

	}

	void setVal(float val_dB)
	{
		*val = val_dB;
	}

	void onStart_initSlider()
	{
		set_Gain->setValue(*val);
	}

	void setVisible(bool on)
	{
		set_Gain->setVisible(on);
		track_Label->setVisible(on);
	}

	void setLayout(UI_ControlGroup* uiGrp, float frac_startX, float frac_startY, float wd, float ht)
	{
		uiGrp->memberComponent_setBounds(track_Label, 
			frac_startX + 0.05 * wd,
			frac_startY + 0.05 * ht,
			0.3 * wd, 
			0.9 * ht
		);

		uiGrp->memberComponent_setBounds(set_Gain,
			frac_startX + 0.4 * wd,
			frac_startY + 0.05 * ht,
			0.59 * wd,
			0.9 * ht
		);
	}
};

class EventThickener
{
public:

	int smplsToHold = 5;
	int smpls_HELD = 0;
	double outputVal = 0;

	double handleInputVal(double inputVal)
	{
		if (smpls_HELD == 0 && inputVal != 0)
		{
			outputVal = inputVal;
			smpls_HELD++;
		}

		if (inputVal == 0)
		{
			if (smpls_HELD == 0 || smpls_HELD == smplsToHold)
			{
				outputVal = inputVal;
				smpls_HELD = 0;
			}

			if (smpls_HELD > 0 && smpls_HELD < smplsToHold)
			{
				outputVal = outputVal;
				smpls_HELD++;
			}
		}

		return outputVal;
	}

	EventThickener(int smpls_Hold)
	{
		smplsToHold = smpls_Hold;
	}

	~EventThickener()
	{

	}
};

class MappingSlider
{
public:
	Label* nameLabel;
	Slider* sliderControl;
	float* minBound;
	float* maxBound;
	float* mapVal = 0;

	MappingSlider(String name, float minVal, float maxVal)
	{
		nameLabel = new Label(name);
		nameLabel->setText(name, dontSendNotification);
		nameLabel->setJustificationType(Justification::centred);

		mapVal = new float(0);
		minBound = new float(minVal);
		maxBound = new float(maxVal);
		sliderControl = new Slider;
		sliderControl->setSliderStyle(Slider::SliderStyle::LinearVertical);
		sliderControl->setRange(0, 1);
		sliderControl->setValue(0);
		sliderControl->setColour(sliderControl->trackColourId, Colours::yellow);
		sliderControl->setColour(sliderControl->backgroundColourId, Colours::blue);
		sliderControl->setTextBoxStyle(Slider::NoTextBox,true,0,0);
		sliderControl->onValueChange = [this]
		{
			*mapVal = *minBound + sliderControl->getValue() * (*maxBound - *minBound);
		};
	}

	void setBounds(UI_ControlGroup* uiGrp, float frac_startX, float frac_startY, float frac_Wd, float frac_Ht)
	{
		uiGrp->memberComponent_setBounds(sliderControl, frac_startX, frac_startY, frac_Wd, frac_Ht * 0.8);
		uiGrp->memberComponent_setBounds(nameLabel, frac_startX, frac_startY + frac_Ht * 0.85, frac_Wd, frac_Ht * 0.1);
	}

	void setVisible(bool on)
	{
		nameLabel->setVisible(on);
		sliderControl->setVisible(on);
	}
};

class Oscillators
{
public:
	Oscillators()
	{
		LPF_Randomness.flushDelays();
		LPF_Randomness.calculateLPFCoeffs(50, 0.7, 100);

		set_Osc_Freq = new Slider;
		set_Osc_Freq->setRange(0.5, 4);
		set_Osc_Freq->setValue(1);
		set_Osc_Freq->setColour(set_Osc_Freq->trackColourId, Colours::yellow);
		set_Osc_Freq->setColour(set_Osc_Freq->backgroundColourId, Colours::blue);
		set_Osc_Freq->setNumDecimalPlacesToDisplay(2);
		set_Osc_Freq->onValueChange = [this]
		{
			*Osc_Freq = set_Osc_Freq->getValue();
		};
		Lab_Osc_Freq = new Label("", "Frequency");
		Lab_Osc_Freq->attachToComponent(set_Osc_Freq, true);

		randGen = new Random;
	}

	~Oscillators()
	{

	}

	// Triangle Oscillator
	Slider* set_Osc_Freq;
	Label* Lab_Osc_Freq;
	double* Osc_Freq = new double(1.0);
	unsigned long ticksElapsed = 0;
	double triOsc_Val = 0;
	double impTrain_Val = 0;
	double randomness_Val = 0;

	void triOsc_Update()
	{
		double triOsc_Period = 1 / *Osc_Freq;
		long t = ticksElapsed;
		int D = (int)(triOsc_Period / 0.01);
		double funcVal = abs((t + D - 1) % ((D - 1) * 2) - (D - 1)) / (float)D;
		triOsc_Val = funcVal;

		ticksElapsed++;
	}

	// Randomness Generator
	Random *randGen;
	BiQuad LPF_Randomness;

	// IMPULSE TRAIN
	double impulseTrain_TimeElapsed = 0;
	void impulseTrain_Update()
	{
		double output = 0;
		double period = 1 / *Osc_Freq;

		if (impulseTrain_TimeElapsed >= period)
		{
			output = 1;
			impulseTrain_TimeElapsed = 0;
		}

		impulseTrain_TimeElapsed += 0.01;
		impTrain_Val = output;
	}

	void getNewRandomValue()
	{
		if (ticksElapsed % 10000 == 0)
			randGen = new Random;

		int randomIndex = randGen->nextInt(1000);
		randomness_Val = randomIndex / 1000.0;
		// setValue_MovFeature("Randomness", LPF_Randomness.doBiQuad(randomIndex / 1000.0, 0));
	}

	void setVisible(bool on)
	{
		set_Osc_Freq->setVisible(on);
	}
};

class EnergyCompute
{
public:
	EnergyCompute()
	{
		smoothe_CoM_Speed.flushDelays();
		smoothe_CoM_Speed.calculateLPFCoeffs(5, 0.7, 100);
	}

	~EnergyCompute()
	{

	}

	BiQuad smoothe_CoM_Speed;
	float CoM_Coord_H = 0;
	float CoM_Coord_V = 0;
	float CoM_Speed = 0;

	// Delay Variables
	float CoM_H_Disp_z1 = 0;
	float CoM_V_Disp_z1 = 0;

	void calcCoMCoordinates(float pitch_Trunk, float pitch_Thigh, float pitch_Shank)
	{
		float seg_AP_Ang_Vals[3] = {
			pitch_Trunk,
			pitch_Thigh,
			pitch_Shank };

		float seg_wtFracs[3] = { 0.59, 0.29, 0.09 };
		float seg_LnFracs[3] = { 0.402, 0.241, 0.25 };
		float seg_CoM_Dist_Joint[3] = { 0.57, 0.59, 0.55 };

		float seg_CoM_X[3] = { 0.0, 0.0, 0.0 };
		float seg_CoM_Y[3] = { 0.0, 0.0, 0.0 };

		float body_CoM_X = 0;
		float body_CoM_Y = 0;

		// Helper Variables	-- START WITH ANKLE
		float lastJointX = 0;
		float lastJointY = 0;

		// Calculate CoM Locations of each body segment
		for (int i = 0; i < 3; i++)
		{
			int j = 2 - i;
			// CoM of Segment: Last Joint Position + sin(seg Angle) x seg length x seg CoM percentage of length
			seg_CoM_X[j] = lastJointX + sin(M_PI / 180.0 * seg_AP_Ang_Vals[j]) * seg_LnFracs[j] * seg_CoM_Dist_Joint[j];
			seg_CoM_Y[j] = lastJointY + cos(M_PI / 180.0 * seg_AP_Ang_Vals[j]) * seg_LnFracs[j] * seg_CoM_Dist_Joint[j];

			// Find relative position of next joint in space
			lastJointX += sin(M_PI / 180.0 * seg_AP_Ang_Vals[j]) * seg_LnFracs[j];
			lastJointY += cos(M_PI / 180.0 * seg_AP_Ang_Vals[j]) * seg_LnFracs[j];
		}

		float numSum_X = 0;
		float numSum_Y = 0;
		float denSum = seg_wtFracs[0] + seg_wtFracs[1] + seg_wtFracs[2];

		for (int i = 0; i < 3; i++)
		{
			numSum_X += seg_wtFracs[0] * seg_CoM_X[i];
			numSum_Y += seg_wtFracs[0] * seg_CoM_Y[i];
		}

		CoM_Coord_H = numSum_X / denSum;
		CoM_Coord_V = numSum_Y / denSum;
		
		// CALCULATE CoM Speed
		float CoM_H_Spd = fabs(CoM_Coord_H - CoM_H_Disp_z1) * 100;
		float CoM_V_Spd = fabs(CoM_Coord_V - CoM_V_Disp_z1) * 100;

		CoM_Speed = sqrt(CoM_H_Spd * CoM_H_Spd + CoM_V_Spd * CoM_V_Spd);
		CoM_Speed = smoothe_CoM_Speed.doBiQuad(CoM_Speed, 0);
		CoM_Speed = jlimit(0.0, 1.0, (double)CoM_Speed);

		CoM_H_Disp_z1 = CoM_Coord_H;
		CoM_V_Disp_z1 = CoM_Coord_V;
	}
};

class CoM_Info_Display
{
public:
	UI_ControlGroup* ui_Grp;
	Label* CoM_H;
	Label* CoM_V;
	Label* CoM_Speed;

	CoM_Info_Display(UI_ControlGroup* uiGroup)
	{
		ui_Grp = uiGroup;
		CoM_H = new Label("", "CoM_Horz_Loc = ");
		CoM_V = new Label("", "CoM_Vert_Loc = ");
		CoM_Speed = new Label("", "CoM_Speed = ");

		CoM_H->setJustificationType(Justification::centred);
		CoM_V->setJustificationType(Justification::centred);
		CoM_Speed->setJustificationType(Justification::centred);
	}

	~CoM_Info_Display()
	{

	}

	void update(float h, float v, float speed)
	{
		CoM_H->setText("CoM_Horz_Loc = " + String(h, 3), dontSendNotification);
		CoM_V->setText("CoM_Vert_Loc = " + String(v, 3), dontSendNotification);
		CoM_Speed->setText("CoM_Speed = " + String(speed, 3), dontSendNotification);
	}
};
