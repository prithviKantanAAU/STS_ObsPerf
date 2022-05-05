#pragma once
#include <JuceHeader.h>
#include "UtilityClasses.h"
#include "UtilityClasses_2.h"
using namespace juce;

class Main_MappingMatrix : ComboBox::Listener
{
public:
	
	// Common UI Elements
	UI_ControlGroup* uiGrp_MovFeat;
	UI_ControlGroup* uiGrp_MapMatrix;
	UI_ControlGroup* uiGrp_AudFeat;
	UI_ControlGroup* uiGrp_Presets;

	// TARGET MOVEMENT FEATURE VECTOR
	std::vector<MovementFeature>* featureVec_TARGET;

	// Normalizers and Denormalizers
	std::vector<Norm_Denorm_Metering> normalizers_MF;
	std::vector<Norm_Denorm_Metering> denormalizers_AP;
	std::vector<MF_Multiplier> multipliers_MF;
	std::vector<Mapping_Quantization> mapQuants_AP;

	// Mapping Matrix
	std::vector<std::vector<MappingMatrix_Node>> mappingMatrix;

	// Hide / Show MF / AP
	ComboBox* MF_Visible;
	ComboBox* MF_Hidden;
	ComboBox* AP_Visible;
	ComboBox* AP_Hidden;

	void populateBoxes_paramVisibility()
	{
		MF_Visible->clear();
		MF_Hidden->clear();
		AP_Visible->clear();
		AP_Hidden->clear();

		MF_Visible->addListener(this);
		MF_Visible->addItem("Visible MF", 99);
		MF_Visible->setColour(MF_Visible->backgroundColourId, Colours::green);
		MF_Visible->setSelectedId(99);
		MF_Hidden->addListener(this);
		MF_Hidden->addItem("Hidden MF", 99);
		MF_Hidden->setColour(MF_Hidden->backgroundColourId, Colours::red);
		MF_Hidden->setSelectedId(99);
		AP_Visible->addListener(this);
		AP_Visible->addItem("Visible AP", 99);
		AP_Visible->setColour(AP_Visible->backgroundColourId, Colours::green);
		AP_Visible->setSelectedId(99);
		AP_Hidden->addListener(this);
		AP_Hidden->addItem("Hidden AP", 99);
		AP_Hidden->setColour(AP_Hidden->backgroundColourId, Colours::red);
		AP_Hidden->setSelectedId(99);

		for (int i = 0; i < featureVec_TARGET->size(); i++)
		{
			if (featureVec_TARGET->at(i).isVisible)
				MF_Visible->addItem(featureVec_TARGET->at(i).mpName, i + 1);
			else
				MF_Hidden->addItem(featureVec_TARGET->at(i).mpName, i + 1);
		}

		for (int i = 0; i < audParams.size(); i++)
		{
			if (audParams.at(i).isVisible)
				AP_Visible->addItem(audParams.at(i).name, i + 1);
			else
				AP_Hidden->addItem(audParams.at(i).name, i + 1);
		}
	}

	void hideAllVisibleUI()
	{
		// MF RELATED ELEMENTS
		for (int i = 0; i < featureVec_TARGET->size(); i++)
		{
			
		}

		// AP RELATED ELEMENTS
		for (int j = 0; j < audParams.size(); j++)
		{
			
		}

		// MAPPING MATRIX
		for (int i = 0; i < featureVec_TARGET->size(); i++)
		{
			
		}
	}

	void comboBoxChanged(ComboBox* box)
	{
		if (box == preset_Set)
		{
			loadPreset(box->getSelectedId());
		}

		if (box->getSelectedId() != 99)
		{
			if (box == MF_Visible)
			{
				featureVec_TARGET->at(box->getSelectedId() - 1).isVisible = false;
				
				featureVec_TARGET->at(box->getSelectedId() - 1).nameDisp->setVisible(false);
				normalizers_MF.at(box->getSelectedId() - 1).setVisible(false);
				multipliers_MF.at(box->getSelectedId() - 1).multFactor->setVisible(false);
				for (int j = 0; j < audParams.size(); j++)
				{
					mappingMatrix.at(box->getSelectedId() - 1).at(j).onOff->setVisible(false);
				}
			}

			if (box == MF_Hidden)
			{
				featureVec_TARGET->at(box->getSelectedId() - 1).isVisible = true;

				featureVec_TARGET->at(box->getSelectedId() - 1).nameDisp->setVisible(true);
				normalizers_MF.at(box->getSelectedId() - 1).setVisible(true);
				multipliers_MF.at(box->getSelectedId() - 1).multFactor->setVisible(true);
				for (int j = 0; j < audParams.size(); j++)
				{
					if (audParams.at(j).isVisible)
					mappingMatrix.at(box->getSelectedId() - 1).at(j).onOff->setVisible(true);
				}
			}

			if (box == AP_Visible)
			{
				audParams.at(box->getSelectedId() - 1).isVisible = false;

				audParams.at(box->getSelectedId() - 1).nameDisp->setVisible(false);
				mapQuants_AP.at(box->getSelectedId() - 1).setVisible(false);
				denormalizers_AP.at(box->getSelectedId() - 1).setVisible(false);

				for (int j = 0; j < featureVec_TARGET->size(); j++)
				{
					mappingMatrix.at(j).at(box->getSelectedId() - 1).onOff->setVisible(false);
				}
			}

			if (box == AP_Hidden)
			{
				audParams.at(box->getSelectedId() - 1).isVisible = true;

				audParams.at(box->getSelectedId() - 1).nameDisp->setVisible(true);
				mapQuants_AP.at(box->getSelectedId() - 1).setVisible(true);
				denormalizers_AP.at(box->getSelectedId() - 1).setVisible(true);

				for (int j = 0; j < featureVec_TARGET->size(); j++)
				{
					if (featureVec_TARGET->at(j).isVisible)
					mappingMatrix.at(j).at(box->getSelectedId() - 1).onOff->setVisible(true);
				}
			}

			hideAllVisibleUI();
			populateBoxes_paramVisibility();
			arrangeUI();
			box->setSelectedId(99);
		}
	}

	// Audio Parameters FAUST
	std::vector<AudioParameter_FAUST> audParams;
	std::vector<double> mapQuant_Norm_Inputs;
	std::vector<double> isAP_Mapped;

	// LAYOUT VARIABLES
	float within_UIgrp_startX_frac = 0.05;
	float within_UIgrp_endX_frac = 0.95;
	float within_UIgrp_startY_frac = 0.05;
	float within_UIgrp_endY_frac = 0.95;
	float frac_MF_row_ht = 0;
	float frac_AP_col_wd = 0;
	int num_MF_vis = 0;
	int num_AP_vis = 0;
	
	std::vector<int> MF_vis_indices;
	std::vector<int> AP_vis_indices;
	std::vector<float> MF_row_startY_vals;
	std::vector<float> AP_col_startX_vals;

	Main_MappingMatrix()
	{
		// INITIALIZE UI CONTROL GROUPS
		uiGrp_MovFeat = new UI_ControlGroup("Movement Features", Colours::green);
		uiGrp_AudFeat = new UI_ControlGroup("Audio Parameters", Colours::blue);
		uiGrp_MapMatrix = new UI_ControlGroup("Mapping Matrix", Colours::red);
		uiGrp_Presets = new UI_ControlGroup("Presets", Colours::red);
		
		// INITIALIZE VISIBLE PARAM COMBO BOXES
		MF_Visible = new ComboBox;
		MF_Hidden = new ComboBox;
		AP_Visible = new ComboBox;
		AP_Hidden = new ComboBox;

		// DEFINE AUDIO PARAMETERS
		AudioParameter_FAUST* audParam_Ptr;

		// TRIGGERS
		audParam_Ptr = new AudioParameter_FAUST("T_Djemb", 0, 100, 0, 1, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("T_Voice", 50, 100, 50, 3, 1);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("T_Piano", 0, 100, 0, 5, 1);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("T_Flute", 0, 100, 0, 24, 1);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("T_Gtr", 0, 100, 0, 12, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("T_Bell", 0, 100, 0, 17, 1);
		audParams.push_back(*audParam_Ptr);

		// FUNDAMENTAL FREQUENCIES
		audParam_Ptr = new AudioParameter_FAUST("F_Djemb", 150, 600, 150, 9, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("F_Voice", 0, 15, 1, 2, 1);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("F_Piano", 1, 9, 1, 4, 4);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("F_Flute", 0, 9, 0, 8, 1);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("F_Gtr", 0, 15, 1, 19, 1, false);
		audParams.push_back(*audParam_Ptr);

		// DYNAMICS
		audParam_Ptr = new AudioParameter_FAUST("D_Djemb", 0.01, 4.5, 1, 15, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("D_Voice", 0, 1, 1, 14, 1);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("D_Piano", 0, 1, 1, 7, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("D_Flute", 0.4, 1, 0.4, 16, 1);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("D_Gtr", 20, 1500, 1500, 18, 1);
		audParams.push_back(*audParam_Ptr);

		// TIMBRE
		audParam_Ptr = new AudioParameter_FAUST("Vowel", 0, 2, 0, 11, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("Gtr Stf", 0.01, 0.4, 0.1, 13, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("Flute Vib", 0.036, 0.13, 0.036, 25, 1, false);
		audParams.push_back(*audParam_Ptr);

		// NEGATIVE FEEDBACK
		audParam_Ptr = new AudioParameter_FAUST("Detune", 0, 1, 0, 6, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("F_Warp", 0.5, 1, 0.5, 10, 1);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("Flute Pos", 0, 0.5, 0.5, 23, 1, false);
		audParams.push_back(*audParam_Ptr);

		// SINE WAVES
		audParam_Ptr = new AudioParameter_FAUST("Sin1 F", 300, 500, 300, 20, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("Sin2 F", 600, 1000, 600, 21, 1, false);
		audParams.push_back(*audParam_Ptr);
		audParam_Ptr = new AudioParameter_FAUST("Sin3 F", 1200, 2000, 1200, 22, 1, false);
		audParams.push_back(*audParam_Ptr);

		for (int i = 0; i < audParams.size(); i++)
		{
			mapQuant_Norm_Inputs.push_back(0);
			isAP_Mapped.push_back(false);
		}

		preset_init_UI();
	}

	~Main_MappingMatrix()
	{

	}

	void initialize(std::vector<MovementFeature>* featureVec)
	{
		featureVec_TARGET = featureVec;
		Norm_Denorm_Metering* ptr_norm_MF;
		Norm_Denorm_Metering* ptr_denorm_AP;
		MF_Multiplier* ptr_mult_MF;
		Mapping_Quantization* ptr_mapQuant_AP;
		MappingMatrix_Node* ptr_mapMat_Node;

		// CREATE NORMALIZERS AND MULTIPLIERS FOR ALL MOVEMENT FEATURES
		for (MovementFeature& iter : *featureVec)
		{
			ptr_norm_MF = new Norm_Denorm_Metering(iter.mpName,1,iter.minVal,iter.maxVal);
			normalizers_MF.push_back(*ptr_norm_MF);
		}

		for (MovementFeature& iter : *featureVec)
		{
			ptr_mult_MF = new MF_Multiplier(featureVec,&normalizers_MF);
			multipliers_MF.push_back(*ptr_mult_MF);
		}

		// POPULATE MAPPING MATRIX
		std::vector<MappingMatrix_Node> mapMat_row;

		for (MovementFeature& iter1 : *featureVec)
		{
			mapMat_row.clear();

			for (AudioParameter_FAUST& iter2 : audParams)
			{
				ptr_mapMat_Node = new MappingMatrix_Node;
				mapMat_row.push_back(*ptr_mapMat_Node);
			}

			mappingMatrix.push_back(mapMat_row);
		}

		// CREATE MAPPERS AND DENORMALIZERS FOR ALL AUDIO PARAMETERS
		for (AudioParameter_FAUST& iter : audParams)
		{
			ptr_mapQuant_AP = new Mapping_Quantization(50, true, 1, 0);
			mapQuants_AP.push_back(*ptr_mapQuant_AP);

			ptr_denorm_AP = new Norm_Denorm_Metering(iter.name, 2, iter.minVal, iter.maxVal);
			denormalizers_AP.push_back(*ptr_denorm_AP);
		}

		populateBoxes_paramVisibility();
		// ARRANGE UI ONSCREEN
		// arrangeUI(featureVec);
	}

	void callback(std::vector<MovementFeature>* featureVec)
	{
		// HANDLE MF PREPROCESSING
		for (int i = 0; i < featureVec->size(); i++)
		{
			if (featureVec->at(i).isVisible)
			{
				normalizers_MF.at(i).processInput(featureVec->at(i).val);
				multipliers_MF.at(i).applyFeatureMult(*normalizers_MF.at(i).outputVal);
			}
		}

		// CARRY OUT MAPPING TO AP INPUTS
		map_MF_AP();

		// PROCESS AP TO YIELD FINAL VALUE
		for (int i = 0; i < audParams.size(); i++)
		{
			if (audParams.at(i).isVisible)
			{
				if (isAP_Mapped.at(i))
				{
					mapQuants_AP.at(i).process(mapQuant_Norm_Inputs[i]);
					denormalizers_AP.at(i).processInput(*mapQuants_AP.at(i).outputVal);
				}

				else 
					*denormalizers_AP.at(i).outputVal = audParams.at(i).defaultVal;

				audParams.at(i).val = *denormalizers_AP.at(i).outputVal;
			}

			else isAP_Mapped.at(i) = false;
		}
	}

	void map_MF_AP()
	{
		for (int i = 0; i < audParams.size(); i++)
			mapQuant_Norm_Inputs[i] = 0;

		for (int j = 0; j < num_AP_vis; j++)
		{
			isAP_Mapped.at(AP_vis_indices[j]) = false;
			for (int i = 0; i < num_MF_vis; i++)
			{
				bool* mappingTrue = mappingMatrix.at(MF_vis_indices[i]).at(AP_vis_indices[j]).isOn;
				if (*mappingTrue)
				{
					isAP_Mapped.at(AP_vis_indices[j]) = true;
					mapQuant_Norm_Inputs[AP_vis_indices[j]] += *multipliers_MF.at(MF_vis_indices[i]).outputVal;
				}
			}

			mapQuant_Norm_Inputs[AP_vis_indices[j]] = jlimit(0.0, 1.0, mapQuant_Norm_Inputs[AP_vis_indices[j]]);
		}
	}

	void setVisible(bool on)
	{
		uiGrp_MovFeat->setVisible(on);
		uiGrp_AudFeat->setVisible(on);
		uiGrp_MapMatrix->setVisible(on);
		uiGrp_Presets->setVisible(on);
		MF_Visible->setVisible(on);
		MF_Hidden->setVisible(on);
		AP_Visible->setVisible(on);
		AP_Hidden->setVisible(on);
		preset_enter_Name->setVisible(on);
		preset_hit_Save->setVisible(on);
		preset_Set->setVisible(on);

		// MF RELATED ELEMENTS
		for (int i = 0; i < featureVec_TARGET->size(); i++)
		{
			featureVec_TARGET->at(i).nameDisp->setVisible(false);
			normalizers_MF.at(i).setVisible(false);
			multipliers_MF.at(i).multFactor->setVisible(false);

			if (featureVec_TARGET->at(i).isVisible)
			{
				featureVec_TARGET->at(i).nameDisp->setVisible(on);
				normalizers_MF.at(i).setVisible(on);
				multipliers_MF.at(i).multFactor->setVisible(on);
			}
		}

		// AP RELATED ELEMENTS
		for (int j = 0; j < audParams.size(); j++)
		{
			audParams.at(j).nameDisp->setVisible(false);
			mapQuants_AP.at(j).setVisible(false);
			denormalizers_AP.at(j).setVisible(false);

			if (audParams.at(j).isVisible)
			{
				audParams.at(j).nameDisp->setVisible(on);
				mapQuants_AP.at(j).setVisible(on);
				denormalizers_AP.at(j).setVisible(on);
			}
		}

		// MAPPING MATRIX
		for (int i = 0; i < featureVec_TARGET->size(); i++)
		{
			for (int j = 0; j < audParams.size(); j++)
			{
				mappingMatrix.at(i).at(j).onOff->setVisible(false);

				if (featureVec_TARGET->at(i).isVisible && audParams.at(j).isVisible)
					mappingMatrix.at(i).at(j).onOff->setVisible(on);
			}
		}
	}

	void updateElements()
	{
		for (int i = 0; i < featureVec_TARGET->size(); i++)
		{
			if (featureVec_TARGET->at(i).isVisible)
				normalizers_MF.at(i).updateMeter();
		}

		for (int i = 0; i < audParams.size(); i++)
		{
			if (audParams.at(i).isVisible)
				denormalizers_AP.at(i).updateMeter();
		}
	}

	void setLayout(int wd, int ht)
	{
		uiGrp_MovFeat->getBoundingRec(wd, ht, 0.05, 0.1, 0.34, 0.53);
		uiGrp_MapMatrix->getBoundingRec(wd, ht, 0.4, 0.1, 0.58, 0.53);
		uiGrp_AudFeat->getBoundingRec(wd, ht, 0.4, 0.65, 0.58, 0.33);
		uiGrp_Presets->getBoundingRec(wd, ht, 0.05, 0.65, 0.34, 0.33);

		MF_Visible->setBounds(wd * 0.4, 0, wd * 0.14, 30);
		MF_Hidden->setBounds(wd * 0.55, 0, wd * 0.14, 30);
		AP_Visible->setBounds(wd * 0.7, 0, wd * 0.14, 30);
		AP_Hidden->setBounds(wd * 0.85, 0, wd * 0.14, 30);
	}

	void populate_RowCol_StartPos(int num_MF_vis, int num_AP_vis)
	{
		MF_row_startY_vals.clear();
		AP_col_startX_vals.clear();

		frac_MF_row_ht = (within_UIgrp_endY_frac - within_UIgrp_startY_frac) / (float)num_MF_vis;
		frac_AP_col_wd = (within_UIgrp_endX_frac - within_UIgrp_startX_frac) / (float)num_AP_vis;

		float nextRow_Y_frac = within_UIgrp_startY_frac - frac_MF_row_ht;
		float nextCol_X_frac = within_UIgrp_startX_frac - frac_AP_col_wd;

		for (int i = 0; i < num_MF_vis; i++)
		{
			nextRow_Y_frac += frac_MF_row_ht;
			MF_row_startY_vals.push_back(nextRow_Y_frac);
		}

		for (int j = 0; j < num_AP_vis; j++)
		{
			nextCol_X_frac += frac_AP_col_wd;
			AP_col_startX_vals.push_back(nextCol_X_frac);
		}
	}

	void arrangeUI()
	{
		// POPULATE VISIBLE PARAM INDICES
		num_MF_vis = 0;
		num_AP_vis = 0;
		MF_vis_indices.clear();
		AP_vis_indices.clear();

		int iterator = 0;
		for (MovementFeature& iter : *featureVec_TARGET)
		{
			if (iter.isVisible)
			{
				num_MF_vis++;
				MF_vis_indices.push_back(iterator);
			}
			iterator++;
		}

		iterator = 0;
		for (AudioParameter_FAUST& iter : audParams)
		{
			if (iter.isVisible)
			{
				num_AP_vis++;
				AP_vis_indices.push_back(iterator);
			}
			iterator++;
		}

		populate_RowCol_StartPos(num_MF_vis, num_AP_vis);

		// PUT MF UI IN PLACE
		for (int i = 0; i < num_MF_vis; i++)
		{
			uiGrp_MovFeat->memberComponent_setBounds(
				featureVec_TARGET->at(MF_vis_indices[i]).nameDisp,
				0,
				MF_row_startY_vals[i],
				0.35,
				frac_MF_row_ht * 0.95
			);

			normalizers_MF.at(MF_vis_indices[i]).setLayout(
				uiGrp_MovFeat,
				0.38,
				MF_row_startY_vals[i],
				0.4,
				frac_MF_row_ht * 0.95
			);

			uiGrp_MovFeat->memberComponent_setBounds(
				multipliers_MF.at(MF_vis_indices[i]).multFactor,
				0.8,
				MF_row_startY_vals[i],
				0.19,
				frac_MF_row_ht * 0.95
			);
		}

		// PUT AP UI IN PLACE
		for (int i = 0; i < num_AP_vis; i++)
		{
			uiGrp_AudFeat->memberComponent_setBounds(
				audParams.at(AP_vis_indices[i]).nameDisp,
				AP_col_startX_vals[i],
				0.9,
				frac_AP_col_wd,
				0.1
			);

			denormalizers_AP.at(AP_vis_indices[i]).setLayout(
				uiGrp_AudFeat,
				AP_col_startX_vals[i],
				0.75,
				frac_AP_col_wd,
				0.14
			);

			mapQuants_AP.at(AP_vis_indices[i]).setLayout(
				uiGrp_AudFeat,
				AP_col_startX_vals[i],
				0.05,
				frac_AP_col_wd,
				0.73
			);
		}

		// PUT MAPPING MATRIX IN PLACE
		for (int i = 0; i < num_MF_vis; i++)
		{
			for (int j = 0; j < num_AP_vis; j++)
			{
				uiGrp_MapMatrix->memberComponent_setBounds(
					mappingMatrix.at(MF_vis_indices[i]).at(AP_vis_indices[j]).onOff,
					AP_col_startX_vals[j] + frac_AP_col_wd * 0.4,
					MF_row_startY_vals[i],
					frac_AP_col_wd * 0.95,
					frac_MF_row_ht * 0.95
				);
			}
		}

		uiGrp_Presets->memberComponent_setBounds(preset_enter_Name, 0.05, 0.1, 0.9, 0.3);
		uiGrp_Presets->memberComponent_setBounds(preset_hit_Save, 0.05, 0.5, 0.9, 0.2);
		uiGrp_Presets->memberComponent_setBounds(preset_Set, 0.05, 0.75, 0.9, 0.2);

		setVisible(false);
		setVisible(true);
	}

	////////////////////////////////////////////////// PRESET RELATED FUNCTIONALITY //////////////////////////////////////

	short preset_count = 0;
	std::vector<String> preset_names;
	String preset_folderPath = "";

	ComboBox* preset_Set;
	TextEditor* preset_enter_Name;
	TextButton* preset_hit_Save;
	FILE* presetFile;

	void preset_init_UI()
	{
		preset_Set = new ComboBox;
		preset_Set->addListener(this);
		preset_populateNames();

		preset_enter_Name = new TextEditor;
		preset_enter_Name->setJustification(Justification::centred);

		preset_hit_Save = new TextButton("Save Preset");
		preset_hit_Save->setColour(preset_hit_Save->buttonColourId, Colours::black);
		preset_hit_Save->onClick = [this]
		{
			savePreset(preset_enter_Name->getText());
			preset_enter_Name->setText("");
		};
	}

	void preset_populateNames()
	{
		preset_Set->clear();
		preset_folderPath = File::getSpecialLocation(File::currentApplicationFile).getFullPathName();
		preset_folderPath = preset_folderPath.upToLastOccurrenceOf("\\", true, false) + "Mapping Presets\\";

		auto dir_Base = File(preset_folderPath);
		preset_count = dir_Base.getNumberOfChildFiles(2, "*.csv");
		auto presetFiles = dir_Base.findChildFiles(2, false, "*.csv");
		presetFiles.sort();
		File currentFile;

		for (int i = 0; i < preset_count; i++)						// Load Styles from Defined Directory
		{
			currentFile = presetFiles.getUnchecked(i);
			preset_names.push_back(currentFile.getFileNameWithoutExtension());
			preset_Set->addItem(currentFile.getFileNameWithoutExtension(), i + 1);
		}
	}

	void savePreset(String name)
	{
		if (name == "") return;

		String presetFile_Path = "";
		// INITIALIZE PATHS, STORE INFO
		presetFile_Path = File::getSpecialLocation(File::currentApplicationFile).getFullPathName();
		presetFile_Path = presetFile_Path.upToLastOccurrenceOf("\\", true, false);
		presetFile_Path += "Mapping Presets\\" + name + ".csv";

		int toWrite_numRows =
			1 +												// First Header
			1 +												// Second Header
			featureVec_TARGET->size() +						// Movement Feature-Related
			1 +												// AP Smoothing Fcs
			1 +												// AP Polarity
			1 +												// AP Map Function
			1 +												// AP Num Quant Levels
			1 +												// AP Norm Bound L
			1;												// AP Norm Bound U

		int toWrite_numCols =
			1 +												// Movement Feature Name / Quantity
			1 +												// Movement Feature Visibility
			1 +												// MF NormBound L
			1 +												// MF NormBound U
			1 +												// MF Mult Feature Name
			audParams.size();								// Audio Parameter Names

		presetFile = fopen(presetFile_Path.toStdString().c_str(), "w");

		std::vector<String> rowData;
		String currentRow = "";

		// WRITE DATA to String vector
		for (int i = 0; i < toWrite_numRows; i++)
		{
			currentRow = "";

			if (i == 0)												// First Header
			{
				for (int j = 0; j < (toWrite_numCols - audParams.size()); j++)
					currentRow += ",";

				for (int k = 0; k < audParams.size(); k++)
					currentRow += audParams.at(k).name + ",";

				rowData.push_back(currentRow.upToLastOccurrenceOf(",", false, true));
				continue;
			}

			if (i == 1)												// Second Header
			{
				currentRow += ",";
				currentRow += "Visibility" ",";
				currentRow += "MP_NormBound_L" ",";
				currentRow += "MP_NormBound_U" ",";
				currentRow += "Name_MP_Mult" ",";

				for (int k = 0; k < audParams.size(); k++)
					currentRow += audParams.at(k).isVisible ? "Visible," : "Invisible," ;

				rowData.push_back(currentRow.upToLastOccurrenceOf(",", false, true));
				continue;
			}

			if (i >= 2 && i < 2 + featureVec_TARGET->size())		// Movement Features
			{
				currentRow += featureVec_TARGET->at(i - 2).mpName + ",";
				currentRow += String(featureVec_TARGET->at(i - 2).isVisible ? "Yes" : "No") + ",";
				currentRow += String(*normalizers_MF.at(i - 2).targetVar_Bound_Rel_L) + ",";
				currentRow += String(*normalizers_MF.at(i - 2).targetVar_Bound_Rel_U) + ",";
				short multFeat_ID = multipliers_MF.at(i - 2).multFactor->getSelectedId();
				currentRow += multipliers_MF.at(i - 2).multFactor->getItemText(multFeat_ID - 1) + ",";
				
				for (int k = 0; k < audParams.size(); k++)
				{
					currentRow += *mappingMatrix.at(i - 2).at(k).isOn ? "1," : "0,";
				}

				rowData.push_back(currentRow.upToLastOccurrenceOf(",",false,true));
				continue;
			}

			int apIdx = 0;
			for (int k = 0; k < toWrite_numCols; k++)
			{
				apIdx = k - toWrite_numCols + audParams.size();

				if (k > 0 && (k < toWrite_numCols - audParams.size()))
					currentRow += ",";

				if (i == (2 + featureVec_TARGET->size()))				// AP Smooth FC
				{
					if (k == 0)
						currentRow += "AP_Smooth_Fc,";

					if (k >= toWrite_numCols - audParams.size())
					{
						currentRow += String(*mapQuants_AP.at(apIdx).smoothing_Fc) + ",";
					}

					continue;
				}

				if (i == (3 + featureVec_TARGET->size()))				// AP Polarity
				{
					if (k == 0)
						currentRow += "AP_Polarity,";

					if (k >= toWrite_numCols - audParams.size())
					{
						currentRow += String(*mapQuants_AP.at(apIdx).isPolarity_Pos ? 1 : 0) + ",";
					}

					continue;
				}

				if (i == (4 + featureVec_TARGET->size()))				// AP Map Func
				{
					if (k == 0)
						currentRow += "AP_MapFunc,";

					if (k >= toWrite_numCols - audParams.size())
					{
						currentRow += String(mapQuants_AP.at(apIdx).set_MapFunc->getItemText(
							*mapQuants_AP.at(apIdx).mapFunc - 1)) + ",";
					}

					continue;
				}

				if (i == (5 + featureVec_TARGET->size()))				// AP Num QUant
				{
					if (k == 0)
						currentRow += "AP_NumQuant,";

					if (k >= toWrite_numCols - audParams.size())
					{
						currentRow += String(*mapQuants_AP.at(apIdx).quantLevels) + ",";
					}

					continue;
				}

				if (i == (6 + featureVec_TARGET->size()))				// AP NormBound L
				{
					if (k == 0)
						currentRow += "AP_NormBound_L,";

					if (k >= toWrite_numCols - audParams.size())
					{
						currentRow += String(*denormalizers_AP.at(apIdx).targetVar_Bound_Rel_L) + ",";
					}

					continue;
				}

				if (i == (7 + featureVec_TARGET->size()))				// AP NormBound R
				{
					if (k == 0)
						currentRow += "AP_NormBound_U,";

					if (k >= toWrite_numCols - audParams.size())
					{
						currentRow += String(*denormalizers_AP.at(apIdx).targetVar_Bound_Rel_U) + ",";
					}

					continue;
				}
			}

			rowData.push_back(currentRow.upToLastOccurrenceOf(",", false, true));
		}

		// SAVE CSV FILE
		String formatSpecifier = "%s,\n";

		for (int i = 0; i < size(rowData); i++)
		{
			fprintf(
				presetFile,
				formatSpecifier.toStdString().c_str(),
				rowData.at(i)
			);
		}

		fclose(presetFile);

		preset_populateNames();
	}

	bool loadPreset(short num)
	{
		if (num > preset_count) return false;
		String path = preset_folderPath + preset_Set->getItemText(num - 1) + ".csv";

		File* currentFile;
		currentFile = new File(path);
		juce::FileInputStream inputStream(*currentFile);

		int currentLine = 0;
		int numColumns = 0;
		std::vector<String> columnHeaders;
		std::vector<String> columnHeaders_L2;
		std::vector<String> rowData_VECTOR;


		if (!inputStream.openedOk())
			return false;  // failed to open

		while (!inputStream.isExhausted())
		{
			auto line = inputStream.readNextLine();
			auto line_Rem = line;

			rowData_VECTOR.clear();
			String rowHeader = line.upToFirstOccurrenceOf(",", false, true);

			if (currentLine == 0)
			{
				while (line_Rem != "")
				{
					columnHeaders.push_back(line_Rem.upToFirstOccurrenceOf(",", false, true));
					line_Rem = line_Rem.fromFirstOccurrenceOf(",", false, true);
					numColumns++;
				}
			}

			if (currentLine == 1)
			{
				while (line_Rem != "")
				{
					columnHeaders_L2.push_back(line_Rem.upToFirstOccurrenceOf(",", false, true));
					line_Rem = line_Rem.fromFirstOccurrenceOf(",", false, true);
				}
			}

			if (currentLine > 1)
			{
				while (line_Rem != "")
				{
					rowData_VECTOR.push_back(line_Rem.upToFirstOccurrenceOf(",", false, true));
					line_Rem = line_Rem.fromFirstOccurrenceOf(",", false, true);
				}

				for (int i = 0; i < numColumns; i++)
				{
					for (int k = 0; k < featureVec_TARGET->size(); k++)
					{
						if (rowHeader == featureVec_TARGET->at(k).mpName)
						{
							if (columnHeaders_L2.at(i) == "MP_NormBound_L")
								normalizers_MF.at(k).subsetRange->setMinValue(rowData_VECTOR.at(i).getDoubleValue());

							if (columnHeaders_L2.at(i) == "MP_NormBound_U")
								normalizers_MF.at(k).subsetRange->setMaxValue(rowData_VECTOR.at(i).getDoubleValue());

							if (columnHeaders_L2.at(i) == "Visibility")
							{
								if (rowData_VECTOR.at(i) == "Yes" && !featureVec_TARGET->at(k).isVisible)
								{
									featureVec_TARGET->at(k).isVisible = true;
									featureVec_TARGET->at(k).nameDisp->setVisible(true);
									normalizers_MF.at(k).setVisible(true);
									multipliers_MF.at(k).multFactor->setVisible(true);
									for (int j = 0; j < audParams.size(); j++)
									{
										mappingMatrix.at(k).at(j).onOff->setVisible(true);
									}
								}

								if (rowData_VECTOR.at(i) == "No" && featureVec_TARGET->at(k).isVisible)
								{
									featureVec_TARGET->at(k).isVisible = false;
									featureVec_TARGET->at(k).nameDisp->setVisible(false);
									normalizers_MF.at(k).setVisible(false);
									multipliers_MF.at(k).multFactor->setVisible(false);
									for (int j = 0; j < audParams.size(); j++)
									{
										mappingMatrix.at(k).at(j).onOff->setVisible(false);
									}
								}
							}

							if (columnHeaders_L2.at(i) == "Name_MP_Mult")
							{
								if (rowData_VECTOR.at(i) == "NONE")
									multipliers_MF.at(k).multFactor->setSelectedId(1);

								else 
								{
									for (int l = 0; l < featureVec_TARGET->size(); l++)
									{
										if (rowData_VECTOR.at(i) == featureVec_TARGET->at(l).mpName)
											multipliers_MF.at(k).multFactor->setSelectedId(l + 2);
									}
								}
							}							
						}
					}

					for (int j = 0; j < audParams.size(); j++)
					{
						if (columnHeaders.at(i) == audParams.at(j).name)
						{
							for (int k = 0; k < featureVec_TARGET->size(); k++)
							{
								if (rowHeader == featureVec_TARGET->at(k).mpName)
								{
									bool on = rowData_VECTOR.at(i).getIntValue() == 1 ? true : false;
									mappingMatrix.at(k).at(j).onOff->setToggleState(on, sendNotificationSync);
								}
							}

							if (rowHeader == "AP_Smooth_Fc")
								mapQuants_AP.at(j).set_paramSmooth_Fc->setValue(rowData_VECTOR.at(i).getDoubleValue());

							if (rowHeader == "AP_Polarity")
							{
								short polarity = rowData_VECTOR.at(i).getIntValue() == 1 ? 1 : 2;
								mapQuants_AP.at(j).set_Polarity->setSelectedId(polarity);
							}

							if (rowHeader == "AP_MapFunc")
							{
								for (int k = 0; k < mapQuants_AP.at(j).set_MapFunc->getNumItems(); k++)
								{
									if (rowData_VECTOR.at(i) == mapQuants_AP.at(j).set_MapFunc->getItemText(k))
									{
										DBG(rowData_VECTOR.at(i));
										mapQuants_AP.at(j).set_MapFunc->setSelectedId(k + 1);
									}
								}
							}

							if (rowHeader == "AP_NumQuant")
							{
								mapQuants_AP.at(j).set_QuantLevels->setValue(rowData_VECTOR.at(i).getDoubleValue());
							}

							if (rowHeader == "AP_NormBound_L")
								denormalizers_AP.at(j).subsetRange->setMinValue(rowData_VECTOR.at(i).getDoubleValue());

							if (rowHeader == "AP_NormBound_U")
								denormalizers_AP.at(j).subsetRange->setMaxValue(rowData_VECTOR.at(i).getDoubleValue());

							if (columnHeaders_L2.at(i) == "Visible" && !audParams.at(j).isVisible)
							{
								audParams.at(j).isVisible = true;
								audParams.at(j).nameDisp->setVisible(true);
								mapQuants_AP.at(j).setVisible(true);
								denormalizers_AP.at(j).setVisible(true);

								for (int l = 0; l < featureVec_TARGET->size(); l++)
								{
									mappingMatrix.at(l).at(j).onOff->setVisible(true);
								}
							}

							if (columnHeaders_L2.at(i) == "Invisible" && audParams.at(j).isVisible)
							{
								audParams.at(j).isVisible = false;
								audParams.at(j).nameDisp->setVisible(false);
								mapQuants_AP.at(j).setVisible(false);
								denormalizers_AP.at(j).setVisible(false);

								for (int l = 0; l < featureVec_TARGET->size(); l++)
								{
									mappingMatrix.at(l).at(j).onOff->setVisible(false);
								}
							}
						}
					}
				}
			}
			currentLine++;
		}

		hideAllVisibleUI();
		populateBoxes_paramVisibility();
		arrangeUI();

		return true;
	}
};
