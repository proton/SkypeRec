#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QVector>
#include <QObject>
#include <QHash>

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
	QHash<QString, AUTO_RECORD_TYPE> autorec;
	QString files_directory;
	QString files_names;
	bool files_tags;
	bool gui_notify;
	bool gui_windowed;
	bool gui_hide_legal_info;
	bool gui_first_launch;
public:
	inline const FileWriter& fileWriters(int i) const { return file_writers[i]; }
	inline int audioMp3Quality() const { return audio_mp3_bitrate; }
	inline int audioOggQuality() const { return audio_ogg_quality; }
	//
	inline AUTO_RECORD_TYPE autoRecordGlobal() const { return autorec["GLOBAL"]; }
	inline AUTO_RECORD_TYPE autoRecord(const QString& name) const
	{
		return (autorec.contains(name))?autorec[name]:autoRecordGlobal();
	}
	inline const QHash<QString, AUTO_RECORD_TYPE>& autoRecordTable() const
	{
		return autorec;
	}
	//
	QString filesDirectory() const;
	inline const QString& filesNames() const { return files_names; }
	inline bool filesTags() const { return files_tags; }
	inline bool guiNotify() const { return gui_notify; }
	inline bool guiWindowed() const { return gui_windowed; }
	inline bool guiHideLegalInfo() const { return gui_hide_legal_info; }
	inline bool guiFirstLaunch() const { return gui_first_launch; }
public slots:
	void setAudioMp3Quality(int v);
	void setAudioOggQuality(int v);
	//
	void setAutoRecord(const QString& name, int v);
	inline void setAutoRecordGlobal(int v)
	{
		setAutoRecord("GLOBAL", v);
	}
	void removeAutoRecord(const QString& name);
	//
	void setFilesDirectory(const QString&);
	void setFilesNames(const QString&);
	void setFilesTags(bool v);
	void setGuiNotify(bool v);
	void setGuiWindowed(bool v);
	void setGuiHideLegalInfo(bool v);
	void setGuiFirstLaunch(bool v);
};

extern Settings settings;

#endif // SETTINGS_H
