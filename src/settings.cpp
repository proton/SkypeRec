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
	autorec_enabled = getValue("AutoRecord/Enable", true).toBool();
	autorec_ask = getValue("AutoRecord/Ask", true).toBool();
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

void Settings::setGuiWindowed(bool v)
{
	gui_windowed = v;
	pref.setValue("Gui/Windowed", gui_windowed);
}

void Settings::setGuiFirstLaunch(bool v)
{
	gui_first_launch = v;
	pref.setValue("Gui/FirstLaunch", gui_first_launch);
}
