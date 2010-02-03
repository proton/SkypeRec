#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>
#include <QObject>

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

enum AUTO_RECORD_TYPE
{
	AUTO_RECORD_ON,
	AUTO_RECORD_ASK,
	AUTO_RECORD_OFF
};

struct FileWriter
{
	AUDIO_FORMAT format;
	QString postfix;
	bool enabled;
};

struct AutoRecord
{
	AUTO_RECORD_TYPE format;
	QString name;
};

class Settings : public QObject
{
	Q_OBJECT
public:
	Settings();
	DISABLE_COPY_AND_ASSIGNMENT(Settings);
private:
	QSettings pref;
	QVariant getValue(const QString& name, const QVariant& default_value);
	void load();
	//
	QVector<FileWriter> file_writers;
	int audio_mp3_bitrate;
	int audio_ogg_quality;
	AUTO_RECORD_TYPE autorec_global;
	QVector<AutoRecord> autorec_local;
//	bool autorec_enabled;
//	bool autorec_ask;
	QString files_directory;
	QString files_names;
	bool files_tags;
	bool gui_notify;
	bool gui_windowed;
	bool gui_hide_legal_info;
	bool gui_first_launch;
public:
	inline FileWriter fileWriters(int i) const { return file_writers[i]; }
	inline int audioMp3Quality() const { return audio_mp3_bitrate; }
	inline int audioOggQuality() const { return audio_ogg_quality; }
	inline AUTO_RECORD_TYPE autoRecord() const { return autorec_global; }
	inline AutoRecord autoRecord(int i) const { return autorec_local[i]; }
	QString filesDirectory() const;
	inline QString filesNames() const { return files_names; }
	inline bool filesTags() const { return files_tags; }
	inline bool guiNotify() const { return gui_notify; }
	inline bool guiWindowed() const { return gui_windowed; }
	inline bool guiHideLegalInfo() const { return gui_hide_legal_info; }
	inline bool guiFirstLaunch() const { return gui_first_launch; }
public slots:
	void setAutoRecord(int v);
	void setGuiNotify(bool v);
	void setGuiWindowed(bool v);
	void setGuiFirstLaunch(bool v);
};

extern Settings settings;

#endif // SETTINGS_H
