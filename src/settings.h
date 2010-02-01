#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>

#include "common.h"

enum AUDIO_FORMAT
{
	AUDIO_FORMAT_WAV,
	AUDIO_FORMAT_MP3,
	AUDIO_FORMAT_OGG
};

enum FILE_WRITER_ID
{
	FILE_WRITER_IN,
	FILE_WRITER_OUT,
	FILE_WRITER_2CH,
	FILE_WRITER_ALL,
	FILE_WRITER_COUNT
};

struct FileWriter
{
	AUDIO_FORMAT format;
	QString postfix;
	bool enabled;
};

class Settings
{
public:
	Settings();
	DISABLE_COPY_AND_ASSIGNMENT(Settings);
private:
	QSettings pref;
	QVariant getValue(QString, const QVariant) const;
	void load();
	//
	QVector<FileWriter> file_writers;
	int audio_mp3_bitrate;
	int audio_ogg_quality;
	bool autorec_enabled;
	bool autorec_ask;
	QString files_directory;
	QString files_names;
	bool files_tags;
	bool gui_notify;
	bool gui_windowed;
	bool gui_hide_legal_info;
public:
	inline FileWriter fileWriters(int i) const { return file_writers[i]; }
	inline int audioMp3Quality() const { return audio_mp3_bitrate; }
	inline int audioOggQuality() const { return audio_ogg_quality; }
	inline bool autorecEnabled() const { return autorec_enabled; }
	inline bool autorecAsk() const { return autorec_ask; }
	inline QString filesDiectory() const { return files_directory; }
	inline QString filesNames() const { return files_names; }
	inline bool filesTags() const { return files_tags; }
	inline bool guiNotify() const { return gui_notify; }
	inline bool guiWindowed() const { return gui_windowed; }
	inline bool guiHideLegalInfo() const { return gui_hide_legal_info; }
};

#endif // SETTINGS_H

//	X(OutputFormat,                output.format)
//	X(OutputFormatMp3Bitrate,      output.format.mp3.bitrate)
//	X(OutputFormatVorbisQuality,   output.format.vorbis.quality)
//	X(OutputStereo,                output.stereo)
//	X(OutputStereoMix,             output.stereo.mix)
//	X(SuppressLegalInformation,    suppress.legalinformation)
//	X(SuppressFirstRunInformation, suppress.firstruninformation)

//	X(OutputSaveTags,              output.savetags)
//	X(OutputPattern,               output.pattern)
//	X(AutoRecordDefault,           autorecord.default)
//	X(AutoRecordAsk,               autorecord.ask)
//	X(AutoRecordYes,               autorecord.yes)
//	X(AutoRecordNo,                autorecord.no)
//	X(OutputPath,                  output.path)
//	X(PreferencesVersion,          preferences.version)
//	X(NotifyRecordingStart,        notify.recordingstart)
//	X(GuiWindowed,                 gui.windowed)
//	X(DebugWriteSyncFile,          debug.writesyncfile)
