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

class GaitEventDetection
{
public:
	
	// Smooth Gyr Signal
	BiQuad gyrSmooth_L;
	BiQuad gyrSmooth_R;

	float Thresh_HS_GyrZ_L = 64;
	float Thresh_HS_GyrZ_R = 64;
	int Thresh_HS_Smpls_L = 5;
	int Thresh_HS_Smpls_R = 5;
	float Thresh_TO_GyrZ_L = -73;
	float Thresh_TO_GyrZ_R = -73;
	float Thresh_MS_Crossing = 150;

	float gyrZ_L_Z1 = 0, gyrZ_L_Z2 = 0, gyrZ_R_Z1 = 0, gyrZ_R_Z2 = 0;
	int smpls_sinceLast_HS_L = 0, smpls_sinceLast_HS_R = 0;

	// STATES
	bool isSwing_L = false;
	bool isSwing_R = false;
	bool isDblSupport = false;
	bool isStationary = false;
	bool isTurning = false;

	// EVENTS
	bool HS_L = false;
	bool HS_R = false;
	bool TO_L = false;
	bool TO_R = false;
	bool MS_L = false;
	bool MS_R = false;
	bool MS_L_CROSSED = false;
	bool MS_R_CROSSED = false;

	short lastFootContact = 1;

	GaitEventDetection()
	{
		LAB_TO_MS_Thresh = new Label("","TO MS Thresh");
		LAB_TO_HS_Smpls = new Label("", "TO HS Thresh");
		LAB_MS_Thresh = new Label("", "Midswing Thresh");
		Set_Thresh_HS_Smpls_L = new Slider;
		Set_Thresh_HS_Smpls_R = new Slider;
		Set_Thresh_MS_Crossing = new Slider;
		Set_Thresh_TO_HS_L = new MovementRangeSlider(-250, 250);
		Set_Thresh_TO_HS_R = new MovementRangeSlider(-250, 250);

		Set_Thresh_HS_Smpls_L->setRange(0, 50);
		Set_Thresh_HS_Smpls_L->setValue(Thresh_HS_Smpls_L);
		Set_Thresh_HS_Smpls_L->setNumDecimalPlacesToDisplay(0);
		Set_Thresh_HS_Smpls_L->setColour(Set_Thresh_HS_Smpls_L->trackColourId, Colours::yellow);
		Set_Thresh_HS_Smpls_L->setColour(Set_Thresh_HS_Smpls_L->backgroundColourId, Colours::blue);
		Set_Thresh_HS_Smpls_L->onValueChange = [this]
		{
			Thresh_HS_Smpls_L = Set_Thresh_HS_Smpls_L->getValue();
		};

		Set_Thresh_HS_Smpls_R->setRange(0, 50);
		Set_Thresh_HS_Smpls_R->setValue(Thresh_HS_Smpls_R);
		Set_Thresh_HS_Smpls_R->setNumDecimalPlacesToDisplay(0);
		Set_Thresh_HS_Smpls_R->setColour(Set_Thresh_HS_Smpls_R->trackColourId, Colours::yellow);
		Set_Thresh_HS_Smpls_R->setColour(Set_Thresh_HS_Smpls_R->backgroundColourId, Colours::blue);
		Set_Thresh_HS_Smpls_R->onValueChange = [this]
		{
			Thresh_HS_Smpls_R = Set_Thresh_HS_Smpls_R->getValue();
		};

		Set_Thresh_MS_Crossing->setRange(0, 200);
		Set_Thresh_MS_Crossing->setValue(Thresh_MS_Crossing);
		Set_Thresh_MS_Crossing->setNumDecimalPlacesToDisplay(0);
		Set_Thresh_MS_Crossing->setColour(Set_Thresh_MS_Crossing->trackColourId, Colours::yellow);
		Set_Thresh_MS_Crossing->setColour(Set_Thresh_MS_Crossing->backgroundColourId, Colours::blue);
		Set_Thresh_MS_Crossing->onValueChange = [this]
		{
			Thresh_MS_Crossing = Set_Thresh_MS_Crossing->getValue();
		};

		Set_Thresh_TO_HS_L->range->setMinAndMaxValues(Thresh_TO_GyrZ_L, Thresh_HS_GyrZ_L);
		Set_Thresh_TO_HS_R->range->setMinAndMaxValues(Thresh_TO_GyrZ_R, Thresh_HS_GyrZ_R);

		gyrSmooth_L.calculateLPFCoeffs(5, 0.7, 100);
		gyrSmooth_R.calculateLPFCoeffs(5, 0.7, 100);
	};

	~GaitEventDetection()
	{};

	void calcFeatures_GaitEvents(float gyrZ_Shank_L, float gyrZ_Shank_R, float pitch_Shank_L, float pitch_Shank_R, bool swng_L, bool swng_R, float gyrY_Trunk)
	{
		// LOGIC FOR EVENT DETECTION
		HS_L = false;
		HS_R = false;
		TO_L = false;
		TO_R = false;
		MS_L = false;
		MS_R = false;

		isSwing_L = swng_L;
		isSwing_R = swng_R;

		Thresh_TO_GyrZ_L = Set_Thresh_TO_HS_L->VAL_bounds[0];
		Thresh_HS_GyrZ_L = Set_Thresh_TO_HS_L->VAL_bounds[1];
		Thresh_TO_GyrZ_R = Set_Thresh_TO_HS_R->VAL_bounds[0];
		Thresh_HS_GyrZ_R = Set_Thresh_TO_HS_R->VAL_bounds[1];

		// DETECT HEEL STRIKE
		if (isSwing_L && MS_L_CROSSED && (gyrZ_L_Z1 < gyrZ_L_Z2) && (gyrZ_L_Z1 < gyrZ_Shank_L) && (gyrZ_L_Z1 < Thresh_HS_GyrZ_L))
		{
			isSwing_L = false;
			HS_L = true;
			smpls_sinceLast_HS_L = 0;
			MS_L_CROSSED = false;
			lastFootContact = 1;
		}

		if (isSwing_R && MS_R_CROSSED && (gyrZ_R_Z1 < gyrZ_R_Z2) && (gyrZ_R_Z1 < gyrZ_Shank_R) && (gyrZ_R_Z1 < Thresh_HS_GyrZ_R))
		{
			isSwing_R = false;
			HS_R = true;
			smpls_sinceLast_HS_R = 0;
			MS_R_CROSSED = false;
			lastFootContact = 2;
		}

		// DETECT TOE OFF
		if (!isSwing_L && (gyrZ_Shank_L > gyrZ_L_Z1) && (gyrZ_L_Z1 < gyrZ_L_Z2) 
			&& (gyrZ_L_Z1 < Thresh_TO_GyrZ_L) && (smpls_sinceLast_HS_L >= Thresh_HS_Smpls_L))
		{
			isSwing_L = true;
			TO_L = true;
		}

		if (!isSwing_R && (gyrZ_Shank_R > gyrZ_R_Z1) && (gyrZ_R_Z1 < gyrZ_R_Z2)
			&& (gyrZ_R_Z1 < Thresh_TO_GyrZ_R) && (smpls_sinceLast_HS_R >= Thresh_HS_Smpls_R))
		{
			isSwing_R = true;
			TO_R = true;
		}

		// CHECK IF MS CROSSED
		if ((gyrZ_Shank_L > Thresh_MS_Crossing) && (gyrZ_L_Z1 <= Thresh_MS_Crossing))
		{
			MS_L_CROSSED = true;
			MS_L = true;
		}

		if ((gyrZ_Shank_R > Thresh_MS_Crossing) && (gyrZ_R_Z1 <= Thresh_MS_Crossing))
		{
			MS_R_CROSSED = true;
			MS_R = true;
		}

		// CHECK TURNING
		// isTurning = (gyrY_Trunk > 100) ? true : false;

		// CHECK STATIONARITY
		/*bool angleCondition = (abs(pitch_Shank_L) < 10) && (abs(pitch_Shank_R) < 10);
		bool velCondition = (abs(gyrZ_Shank_L) < 20) && (abs(gyrZ_Shank_R) < 20);
		isStationary = angleCondition && velCondition;
		if (isStationary)
		{
			isSwing_L = false;
			isSwing_R = false;
		}*/

		// UPDATE HS TIMERS
		if (!isSwing_L) smpls_sinceLast_HS_L++;
		if (!isSwing_R) smpls_sinceLast_HS_R++;

		// SHUFFLE DELAYS
		gyrZ_L_Z2 = gyrZ_L_Z1;
		gyrZ_L_Z1 = gyrZ_Shank_L;
		gyrZ_R_Z2 = gyrZ_R_Z1;
		gyrZ_R_Z1 = gyrZ_Shank_R;
	}

	// UI ELEMENTS
	Label* LAB_TO_MS_Thresh;
	Label* LAB_TO_HS_Smpls;
	Slider* Set_Thresh_HS_Smpls_L;
	Slider* Set_Thresh_HS_Smpls_R;
	MovementRangeSlider* Set_Thresh_TO_HS_L;
	MovementRangeSlider* Set_Thresh_TO_HS_R;
	Label* LAB_MS_Thresh;
	Slider* Set_Thresh_MS_Crossing;

	void setVisible(bool on)
	{
		LAB_TO_MS_Thresh->setVisible(on);
		LAB_TO_HS_Smpls->setVisible(on);
		Set_Thresh_HS_Smpls_L->setVisible(on);
		Set_Thresh_HS_Smpls_R->setVisible(on);
		Set_Thresh_TO_HS_L->setVisible(on);
		Set_Thresh_TO_HS_R->setVisible(on);
		LAB_MS_Thresh->setVisible(on);
		Set_Thresh_MS_Crossing->setVisible(on);
	}	
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
	double progress_Val = 0;

	short numCol = 6;
	long numSmpl_TOT = 100000000;
	long numSmpl_NOW = 0;
	long smpl_Strt = 0;
	long smpl_End = 100000000;

	enum modes { SENSOR_NO_FILE_LOAD = 1, SENSOR_FILE_LOAD = 2, FILE_LOADED = 3 };

	short mode = 1;

	String rec_LogPath = "";

	std::vector<String> TR;
	std::vector<String> TH_L;
	std::vector<String> TH_R;
	std::vector<String> SH_L;
	std::vector<String> SH_R;
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
		TH_L.clear();
		TH_R.clear();
		SH_L.clear();
		SH_R.clear();
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
		TH_L.clear();
		TH_R.clear();
		SH_L.clear();
		SH_R.clear();
		numSmpl_NOW = 0;

		for (int i = 0; i < 10; i++) logFound[i] = false;
		short fileIdx = 0;

		// ITERATE ACROSS ALL FILES
		for (File& iter : imuLogFiles)
		{
			FileInputStream imuLogStream(iter);

			if (iter.getFullPathName().contains("Trunk"))
				fileIdx = 1;
			if (iter.getFullPathName().contains("Thigh L"))
				fileIdx = 2;
			if (iter.getFullPathName().contains("Thigh R"))
				fileIdx = 3;
			if (iter.getFullPathName().contains("Shank L"))
				fileIdx = 4;
			if (iter.getFullPathName().contains("Shank R"))
				fileIdx = 5;

			if (imuLogStream.openedOk())
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
						TH_L.push_back(newLine);
						logSizes[1]++;
						break;
					case 3:
						TH_R.push_back(newLine);
						logSizes[2]++;
						break;
					case 4:
						SH_L.push_back(newLine);
						logSizes[3]++;
						break;
					case 5:
						SH_R.push_back(newLine);
						logSizes[4]++;
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
			if (location == "Thigh L" && logFound[1]) out = TH_L[numSmpl_NOW];
			if (location == "Thigh R" && logFound[2]) out = TH_R[numSmpl_NOW];
			if (location == "Shank L" && logFound[3]) out = SH_L[numSmpl_NOW];
			if (location == "Shank R" && logFound[4]) out = SH_R[numSmpl_NOW];
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

class Visualizer_Block
{
public:
	Visualizer_Block(int ln, int br, int th, Colour* cl, int index)
	{
		length = ln;
		breadth = br;
		thickness = th;
		col = cl;

		sagit = new Label;
		front = new Label;

		sagit->setColour(sagit->backgroundColourId, *col);
		sagit->setColour(sagit->outlineColourId, Colours::red);
		front->setColour(front->backgroundColourId, *col);
		front->setColour(front->outlineColourId, Colours::red);

		idx = index;
	}

	~Visualizer_Block()
	{}

	Label* sagit;
	Label* front;

	int length = 0;
	int breadth = 0;
	int thickness = 0;
	Colour* col;

	float sagit_X = 0, sagit_Y = 0, front_X = 0, front_Y = 0;
	int idx;

	void updateXY(int plane, float x, float y)
	{
		switch (plane)
		{
		case 1:
			sagit_X = x;
			sagit_Y = y;
			break;
		case 2:
			front_X = x;
			front_Y = y;
			break;
		}
	}

	void placeLabel_inUIGroup(UI_ControlGroup* uiGrp, int plane, float x_Rel, float y_Rel, float wd_Rel, float ht_Rel)
	{
		Label* toPlace;
		
		switch (plane)
		{
		case 1: 
			toPlace = sagit;
			break;
		case 2:
			toPlace = front;
			break;
		}

		uiGrp->memberComponent_setBounds(toPlace, x_Rel, y_Rel, wd_Rel, ht_Rel);
	}

	void updateAlpha(bool isOn)
	{
		sagit->setAlpha(isOn ? 1 : 0.3);
		front->setAlpha(isOn ? 1 : 0.3);
	}

	void setVisible(bool on)
	{
		sagit->setVisible(on);
		front->setVisible(on);
	}
};

class Visualizer_Joint
{
public:
	Visualizer_Joint(String navn, Colour col, int blk_L, int blk_B, int blk_T)
	{
		name = navn;
		block = new Visualizer_Block(blk_L, blk_B, blk_T, &col, 0);
	}

	~Visualizer_Joint()
	{

	}

	String name = "";
	Visualizer_Block* block;
	bool is_AboveHip = false;

	void setVisible(bool on)
	{
		block->setVisible(on);
	}
};

class Visualizer_BodySegment
{
public:
	Visualizer_BodySegment(String navn, int numBlks, Colour col, int blk_L, 
		int blk_B, int blk_T, Visualizer_Joint* j_A, Visualizer_Joint* j_B, bool is_AboveHip)
	{
		name = navn;
		numBlocks = numBlks;
		Visualizer_Block* blockPtr;
		
		for (int i = 0; i < numBlks; i++)
		{
			blockPtr = new Visualizer_Block(blk_L, blk_B, blk_T, &col, i);
			blockVector.push_back(*blockPtr);
		}

		joint_Above = j_A;
		joint_Below = j_B;

		isAbove_HIP = is_AboveHip;
	};

	~Visualizer_BodySegment()
	{
		blockVector.clear();
	};

	String name = "";
	std::vector<Visualizer_Block> blockVector;
	int numBlocks = 0;
	bool isOnline = false;
	bool isAbove_HIP = false;
	float pitch = 0;
	float roll = 0;

	Visualizer_Joint* joint_Above;
	Visualizer_Joint* joint_Below;

	void updateAngles(float pt, float rl)
	{
		pitch = pt;
		roll = rl;
	}

	void updatePosition()
	{

	}

	void updateAlpha()
	{
		for (Visualizer_Block& iter : blockVector)
		{
			iter.updateAlpha(isOnline);
		}

		if (isAbove_HIP) joint_Above->block->updateAlpha(isOnline);
		else joint_Below->block->updateAlpha(isOnline);
	}

	void setVisible(bool on)
	{
		for (Visualizer_Block& iter : blockVector)
			iter.setVisible(on);
	}
};

class Visualizer
{
public:
	Visualizer()
	{
		// INITIALIZE ALL JOINTS
		head = new Visualizer_Joint("Head", Colours::yellow, 50, 50, 50);
		hip = new Visualizer_Joint("Hip", Colours::yellow, 50, 50, 200);
		knee_L = new Visualizer_Joint("Knee L", Colours::yellow, 40, 40, 40);
		knee_R = new Visualizer_Joint("Knee R", Colours::yellow, 40, 40, 40);
		ankle_L = new Visualizer_Joint("Ankle L", Colours::yellow, 70, 40, 40);
		ankle_R = new Visualizer_Joint("Ankle R", Colours::yellow, 70, 40, 40);

		// INITIALIZE ALL SEGMENTS
		trunk = new Visualizer_BodySegment("Trunk", 10, Colours::blue, 9, 2, 9, head, hip, true);
		thigh_L = new Visualizer_BodySegment("Thigh L", 10, Colours::blue, 5, 2, 5, hip, knee_L, false);
		thigh_R = new Visualizer_BodySegment("Thigh R", 10, Colours::blue, 5, 2, 5, hip, knee_R, false);
		shank_L = new Visualizer_BodySegment("Shank L", 10, Colours::blue, 5, 2, 5, knee_L, ankle_L, false);
		shank_R = new Visualizer_BodySegment("Shank R", 10, Colours::blue, 5, 2, 5, knee_R, ankle_R, false);

		// TEXT COLOUR OF JOINTS
		ankle_L->block->front->setColour(ankle_L->block->front->textColourId, Colours::black);
		ankle_R->block->front->setColour(ankle_R->block->front->textColourId, Colours::black);
		knee_L->block->front->setColour(knee_L->block->front->textColourId, Colours::black);
		knee_R->block->front->setColour(knee_R->block->front->textColourId, Colours::black);
	};

	~Visualizer()
	{};

	// JOINTS
	Visualizer_Joint* head;
	Visualizer_Joint* hip;
	Visualizer_Joint* knee_L;
	Visualizer_Joint* knee_R;
	Visualizer_Joint* ankle_L;
	Visualizer_Joint* ankle_R;

	// SEGMENTS
	Visualizer_BodySegment* trunk;
	Visualizer_BodySegment* thigh_L;
	Visualizer_BodySegment* thigh_R;
	Visualizer_BodySegment* shank_L;
	Visualizer_BodySegment* shank_R;

	float rel_Wd_Ht_SAG_XY_Head[2] = { 0,0 };
	float rel_Wd_Ht_SAG_XY_Hip[2] = { 0,0 };
	float rel_Wd_Ht_SAG_XY_KneeL[2] = { 0,0 };
	float rel_Wd_Ht_SAG_XY_KneeR[2] = { 0,0 };
	float rel_Wd_Ht_SAG_XY_AnkleL[2] = { 0,0 };
	float rel_Wd_Ht_SAG_XY_AnkleR[2] = { 0,0 };
	float rel_Wd_Ht_FRN_XY_Head[2] = { 0,0 };
	float rel_Wd_Ht_FRN_XY_Hip[2] = { 0,0 };
	float rel_Wd_Ht_FRN_XY_KneeL[2] = { 0,0 };
	float rel_Wd_Ht_FRN_XY_KneeR[2] = { 0,0 };
	float rel_Wd_Ht_FRN_XY_AnkleL[2] = { 0,0 };
	float rel_Wd_Ht_FRN_XY_AnkleR[2] = { 0,0 };

	float center_SAG_XY_Head[2] = { 0, 0 };
	float center_SAG_XY_Hip[2] = { 0.5, 0.5 };
	float center_SAG_XY_KneeL[2] = { 0, 0 };
	float center_SAG_XY_KneeR[2] = { 0, 0 };
	float center_SAG_XY_AnkleL[2] = { 0, 0 };
	float center_SAG_XY_AnkleR[2] = { 0, 0 };
	float center_FRN_XY_Head[2] = { 0, 0 };
	float center_FRN_XY_Hip[2] = { 0.5, 0.5 };
	float center_FRN_XY_KneeL[2] = { 0, 0 };
	float center_FRN_XY_KneeR[2] = { 0, 0 };
	float center_FRN_XY_AnkleL[2] = { 0, 0 };
	float center_FRN_XY_AnkleR[2] = { 0, 0 };

	float topLeft_SAG_XY_Head[2] = { 0, 0 };
	float topLeft_SAG_XY_Hip[2] = { 0, 0 };
	float topLeft_SAG_XY_KneeL[2] = { 0, 0 };
	float topLeft_SAG_XY_KneeR[2] = { 0, 0 };
	float topLeft_SAG_XY_AnkleL[2] = { 0, 0 };
	float topLeft_SAG_XY_AnkleR[2] = { 0, 0 };
	float topLeft_FRN_XY_Head[2] = { 0, 0 };
	float topLeft_FRN_XY_Hip[2] = { 0, 0 };
	float topLeft_FRN_XY_KneeL[2] = { 0, 0 };
	float topLeft_FRN_XY_KneeR[2] = { 0, 0 };
	float topLeft_FRN_XY_AnkleL[2] = { 0, 0 };
	float topLeft_FRN_XY_AnkleR[2] = { 0, 0 };

	float topCenter_SAG_XY_Head[2] = { 0, 0 };
	float topCenter_SAG_XY_Hip[2] = { 0, 0 };
	float topCenter_SAG_XY_KneeL[2] = { 0, 0 };
	float topCenter_SAG_XY_KneeR[2] = { 0, 0 };
	float topCenter_SAG_XY_AnkleL[2] = { 0, 0 };
	float topCenter_SAG_XY_AnkleR[2] = { 0, 0 };
	float topCenter_FRN_XY_Head[2] = { 0, 0 };
	float topCenter_FRN_XY_Hip[2] = { 0, 0 };
	float topCenter_FRN_XY_KneeL[2] = { 0, 0 };
	float topCenter_FRN_XY_KneeR[2] = { 0, 0 };
	float topCenter_FRN_XY_AnkleL[2] = { 0, 0 };
	float topCenter_FRN_XY_AnkleR[2] = { 0, 0 };

	float bottomCenter_SAG_XY_Head[2] = { 0, 0 };
	float bottomCenter_SAG_XY_Hip[2] = { 0, 0 };
	float bottomCenter_SAG_XY_KneeL[2] = { 0, 0 };
	float bottomCenter_SAG_XY_KneeR[2] = { 0, 0 };
	float bottomCenter_SAG_XY_AnkleL[2] = { 0, 0 };
	float bottomCenter_SAG_XY_AnkleR[2] = { 0, 0 };
	float bottomCenter_FRN_XY_Head[2] = { 0, 0 };
	float bottomCenter_FRN_XY_Hip[2] = { 0, 0 };
	float bottomCenter_FRN_XY_KneeL[2] = { 0, 0 };
	float bottomCenter_FRN_XY_KneeR[2] = { 0, 0 };
	float bottomCenter_FRN_XY_AnkleL[2] = { 0, 0 };
	float bottomCenter_FRN_XY_AnkleR[2] = { 0, 0 };

	float htFrac_Trunk = 0.402 * 0.95;
	float htFrac_Thigh = 0.241 * 0.95;
	float htFrac_Shank = 0.23 * 0.95;

	float legOffset = 0.2;

	void update(UI_ControlGroup* uiGrp_SAG, UI_ControlGroup* uiGrp_FRN, bool isStance_L, 
		bool isStance_R, float ang_Knee_L, float ang_Knee_R, bool isStationary, bool isTurning)
	{
		// CHANGE ALPHA BASED ON ONLINE STATUS
		trunk->updateAlpha();
		thigh_L->updateAlpha();
		thigh_R->updateAlpha();
		shank_L->updateAlpha();
		shank_R->updateAlpha();

		// CUSTOM LOGIC TO UPDATE POSITIONS OF ALL JOINTS AND SEGMENTS (!)

		// PLACE BODY SEGMENTS

		// POPULATE RELATIVE X and Y COORDINATES

		// TRUNK
		for (Visualizer_Block& iter : trunk->blockVector)
		{
			getIncrementalXY_fromEndPoints(center_SAG_XY_Hip[0], center_SAG_XY_Head[0], center_SAG_XY_Hip[1], center_SAG_XY_Head[1],
				iter.idx, trunk->numBlocks, &iter.sagit_X, &iter.sagit_Y);

			iter.placeLabel_inUIGroup(uiGrp_SAG, 1, iter.sagit_X, iter.sagit_Y, 0.01, 0.02);

			getIncrementalXY_fromEndPoints(center_FRN_XY_Hip[0], center_FRN_XY_Head[0], center_FRN_XY_Hip[1], center_FRN_XY_Head[1],
				iter.idx, trunk->numBlocks, &iter.front_X, &iter.front_Y);

			iter.placeLabel_inUIGroup(uiGrp_FRN, 2, iter.front_X, iter.front_Y, 0.01, 0.02);
		}

		// THIGH L
		for (Visualizer_Block& iter : thigh_L->blockVector)
		{
			getIncrementalXY_fromEndPoints(center_SAG_XY_KneeL[0], center_SAG_XY_Hip[0], center_SAG_XY_KneeL[1], center_SAG_XY_Hip[1],
				iter.idx, thigh_L->numBlocks, &iter.sagit_X, &iter.sagit_Y);

			iter.placeLabel_inUIGroup(uiGrp_SAG, 1, iter.sagit_X, iter.sagit_Y, 0.01, 0.02);

			getIncrementalXY_fromEndPoints(center_FRN_XY_KneeL[0], center_FRN_XY_Hip[0] - legOffset , center_FRN_XY_KneeL[1], center_FRN_XY_Hip[1],
				iter.idx, thigh_L->numBlocks, &iter.front_X, &iter.front_Y);

			iter.placeLabel_inUIGroup(uiGrp_FRN, 2, iter.front_X, iter.front_Y, 0.01, 0.02);
		}

		// THIGH R
		for (Visualizer_Block& iter : thigh_R->blockVector)
		{
			getIncrementalXY_fromEndPoints(center_SAG_XY_KneeR[0], center_SAG_XY_Hip[0], center_SAG_XY_KneeR[1], center_SAG_XY_Hip[1],
				iter.idx, thigh_R->numBlocks, &iter.sagit_X, &iter.sagit_Y);

			iter.placeLabel_inUIGroup(uiGrp_SAG, 1, iter.sagit_X, iter.sagit_Y, 0.01, 0.02);

			getIncrementalXY_fromEndPoints(center_FRN_XY_KneeR[0], center_FRN_XY_Hip[0] + legOffset, center_FRN_XY_KneeR[1], center_FRN_XY_Hip[1],
				iter.idx, thigh_R->numBlocks, &iter.front_X, &iter.front_Y);

			iter.placeLabel_inUIGroup(uiGrp_FRN, 2, iter.front_X, iter.front_Y, 0.01, 0.02);
		}

		// SHANK L
		for (Visualizer_Block& iter : shank_L->blockVector)
		{
			getIncrementalXY_fromEndPoints(center_SAG_XY_AnkleL[0], center_SAG_XY_KneeL[0], center_SAG_XY_AnkleL[1], center_SAG_XY_KneeL[1],
				iter.idx, shank_L->numBlocks, &iter.sagit_X, &iter.sagit_Y);

			iter.placeLabel_inUIGroup(uiGrp_SAG, 1, iter.sagit_X, iter.sagit_Y, 0.01, 0.02);

			getIncrementalXY_fromEndPoints(center_FRN_XY_AnkleL[0], center_FRN_XY_KneeL[0], center_FRN_XY_AnkleL[1], center_FRN_XY_KneeL[1],
				iter.idx, shank_L->numBlocks, &iter.front_X, &iter.front_Y);

			iter.placeLabel_inUIGroup(uiGrp_FRN, 2, iter.front_X, iter.front_Y, 0.01, 0.02);
		}

		// SHANK R
		for (Visualizer_Block& iter : shank_R->blockVector)
		{
			getIncrementalXY_fromEndPoints(center_SAG_XY_AnkleR[0], center_SAG_XY_KneeR[0], center_SAG_XY_AnkleR[1], center_SAG_XY_KneeR[1],
				iter.idx, shank_R->numBlocks, &iter.sagit_X, &iter.sagit_Y);

			iter.placeLabel_inUIGroup(uiGrp_SAG, 1, iter.sagit_X, iter.sagit_Y, 0.01, 0.02);

			getIncrementalXY_fromEndPoints(center_FRN_XY_AnkleR[0], center_FRN_XY_KneeR[0], center_FRN_XY_AnkleR[1], center_FRN_XY_KneeR[1],
				iter.idx, shank_R->numBlocks, &iter.front_X, &iter.front_Y);

			iter.placeLabel_inUIGroup(uiGrp_FRN, 2, iter.front_X, iter.front_Y, 0.01, 0.02);
		}

		// UPDATE JOINT POSITION IN BOTH PLANES (CHECK IF SENSOR IN HIP DIRECTION ONLINE FIRST)
		hip->block->placeLabel_inUIGroup(uiGrp_SAG, 1, topLeft_SAG_XY_Hip[0], topLeft_SAG_XY_Hip[1], rel_Wd_Ht_SAG_XY_Hip[0], rel_Wd_Ht_SAG_XY_Hip[1]);
		head->block->placeLabel_inUIGroup(uiGrp_SAG, 1, topLeft_SAG_XY_Head[0], topLeft_SAG_XY_Head[1], rel_Wd_Ht_SAG_XY_Head[0], rel_Wd_Ht_SAG_XY_Head[1]);
		knee_L->block->placeLabel_inUIGroup(uiGrp_SAG, 1, topLeft_SAG_XY_KneeL[0], topLeft_SAG_XY_KneeL[1], rel_Wd_Ht_SAG_XY_KneeL[0], rel_Wd_Ht_SAG_XY_KneeL[1]);
		knee_R->block->placeLabel_inUIGroup(uiGrp_SAG, 1, topLeft_SAG_XY_KneeR[0], topLeft_SAG_XY_KneeR[1], rel_Wd_Ht_SAG_XY_KneeR[0], rel_Wd_Ht_SAG_XY_KneeR[1]);
		ankle_L->block->placeLabel_inUIGroup(uiGrp_SAG, 1, topLeft_SAG_XY_AnkleL[0], topLeft_SAG_XY_AnkleL[1], rel_Wd_Ht_SAG_XY_AnkleL[0], rel_Wd_Ht_SAG_XY_AnkleL[1]);
		ankle_R->block->placeLabel_inUIGroup(uiGrp_SAG, 1, topLeft_SAG_XY_AnkleR[0], topLeft_SAG_XY_AnkleR[1], rel_Wd_Ht_SAG_XY_AnkleR[0], rel_Wd_Ht_SAG_XY_AnkleR[1]);
		
		hip->block->placeLabel_inUIGroup(uiGrp_FRN, 2, topLeft_FRN_XY_Hip[0], topLeft_FRN_XY_Hip[1], rel_Wd_Ht_FRN_XY_Hip[0], rel_Wd_Ht_FRN_XY_Hip[1]);
		head->block->placeLabel_inUIGroup(uiGrp_FRN, 2, topLeft_FRN_XY_Head[0], topLeft_FRN_XY_Head[1], rel_Wd_Ht_FRN_XY_Head[0], rel_Wd_Ht_FRN_XY_Head[1]);
		knee_L->block->placeLabel_inUIGroup(uiGrp_FRN, 2, topLeft_FRN_XY_KneeL[0], topLeft_FRN_XY_KneeL[1], rel_Wd_Ht_FRN_XY_KneeL[0], rel_Wd_Ht_FRN_XY_KneeL[1]);
		knee_R->block->placeLabel_inUIGroup(uiGrp_FRN, 2, topLeft_FRN_XY_KneeR[0], topLeft_FRN_XY_KneeR[1], rel_Wd_Ht_FRN_XY_KneeR[0], rel_Wd_Ht_FRN_XY_KneeR[1]);
		ankle_L->block->placeLabel_inUIGroup(uiGrp_FRN, 2, topLeft_FRN_XY_AnkleL[0], topLeft_FRN_XY_AnkleL[1], rel_Wd_Ht_FRN_XY_AnkleL[0], rel_Wd_Ht_FRN_XY_AnkleL[1]);
		ankle_R->block->placeLabel_inUIGroup(uiGrp_FRN, 2, topLeft_FRN_XY_AnkleR[0], topLeft_FRN_XY_AnkleR[1], rel_Wd_Ht_FRN_XY_AnkleR[0], rel_Wd_Ht_FRN_XY_AnkleR[1]);

		// UPDATE KNEE ANGLE TEXT
		knee_L->block->front->setText(String(ang_Knee_L, 2), dontSendNotification);
		knee_R->block->front->setText(String(ang_Knee_R, 2), dontSendNotification);

		// SET COLOUR BASED ON STANCE OR SWING
		if (isStance_L)
		{
			ankle_L->block->front->setColour(ankle_L->block->front->backgroundColourId, Colours::green);
			ankle_L->block->sagit->setColour(ankle_L->block->sagit->backgroundColourId, Colours::green);
			ankle_L->block->front->setText("Stance", dontSendNotification);
		}

		else
		{
			ankle_L->block->front->setColour(ankle_L->block->front->backgroundColourId, Colours::yellow);
			ankle_L->block->sagit->setColour(ankle_L->block->sagit->backgroundColourId, Colours::yellow);
			ankle_L->block->front->setText("Swing", dontSendNotification);
		}

		if (isStance_R)
		{
			ankle_R->block->front->setColour(ankle_R->block->front->backgroundColourId, Colours::green);
			ankle_R->block->sagit->setColour(ankle_R->block->sagit->backgroundColourId, Colours::green);
			ankle_R->block->front->setText("Stance", dontSendNotification);
		}

		else
		{
			ankle_R->block->front->setColour(ankle_R->block->front->backgroundColourId, Colours::yellow);
			ankle_R->block->sagit->setColour(ankle_R->block->sagit->backgroundColourId, Colours::yellow);
			ankle_R->block->front->setText("Swing", dontSendNotification);
		}

		// UPDATE COLOUR BASED ON STATIONARY ON TURNING
		hip->block->front->setColour(hip->block->front->backgroundColourId, Colours::yellow);
		if (isStationary)
			hip->block->front->setColour(hip->block->front->backgroundColourId, Colours::orange);
		if (isTurning)
			hip->block->front->setColour(hip->block->front->backgroundColourId, Colours::blue);
			
	}

	void setJointPositions(UI_ControlGroup* uiGrp_SAG, UI_ControlGroup* uiGrp_FRN)
	{
		// CUSTOM COORDINATES RELATIVE TO UI CONTROL GROUP // X , Y // 

		// GET HIP TOP LEFT COORDINATES
		uiGrp_SAG->getRelativePos_TopCorner_fromCenter(hip->block->length, hip->block->breadth, center_SAG_XY_Hip[0], center_SAG_XY_Hip[1],
			&topLeft_SAG_XY_Hip[0], &topLeft_SAG_XY_Hip[1], &rel_Wd_Ht_SAG_XY_Hip[0], &rel_Wd_Ht_SAG_XY_Hip[1]);
		uiGrp_FRN->getRelativePos_TopCorner_fromCenter(hip->block->thickness, hip->block->breadth, center_FRN_XY_Hip[0], center_FRN_XY_Hip[1],
			&topLeft_FRN_XY_Hip[0], &topLeft_FRN_XY_Hip[1], &rel_Wd_Ht_FRN_XY_Hip[0], &rel_Wd_Ht_FRN_XY_Hip[1]);

		// GET HEAD CENTER COORDINATES
		getDistalXY_fromProximalXY(1, true, center_SAG_XY_Hip[0], center_SAG_XY_Hip[1], htFrac_Trunk, trunk->pitch, trunk->roll, &center_SAG_XY_Head[0], &center_SAG_XY_Head[1]);
		getDistalXY_fromProximalXY(2, true, center_FRN_XY_Hip[0], center_FRN_XY_Hip[1], htFrac_Trunk, trunk->pitch, trunk->roll, &center_FRN_XY_Head[0], &center_FRN_XY_Head[1]);

		// GET HEAD TOPLEFT COORDINATES
		uiGrp_SAG->getRelativePos_TopCorner_fromCenter(head->block->length, head->block->breadth, center_SAG_XY_Head[0], center_SAG_XY_Head[1],
			&topLeft_SAG_XY_Head[0], &topLeft_SAG_XY_Head[1], &rel_Wd_Ht_SAG_XY_Head[0], &rel_Wd_Ht_SAG_XY_Head[1]);
		uiGrp_FRN->getRelativePos_TopCorner_fromCenter(head->block->thickness, head->block->breadth, center_FRN_XY_Head[0], center_FRN_XY_Head[1],
			&topLeft_FRN_XY_Head[0], &topLeft_FRN_XY_Head[1], &rel_Wd_Ht_FRN_XY_Head[0], &rel_Wd_Ht_FRN_XY_Head[1]);

		// GET LEFT KNEE CENTER COORDINATES
		getDistalXY_fromProximalXY(1, false, center_SAG_XY_Hip[0], center_SAG_XY_Hip[1], htFrac_Thigh, thigh_L->pitch, thigh_L->roll, &center_SAG_XY_KneeL[0], &center_SAG_XY_KneeL[1]);
		getDistalXY_fromProximalXY(2, false, center_FRN_XY_Hip[0] - legOffset, center_FRN_XY_Hip[1], htFrac_Thigh, thigh_L->pitch, thigh_L->roll, &center_FRN_XY_KneeL[0], &center_FRN_XY_KneeL[1]);

		// GET LEFT ANKLE CENTER COORDINATES
		getDistalXY_fromProximalXY(1, false, center_SAG_XY_KneeL[0], center_SAG_XY_KneeL[1], htFrac_Shank, shank_L->pitch, shank_L->roll, &center_SAG_XY_AnkleL[0], &center_SAG_XY_AnkleL[1]);
		getDistalXY_fromProximalXY(2, false, center_FRN_XY_KneeL[0], center_FRN_XY_KneeL[1], htFrac_Shank, shank_L->pitch, shank_L->roll, &center_FRN_XY_AnkleL[0], &center_FRN_XY_AnkleL[1]);

		// GET RIGHT KNEE CENTER COORDINATES
		getDistalXY_fromProximalXY(1, false, center_SAG_XY_Hip[0], center_SAG_XY_Hip[1], htFrac_Thigh, thigh_R->pitch, thigh_R->roll, &center_SAG_XY_KneeR[0], &center_SAG_XY_KneeR[1]);
		getDistalXY_fromProximalXY(2, false, center_FRN_XY_Hip[0] + legOffset, center_FRN_XY_Hip[1], htFrac_Thigh, thigh_R->pitch, thigh_R->roll, &center_FRN_XY_KneeR[0], &center_FRN_XY_KneeR[1]);

		// GET RIGHT ANKLE CENTER COORDINATES
		getDistalXY_fromProximalXY(1, false, center_SAG_XY_KneeR[0], center_SAG_XY_KneeR[1], htFrac_Shank, shank_R->pitch, shank_R->roll, &center_SAG_XY_AnkleR[0], &center_SAG_XY_AnkleR[1]);
		getDistalXY_fromProximalXY(2, false, center_FRN_XY_KneeR[0], center_FRN_XY_KneeR[1], htFrac_Shank, shank_R->pitch, shank_R->roll, &center_FRN_XY_AnkleR[0], &center_FRN_XY_AnkleR[1]);

		// GET LEFT KNEE TOPLEFT COORDINATES
		uiGrp_SAG->getRelativePos_TopCorner_fromCenter(knee_L->block->length, knee_L->block->breadth, center_SAG_XY_KneeL[0], center_SAG_XY_KneeL[1],
			&topLeft_SAG_XY_KneeL[0], &topLeft_SAG_XY_KneeL[1], &rel_Wd_Ht_SAG_XY_KneeL[0], &rel_Wd_Ht_SAG_XY_KneeL[1]);
		uiGrp_FRN->getRelativePos_TopCorner_fromCenter(knee_L->block->thickness, knee_L->block->breadth, center_FRN_XY_KneeL[0], center_FRN_XY_KneeL[1],
			&topLeft_FRN_XY_KneeL[0], &topLeft_FRN_XY_KneeL[1], &rel_Wd_Ht_FRN_XY_KneeL[0], &rel_Wd_Ht_FRN_XY_KneeL[1]);

		// GET LEFT ANKLE TOPLEFT COORDINATES
		uiGrp_SAG->getRelativePos_TopCorner_fromCenter(ankle_L->block->length, ankle_L->block->breadth, center_SAG_XY_AnkleL[0], center_SAG_XY_AnkleL[1],
			&topLeft_SAG_XY_AnkleL[0], &topLeft_SAG_XY_AnkleL[1], &rel_Wd_Ht_SAG_XY_AnkleL[0], &rel_Wd_Ht_SAG_XY_AnkleL[1]);
		uiGrp_FRN->getRelativePos_TopCorner_fromCenter(ankle_L->block->thickness, ankle_L->block->breadth, center_FRN_XY_AnkleL[0], center_FRN_XY_AnkleL[1],
			&topLeft_FRN_XY_AnkleL[0], &topLeft_FRN_XY_AnkleL[1], &rel_Wd_Ht_FRN_XY_AnkleL[0], &rel_Wd_Ht_FRN_XY_AnkleL[1]);

		// GET RIGHT KNEE TOPLEFT COORDINATES
		uiGrp_SAG->getRelativePos_TopCorner_fromCenter(knee_R->block->length, knee_R->block->breadth, center_SAG_XY_KneeR[0], center_SAG_XY_KneeR[1],
			&topLeft_SAG_XY_KneeR[0], &topLeft_SAG_XY_KneeR[1], &rel_Wd_Ht_SAG_XY_KneeR[0], &rel_Wd_Ht_SAG_XY_KneeR[1]);
		uiGrp_FRN->getRelativePos_TopCorner_fromCenter(knee_R->block->thickness, knee_R->block->breadth, center_FRN_XY_KneeR[0], center_FRN_XY_KneeR[1],
			&topLeft_FRN_XY_KneeR[0], &topLeft_FRN_XY_KneeR[1], &rel_Wd_Ht_FRN_XY_KneeR[0], &rel_Wd_Ht_FRN_XY_KneeR[1]);

		// GET RIGHT ANKLE TOPLEFT COORDINATES
		uiGrp_SAG->getRelativePos_TopCorner_fromCenter(ankle_R->block->length, ankle_R->block->breadth, center_SAG_XY_AnkleR[0], center_SAG_XY_AnkleR[1],
			&topLeft_SAG_XY_AnkleR[0], &topLeft_SAG_XY_AnkleR[1], &rel_Wd_Ht_SAG_XY_AnkleR[0], &rel_Wd_Ht_SAG_XY_AnkleR[1]);
		uiGrp_FRN->getRelativePos_TopCorner_fromCenter(ankle_R->block->thickness, ankle_R->block->breadth, center_FRN_XY_AnkleR[0], center_FRN_XY_AnkleR[1],
			&topLeft_FRN_XY_AnkleR[0], &topLeft_FRN_XY_AnkleR[1], &rel_Wd_Ht_FRN_XY_AnkleR[0], &rel_Wd_Ht_FRN_XY_AnkleR[1]);
	}

	void setVisible(bool on)
	{
		head->setVisible(on);
		hip->setVisible(on);
		knee_L->setVisible(on);
		knee_R->setVisible(on);
		ankle_L->setVisible(on);
		ankle_R->setVisible(on);
		trunk->setVisible(on);
		thigh_L->setVisible(on);
		thigh_R->setVisible(on);
		shank_L->setVisible(on);
		shank_R->setVisible(on);
	}

	void getDistalXY_fromProximalXY(short plane, bool isDist_Above_Hip, float x_Prox, float y_Prox, float length_Rel, float pitch, float roll, float* x_Dist, float* y_Dist)
	{
		float temp = 0;
		switch(plane)
		{
		case 1:
			break;
		case 2:
			temp = roll;
			roll = pitch;
			pitch = temp;
			break;
		}

		float pitchRad = pitch * M_PI / 180.0;
		float rollRad = roll * M_PI / 180.0;
		float rollRad_COS = cos(rollRad);

		if (isDist_Above_Hip)
		{
			*x_Dist = x_Prox + length_Rel * sin(pitchRad) * rollRad_COS;
			*y_Dist = y_Prox - length_Rel * cos(pitchRad) * rollRad_COS;
		}

		else
		{
			*x_Dist = x_Prox - length_Rel * sin(pitchRad) * rollRad_COS;
			*y_Dist = y_Prox + length_Rel * cos(pitchRad) * rollRad_COS;
		}
	}

	void getIncrementalXY_fromEndPoints(float x1, float x2, float y1, float y2, int blk_Idx, int blk_Tot, float* blk_X, float* blk_Y)
	{
		float frac = (float)blk_Idx / (float)blk_Tot;
		*blk_X = x1 + (x2 - x1) * frac;
		*blk_Y = y1 + (y2 - y1) * frac;
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
