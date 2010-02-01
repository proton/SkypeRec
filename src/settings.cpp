#include "settings.h"

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
	}
}

QVariant Settings::getValue(QString name, const QVariant default_value) const
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
		file_writers[i].format = getValue(QString("FileWriters/Writer%1_Format").arg(i), AUDIO_FORMAT_MP3).toInt();
		file_writers[i].postfix = getValue(QString("FileWriters/Writer%1_Postfix").arg(i), ).toString();
	}
	audio_mp3_bitrate = getValue("Audio/MP3_Quality", 64);
	audio_ogg_quality = getValue("Audio/OGG_Quality", 3);
	//
	autorec_enabled = getValue("AutoRecord/Enable", true);
	autorec_ask = getValue("AutoRecord/Ask", true);
	//
	files_directory = getValue("Files/Directory", "~/Skype Calls");
	files_names = getValue("Files/NameFormat", "Calls with &s/Call with &s, %Y-%m-%d, %H:%M:%S - %P");
	files_tags = getValue("Files/Tags", true);
	//
	gui_notify = getValue("Gui/Notify", true);
	gui_windowed = getValue("Gui/Windowed", false);
	gui_hide_legal_info = getValue("Gui/HideLegalInfo", false);
}
