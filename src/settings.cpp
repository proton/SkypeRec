#include "settings.h"

#include <QDir>

Settings settings;

Settings::Settings()
	: pref("pDev", "SkypeRec"), file_writers(FILE_WRITER_COUNT)
{
	load();
}

inline QString default_postfix(FILE_WRITER_ID id)
{
	switch(id)
	{
		case FILE_WRITER_IN: return QString("in");
		case FILE_WRITER_OUT: return QString("out");
		case FILE_WRITER_2CH: return QString("2ch");
		case FILE_WRITER_ALL: return QString("all");
		default: return QString();
	}
}

QVariant Settings::getValue(const QString& name, const QVariant& default_value)
{
	QVariant value(default_value);
	if(pref.contains(name)) value = pref.value(name);
	else pref.setValue(name, default_value);
	return value;
}

void Settings::load()
{
	for(int i=0; i<file_writers.count(); ++i)
	{
		file_writers[i].enabled = getValue(QString("FileWriters/Writer%1_Enabled").arg(i), true).toBool();
		file_writers[i].format = AUDIO_FORMAT(getValue(QString("FileWriters/Writer%1_Format").arg(i), AUDIO_FORMAT_MP3).toInt());
		file_writers[i].postfix = getValue(QString("FileWriters/Writer%1_Postfix").arg(i), default_postfix(FILE_WRITER_ID(i))).toString();
	}
	audio_mp3_bitrate = getValue("Audio/MP3_Quality", 64).toInt();
	audio_ogg_quality = getValue("Audio/OGG_Quality", 3).toInt();
	//
	pref.beginGroup("AutoRecord");
	QStringList autorec_keys = pref.childKeys();
	for(int i=0; i<autorec_keys.size(); ++i)
	{
		autorec[autorec_keys[i]] = AUTO_RECORD_TYPE(pref.value(autorec_keys[i]).toInt());
	}
	if(!autorec.contains("GLOBAL")) autorec["GLOBAL"] = AUTO_RECORD_TYPE(getValue("GLOBAL", AUTO_RECORD_ASK).toInt());
	pref.endGroup();
	//
	files_directory = getValue("Files/Directory", "~/Skype Calls").toString();
	files_names = getValue("Files/NameFormat", "Calls with &s/Call with &s, %Y-%m-%d, %H:%M:%S - %P").toString();
	files_tags = getValue("Files/Tags", true).toBool();
	//
	gui_notify = getValue("Gui/Notify", true).toBool();
	gui_windowed = getValue("Gui/Windowed", false).toBool();
	gui_hide_legal_info = getValue("Gui/HideLegalInfo", false).toBool();
	gui_first_launch = getValue("Gui/FirstLaunch", true).toBool();
}

QString Settings::filesDirectory() const
{
	QString path(files_directory);
	if (path.startsWith("~/") || path == "~") path.replace(0, 1, QDir::homePath());
	else if (!path.startsWith('/')) path.prepend(QDir::currentPath() + '/');
	return path;
}

//------------------------------------------------------------------------------

void Settings::setAudioMp3Quality(int v)
{
	audio_mp3_bitrate = v;
	pref.setValue("Audio/MP3_Quality", audio_mp3_bitrate);
}

void Settings::setAudioOggQuality(int v)
{
	audio_ogg_quality = v;
	pref.setValue("Audio/OGG_Quality", audio_ogg_quality);
}

void Settings::setAutoRecord(const QString& name, int v)
{
	autorec[name] = AUTO_RECORD_TYPE(v);
	pref.beginGroup("AutoRecord");
	pref.setValue(name, autorec[name]);
	pref.endGroup();
}

void Settings::removeAutoRecord(const QString& name)
{
	autorec.remove(name);
	pref.beginGroup("AutoRecord");
	pref.remove(name);
	pref.endGroup();
}

void Settings::setFileWriterState(int writer_id, bool v)
{
	file_writers[writer_id].enabled = v;
	pref.setValue(QString("FileWriters/Writer%1_Enabled").arg(writer_id), v);
}

void Settings::setFileWriterFormat(int writer_id, int v)
{
	file_writers[writer_id].format = AUDIO_FORMAT(v);
	pref.setValue(QString("FileWriters/Writer%1_Format").arg(writer_id), v);
}

void Settings::setFileWriterPostfix(int writer_id, const QString& v)
{
	file_writers[writer_id].postfix = v;
	pref.setValue(QString("FileWriters/Writer%1_Postfix").arg(writer_id), v);
}

void Settings::setFilesDirectory(const QString& v)
{
	files_directory = v;
	pref.setValue("Files/Directory", files_directory);
}

void Settings::setFilesNames(const QString& v)
{
	files_names = v;
	pref.setValue("Files/NameFormat", files_names);
}

void Settings::setFilesTags(bool v)
{
	files_tags = v;
	pref.setValue("Files/Tags", files_tags);
}

void Settings::setGuiNotify(bool v)
{
	gui_notify = v;
	pref.setValue("Gui/Notify", gui_notify);
}

void Settings::setGuiWindowed(bool v)
{
	gui_windowed = v;
	pref.setValue("Gui/Windowed", gui_windowed);
}

void Settings::setShowDebug(bool v)
{
	show_debug = v;
	pref.setValue("Common/ShowDebug", show_debug);
}

void Settings::setGuiHideLegalInfo(bool v)
{
	gui_hide_legal_info = v;
	pref.setValue("Gui/HideLegalInfo", gui_hide_legal_info);
}

void Settings::setGuiFirstLaunch(bool v)
{
	gui_first_launch = v;
	pref.setValue("Gui/FirstLaunch", gui_first_launch);
}
