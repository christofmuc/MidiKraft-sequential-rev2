/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "JuceHeader.h"

#include "DSI.h"
#include "LayerCapability.h"
#include "DataFileLoadCapability.h"
#include "DataFileSendCapability.h"

namespace midikraft {

	class Rev2 : public DSISynth, public LayerCapability, public DataFileLoadCapability, public DataFileSendCapability, public std::enable_shared_from_this<Rev2>
	{
	public:
		// Data Item Types, for storage in the database!
		enum DataType {
			PATCH = 0, 
			GLOBAL_SETTINGS = 1,
			ALTERNATE_TUNING = 2
		};
		enum Rev2StreamType {
			PATCH_STREAM = 3
		};

		Rev2();

		// Basic Synth
		virtual std::string getName() const override;
		virtual std::shared_ptr<DataFile> patchFromPatchData(const Synth::PatchData &data, MidiProgramNumber place) const override;
		virtual int numberOfBanks() const override;
		virtual int numberOfPatches() const override;
		virtual std::string friendlyProgramName(MidiProgramNumber programNo) const override;
		virtual std::string friendlyBankName(MidiBankNumber bankNo) const override;

		// Edit Buffer Capability
		virtual std::shared_ptr<DataFile> patchFromSysex(const MidiMessage& message) const override;
		virtual std::vector<MidiMessage> patchToSysex(std::shared_ptr<DataFile> patch) const override;

		// Program Dump Capability
		virtual std::shared_ptr<DataFile> patchFromProgramDumpSysex(const MidiMessage& message) const override;
		virtual std::vector<MidiMessage> patchToProgramDumpSysex(std::shared_ptr<DataFile> patch, MidiProgramNumber programNumber) const override;

		MidiMessage patchPolySequenceToGatedTrack(const MidiMessage& message, int gatedSeqTrack);
		MidiMessage clearPolySequencer(const MidiMessage &programEditBuffer, bool layerA, bool layerB);
		MidiMessage copySequencersFromOther(const MidiMessage& currentProgram, const MidiMessage &lockedProgram);

		// LayerCapability
		virtual void switchToLayer(int layerNo) override;
		virtual std::vector<MidiMessage> layerToSysex(std::shared_ptr<DataFile> const patch, int sourceLayer, int targetLayer) const override;

		// SoundExpanderCapability
		virtual void changeInputChannel(MidiController *controller, MidiChannel channel, std::function<void()> onFinished) override;
		virtual void setMidiControl(MidiController *controller, bool isOn) override;

		// KeyboardCapability
		MidiNote getLowestKey() const override;
		MidiNote getHighestKey() const override;

		// MasterkeyboardCapability
		virtual void changeOutputChannel(MidiController *controller, MidiChannel channel, std::function<void()> onFinished) override;
		virtual void setLocalControl(MidiController *controller, bool localControlOn) override;

		virtual PatchData filterVoiceRelevantData(std::shared_ptr<DataFile> unfilteredData) const override;

		// DataFileLoadCapability - this is used for loading the GlobalSettings from the synth for the property editor
		virtual std::vector<DataFileImportDescription> dataFileImportChoices() const override;
		virtual int numberOfMidiMessagesPerStreamType(DataStreamType dataTypeID) const override;
		std::vector<MidiMessage> requestDataItem(int itemNo, DataStreamType dataTypeID) override;
		bool isPartOfDataFileStream(const MidiMessage &message, DataStreamType dataTypeID) const override;
		bool isDataFile(const MidiMessage &message, DataFileType dataTypeID) const override;
		virtual bool isStreamComplete(std::vector<MidiMessage> const &messages, DataStreamType streamType) const override;
		virtual bool shouldStreamAdvance(std::vector<MidiMessage> const &messages, DataStreamType streamType) const override;
		std::vector<std::shared_ptr<DataFile>> loadData(std::vector<MidiMessage> messages, DataStreamType dataTypeID) const override;
		std::vector<DataFileDescription> dataTypeNames() const override;

		// DataFileSendCapability
		std::vector<MidiMessage> dataFileToMessages(std::shared_ptr<DataFile> dataFile, std::shared_ptr<SendTarget> target) const override;

		//TODO These should go into the DSISynth class
		virtual std::shared_ptr<DataFileLoadCapability> loader() override;
		virtual int settingsDataFileType() const override;
		virtual DataFileLoadCapability::DataFileImportDescription settingsImport() const override;

		// Implement generic DSISynth global settings capability
		virtual std::vector<DSIGlobalSettingDefinition> dsiGlobalSettings() const override;

	private:
		MidiMessage buildSysexFromEditBuffer(std::vector<uint8> editBuffer);
		MidiMessage filterProgramEditBuffer(const MidiMessage &programEditBuffer, std::function<void(std::vector<uint8> &)> filterExpressionInPlace);

		void initGlobalSettings();

		// That's not very Rev2 specific
		static uint8 clamp(int value, uint8 min = 0, uint8 max = 127);
	};

}
