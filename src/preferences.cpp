/*
	SkypeRec
	Copyright 2008 - 2009 by jlh jlh at gmx dot ch)
	Copyright 2010 by Peter Savichev  (proton) <psavichev@gmail.com>

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License as published by the
	Free Software Foundation; either version 2 of the License, version 3 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	General Public License for more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc.,
	51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

	The GNU General Public License version 2 is included with the source of
	this program under the file name COPYING.  You can also get a copy on
	http://www.fsf.org/
*/

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QListView>
#include <QPair>
#include <QFile>
#include <QSet>
#include <QTextStream>
#include <QtAlgorithms>
#include <QDateTime>
#include <QList>
#include <QFileDialog>
#include <QTabWidget>
#include <ctime>
#include <QSignalMapper>

#include "preferences.h"
#include "common.h"
#include "recorder.h"

QString escape(const QString &s)
{
	QString out(s);
	out.replace('%', "%%");
	out.replace('/', '_');
	return out;
}

QString getFileName(const QString &skypeName, const QString &displayName,
	const QString &mySkypeName, const QString &myDisplayName, const QDateTime &timestamp, const QString &pat)
{
	QString pattern = pat.isEmpty() ? settings.filesNames() : pat;
	QString fileName;

	for (int i = 0; i < pattern.size(); ++i)
	{
		if (pattern.at(i) == QChar('&') && i + 1 < pattern.size())
		{
			++i;
			if (pattern.at(i) == QChar('s')) fileName += escape(skypeName);
			else if (pattern.at(i) == QChar('d')) fileName += escape(displayName);
			else if (pattern.at(i) == QChar('t')) fileName += escape(mySkypeName);
			else if (pattern.at(i) == QChar('e')) fileName += escape(myDisplayName);
			else if (pattern.at(i) == QChar('&')) fileName += QChar('&');
			else
			{
				fileName += QChar('&');
				fileName += pattern.at(i);
			}
		}
		else fileName += pattern.at(i);
	}

	// TODO: uhm, does QT provide any time formatting the strftime() way?
	char *buf = new char[fileName.size() + 1024];
	time_t t = timestamp.toTime_t();
	struct tm *tm = std::localtime(&t);
	std::strftime(buf, fileName.size() + 1024, fileName.toUtf8().constData(), tm);
	fileName = QString::fromLocal8Bit(buf);
	delete[] buf;

	return settings.filesDirectory() + '/' + fileName;
}

// preferences dialog

static QVBoxLayout* makeVFrame(QVBoxLayout* parentLayout, const QString& title)
{
	QGroupBox *box = new QGroupBox(title);
	QVBoxLayout* vbox = new QVBoxLayout(box);
	parentLayout->addWidget(box);
	return vbox;
}

QWidget* PreferencesDialog::createRecordingTab(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QVBoxLayout* vbox = new QVBoxLayout(widget);
	//
	QButtonGroup* group = new QButtonGroup(vbox);
	QRadioButton* radio = new QRadioButton(tr("Automatically &record all calls"), widget);
	group->addButton(radio, AUTO_RECORD_ON);
	vbox->addWidget(radio);
	radio = new QRadioButton(tr("&Ask for every call"), widget);
	group->addButton(radio, AUTO_RECORD_ASK);
	vbox->addWidget(radio);
	radio = new QRadioButton(tr("Do &not automatically record calls"), widget);
	group->addButton(radio, AUTO_RECORD_OFF);
	vbox->addWidget(radio);
	//
	int auto_rec = settings.autoRecordGlobal();
	group->button(auto_rec)->setChecked(true);
	//
	connect(group, SIGNAL(buttonClicked(int)), &settings, SLOT(setAutoRecord(int)));

	QPushButton* button = new QPushButton(tr("Edit &per caller preferences"), widget);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(editPerCallerPreferences()));
	vbox->addWidget(button);

	QCheckBox* check = new QCheckBox(tr("Show &balloon notification when recording starts"), widget);
	check->setChecked(settings.guiNotify());
	connect(button, SIGNAL(toggled(bool)), &settings, SLOT(setGuiNotify(bool)));
	vbox->addWidget(check);

	vbox->addStretch();
	return widget;
}

QWidget* PreferencesDialog::createPathTab(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QVBoxLayout* vbox = new QVBoxLayout(widget);
	//
	QLabel* label = new QLabel(tr("&Save recorded calls here:"), widget);
	filesPathEdit = new QLineEdit(settings.filesDirectory(), widget);
	label->setBuddy(filesPathEdit);
	//
	connect(filesPathEdit, SIGNAL(textChanged(const QString &)), this, SLOT(updateAbsolutePathWarning(const QString &)));
	//
	QPushButton* button = new QPushButton("...", widget);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(browseOutputPath()));
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addWidget(filesPathEdit);
	hbox->addWidget(button);
	vbox->addWidget(label);
	vbox->addLayout(hbox);
	//
	label = new QLabel(tr("File &name:"));
	patternWidget = new QComboBox(widget);
	patternWidget->setEditable(true);
	patternWidget->setEditText(settings.filesNames());
	label->setBuddy(patternWidget);
	patternWidget->addItem(tr("%Y-%m-%d %H:%M:%S Call with &s - %P"));
	patternWidget->addItem(tr("Call with &s, %a %b %d %Y, %H:%M:%S - %P"));
	patternWidget->addItem(tr("%Y, %B/Call with &s, %a %b %d %Y, %H:%M:%S - %P"));
	patternWidget->addItem(tr("Calls with &s/Call with &s, %a %b %d %Y, %H:%M:%S - %P"));
	connect(patternWidget, SIGNAL(editTextChanged(const QString &)), this, SLOT(updatePatternToolTip(const QString &)));
	connect(patternWidget, SIGNAL(editTextChanged(const QString &)), &settings, SLOT(setFilesNames(const QString &)));
	vbox->addWidget(label);
	vbox->addWidget(patternWidget);
	//
	vbox->addStretch();
	//
	absolutePathWarningLabel = new QLabel(tr("<b>Warning:</b> The path you have entered is not an absolute path!"), widget);
	vbox->addWidget(absolutePathWarningLabel);
	//
	updatePatternToolTip("");
	updateAbsolutePathWarning(settings.filesDirectory());
	//
	return widget;
}

QWidget* PreferencesDialog::createFormatTab(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QVBoxLayout* vbox = new QVBoxLayout(widget);
	//
	QGridLayout* grid = new QGridLayout();
	//
	grid->addWidget(new QLabel(tr("Name"), widget), 0, 0);
	grid->addWidget(new QLabel(tr("Format"), widget), 0, 1);
	grid->addWidget(new QLabel(tr("Postfix"), widget), 0, 2);
	QCheckBox* check;
	QComboBox *combo;
	QLineEdit *edit;
	for(int i=0; i<FILE_WRITER_COUNT; ++i)
	{
		const FileWriter& fw = settings.fileWriters(i);
		//
		check = new QCheckBox(writerTitle(FILE_WRITER_ID(i)), widget);
		check->setChecked(fw.enabled);
		connect(check, SIGNAL(toggled(bool)), sm_writer_state, SLOT(map()));
		sm_writer_state->setMapping(check, i);
		writerStateWidgets.append(check);
		grid->addWidget(check, i+1, 0);
		//
		combo = new QComboBox(widget);
		for(int n=0; n<AUDIO_FORMAT_COUNT; ++n)
		{
			combo->addItem(audioFormatTitle(AUDIO_FORMAT(n)), n);
		}
		int n = combo->findData(fw.format);
		if(n>=0) combo->setCurrentIndex(n);
		connect(combo, SIGNAL(currentIndexChanged(int)), sm_writer_format, SLOT(map()));
		sm_writer_format->setMapping(combo, i);
		writerFormatWidgets.append(combo);
		grid->addWidget(combo, i+1, 1);
		//
		edit = new QLineEdit(fw.postfix, widget);
		connect(edit, SIGNAL(editingFinished()), sm_writer_postfix, SLOT(map()));
		sm_writer_postfix->setMapping(edit, i);
		writerPostfixWidgets.append(edit);
		grid->addWidget(edit, i+1, 2);
	}
	connect(sm_writer_state, SIGNAL(mapped(int)), this, SLOT(setFileWriterState(int)));
	connect(sm_writer_format, SIGNAL(mapped(int)), this, SLOT(setFileWriterFormat(int)));
	connect(sm_writer_postfix, SIGNAL(mapped(int)), this, SLOT(setFileWriterPostfix(int)));
	//
	vbox->addLayout(grid);
	//
	grid = new QGridLayout();
	//
	QLabel* label = new QLabel(tr("MP3 &bitrate:"), widget);
	mp3QualityWidget = new QComboBox(widget);
	label->setBuddy(mp3QualityWidget);
	mp3QualityWidget->addItem(tr("8 kbps"), 8);
	mp3QualityWidget->addItem(tr("16 kbps"), 16);
	mp3QualityWidget->addItem(tr("24 kbps"), 24);
	mp3QualityWidget->addItem(tr("32 kbps (recommended for mono)"), 32);
	mp3QualityWidget->addItem(tr("40 kbps"), 40);
	mp3QualityWidget->addItem(tr("48 kbps"), 48);
	mp3QualityWidget->addItem(tr("56 kbps"), 56);
	mp3QualityWidget->addItem(tr("64 kbps (recommended for stereo)"), 64);
	mp3QualityWidget->addItem(tr("80 kbps"), 80);
	mp3QualityWidget->addItem(tr("96 kbps"), 96);
	mp3QualityWidget->addItem(tr("112 kbps"), 112);
	mp3QualityWidget->addItem(tr("128 kbps"), 128);
	mp3QualityWidget->addItem(tr("144 kbps"), 144);
	mp3QualityWidget->addItem(tr("160 kbps"), 160);
	int i = mp3QualityWidget->findData(settings.audioMp3Quality());
	if(i>=0) mp3QualityWidget->setCurrentIndex(i);
	connect(mp3QualityWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(setMp3Quality(int)));
	grid->addWidget(label, 0, 0);
	grid->addWidget(mp3QualityWidget, 0, 1);
	//
	label = new QLabel(tr("Ogg Vorbis &quality:"));
	oggQualityWidget = new QComboBox(widget);
	label->setBuddy(oggQualityWidget);
	oggQualityWidget->addItem(tr("Quality -1"), -1);
	oggQualityWidget->addItem(tr("Quality 0"), 0);
	oggQualityWidget->addItem(tr("Quality 1"), 1);
	oggQualityWidget->addItem(tr("Quality 2"), 2);
	oggQualityWidget->addItem(tr("Quality 3 (recommended)"), 3);
	oggQualityWidget->addItem(tr("Quality 4"), 4);
	oggQualityWidget->addItem(tr("Quality 5"), 5);
	oggQualityWidget->addItem(tr("Quality 6"), 6);
	oggQualityWidget->addItem(tr("Quality 7"), 7);
	oggQualityWidget->addItem(tr("Quality 8"), 8);
	oggQualityWidget->addItem(tr("Quality 9"), 9);
	oggQualityWidget->addItem(tr("Quality 10"), 10);
	i = oggQualityWidget->findData(settings.audioOggQuality());
	if(i>=0) oggQualityWidget->setCurrentIndex(i);
	connect(oggQualityWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(setOggQuality(int)));
	grid->addWidget(label, 1, 0);
	grid->addWidget(oggQualityWidget, 1, 1);
	//
	vbox->addLayout(grid);
	//
	check = new QCheckBox(tr("Save call &information in files"), widget);
	check->setChecked(settings.filesTags());
	connect(check, SIGNAL(toggled(bool)), &settings, SLOT(setFilesTags(bool)));
	vbox->addWidget(check);
	//
	vbox->addStretch();
	return widget;
}

QWidget* PreferencesDialog::createMiscTab(QWidget* parent)
{
	QWidget* widget = new QWidget(parent);
	QVBoxLayout* vbox = new QVBoxLayout(widget);

	QCheckBox *check = new QCheckBox(tr("&Display a small main window.  Enable this if your\n"
		"environment does not provide a system tray (needs restart)"), widget);
	connect(check, SIGNAL(toggled(bool)), &settings, SLOT(setGuiWindowed(bool)));
	vbox->addWidget(check);

	vbox->addStretch();
	return widget;
}

PreferencesDialog::PreferencesDialog()
{
	setWindowTitle(tr("%1 - Preferences").arg(PROGRAM_NAME));
	setAttribute(Qt::WA_DeleteOnClose);

	sm_writer_state = new QSignalMapper(this);
	sm_writer_format = new QSignalMapper(this);
	sm_writer_postfix = new QSignalMapper(this);

	QVBoxLayout* vbox = new QVBoxLayout(this);
	vbox->setSizeConstraint(QLayout::SetFixedSize);

	QTabWidget *tabWidget = new QTabWidget;
	vbox->addWidget(tabWidget);

	tabWidget->addTab(createRecordingTab(tabWidget), tr("Au&tomatic Recording"));
	tabWidget->addTab(createPathTab(tabWidget), tr("&File paths"));
	tabWidget->addTab(createFormatTab(tabWidget), tr("W&riters"));
	tabWidget->addTab(createMiscTab(tabWidget), tr("&Misc"));
	tabWidget->setUsesScrollButtons(false);

	QHBoxLayout *hbox = new QHBoxLayout;
	QPushButton* button = new QPushButton(tr("&Close"));
	button->setDefault(true);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(accept()));
	hbox->addStretch();
	hbox->addWidget(button);
	vbox->addLayout(hbox);

	show();
}

void PreferencesDialog::setFileWriterState(int writer_id)
{
	bool state = writerStateWidgets[writer_id]->isChecked();
	settings.setFileWriterState(writer_id, state);
}

void PreferencesDialog::setFileWriterFormat(int writer_id)
{
	int format = writerFormatWidgets[writer_id]->currentIndex();
	settings.setFileWriterFormat(writer_id, format);
}

void PreferencesDialog::setFileWriterPostfix(int writer_id)
{
	const QString& postfix = writerPostfixWidgets[writer_id]->text();
	settings.setFileWriterPostfix(writer_id, postfix);
}

void PreferencesDialog::setMp3Quality(int index)
{
	settings.setAudioMp3Quality(mp3QualityWidget->itemData(index).toInt());
}

void PreferencesDialog::setOggQuality(int index)
{
	settings.setAudioOggQuality(oggQualityWidget->itemData(index).toInt());
}

void PreferencesDialog::editPerCallerPreferences()
{
	perCallerDialog = new PerCallerPreferencesDialog(this);
}

void PreferencesDialog::browseOutputPath()
{
	QFileDialog dialog(this, tr("Select output path"), settings.filesDirectory());
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	if (!dialog.exec()) return;
	//
	QStringList list = dialog.selectedFiles();
	if (!list.size()) return;
	//
	QString path = list.at(0);
	QString home = QDir::homePath();
	if (path.startsWith(home + '/') || path == home)
		path.replace(0, home.size(), '~');
	if (path.endsWith('/') || path.endsWith('\\'))
		path.chop(1);
	filesPathEdit->setText(path);
}

void PreferencesDialog::updateAbsolutePathWarning(const QString &string)
{
	if (string.startsWith('/') || string.startsWith("~/") || string == "~")
	{
		absolutePathWarningLabel->hide();
		settings.setFilesDirectory(string);
	}
	else absolutePathWarningLabel->show();
}

void PreferencesDialog::hideEvent(QHideEvent *event)
{
	if (perCallerDialog)
		perCallerDialog->accept();

	QDialog::hideEvent(event);
}

void PreferencesDialog::updatePatternToolTip(const QString &pattern)
{
	QString tip =
	"This pattern specifies how the file name for the recorded call is constructed.\n"
	"You can use the following directives:\n\n"

	#define X(a, b) "\t" a "\t" b "\n"
	X("&s"     , "The remote skype name or phone number")
	X("&d"     , "The remote display name")
	X("&t"     , "Your skype name")
	X("&e"     , "Your display name")
	X("&&"     , "Literal & character")
	X("%Y"     , "Year")
	X("%A / %a", "Full / abbreviated weekday name")
	X("%B / %b", "Full / abbreviated month name")
	X("%m"     , "Month as a number (01 - 12)")
	X("%d"     , "Day of the month (01 - 31)")
	X("%H"     , "Hour as a 24-hour clock (00 - 23)")
	X("%I"     , "Hour as a 12-hour clock (01 - 12)")
	X("%p"     , "AM or PM")
	X("%M"     , "Minutes (00 - 59)")
	X("%S"     , "Seconds (00 - 59)")
	X("%%"     , "Literal % character")
	#undef X
	"\t...and all other directives provided by strftime()\n\n"

	"With the current choice, the file name might look like this:\n";

	QString fn = getFileName("echo123", "Skype Test Service", "myskype", "My Full Name",
		QDateTime::currentDateTime(), pattern);
	tip += fn;
	if (fn.contains(':'))
		tip += tr("\n\nWARNING: Microsoft Windows does not allow colon characters (:) in file names.");
//	patternWidget->setToolTip(tip);
}

void PreferencesDialog::closePerCallerDialog() {
	if (perCallerDialog)
		perCallerDialog->accept();
}

// per caller preferences editor

PerCallerPreferencesDialog::PerCallerPreferencesDialog(QWidget* parent) : QDialog(parent)
{
	setWindowTitle("Per Caller Preferences");
	setWindowModality(Qt::WindowModal);
	setAttribute(Qt::WA_DeleteOnClose);

	model = new PerCallerModel(this);

	QHBoxLayout *bighbox = new QHBoxLayout(this);
	QVBoxLayout* vbox = new QVBoxLayout;

	listWidget = new QListView;
	listWidget->setModel(model);
	listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	listWidget->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);
	connect(listWidget->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged()));
	vbox->addWidget(listWidget);

	QVBoxLayout* frame = makeVFrame(vbox, tr("Preference for selected Skype names:"));
	radioYes = new QRadioButton(tr("Automatically &record calls"));
	radioAsk = new QRadioButton(tr("&Ask every time"));
	radioNo  = new QRadioButton(tr("Do &not automatically record calls"));
	connect(radioYes, SIGNAL(clicked(bool)), this, SLOT(radioChanged()));
	connect(radioAsk, SIGNAL(clicked(bool)), this, SLOT(radioChanged()));
	connect(radioNo,  SIGNAL(clicked(bool)), this, SLOT(radioChanged()));
	frame->addWidget(radioYes);
	frame->addWidget(radioAsk);
	frame->addWidget(radioNo);

	bighbox->addLayout(vbox);

	vbox = new QVBoxLayout;

	QPushButton* button = new QPushButton(tr("A&dd"));
	connect(button, SIGNAL(clicked(bool)), this, SLOT(add()));
	vbox->addWidget(button);

	button = new QPushButton(tr("Re&move"));
	connect(button, SIGNAL(clicked(bool)), this, SLOT(remove()));
	vbox->addWidget(button);

	vbox->addStretch();

	button = new QPushButton(tr("&Close"));
	button->setDefault(true);
	connect(button, SIGNAL(clicked(bool)), this, SLOT(accept()));
	vbox->addWidget(button);

	bighbox->addLayout(vbox);

	model->load();
	model->sort();
	selectionChanged();
	show();
}

void PerCallerPreferencesDialog::add(const QString &name, int mode, bool edit)
{
	int i = model->rowCount();
	model->insertRow(i);

	QModelIndex idx = model->index(i, 0);
	model->setData(idx, name, Qt::EditRole);
	model->setData(idx, mode, Qt::UserRole);

	if (edit)
	{
		listWidget->clearSelection();
		listWidget->setCurrentIndex(idx);
		listWidget->edit(idx);
	}
}

void PerCallerPreferencesDialog::remove()
{
	QModelIndexList sel = listWidget->selectionModel()->selectedIndexes();
	qSort(sel);
	while (!sel.isEmpty())
		model->removeRow(sel.takeLast().row());
}

void PerCallerPreferencesDialog::selectionChanged() {
	QModelIndexList sel = listWidget->selectionModel()->selectedIndexes();
	bool notEmpty = !sel.isEmpty();
	int mode = -1;
	while (!sel.isEmpty()) {
		int m = model->data(sel.takeLast(), Qt::UserRole).toInt();
		if (mode == -1) {
			mode = m;
		} else if (mode != m) {
			mode = -1;
			break;
		}
	}
	if (mode == -1) {
		// Qt is a bit annoying about this: You can't deselect
		// everything unless you disable auto-exclusive mode
		radioYes->setAutoExclusive(false);
		radioAsk->setAutoExclusive(false);
		radioNo ->setAutoExclusive(false);
		radioYes->setChecked(false);
		radioAsk->setChecked(false);
		radioNo ->setChecked(false);
		radioYes->setAutoExclusive(true);
		radioAsk->setAutoExclusive(true);
		radioNo ->setAutoExclusive(true);
	} else if (mode == 0) {
		radioNo->setChecked(true);
	} else if (mode == 1) {
		radioAsk->setChecked(true);
	} else if (mode == 2) {
		radioYes->setChecked(true);
	}

	radioYes->setEnabled(notEmpty);
	radioAsk->setEnabled(notEmpty);
	radioNo ->setEnabled(notEmpty);
}

void PerCallerPreferencesDialog::radioChanged()
{
	AUTO_RECORD_TYPE ar;
	if (radioYes->isChecked())
		ar = AUTO_RECORD_ON;
	else if (radioNo->isChecked())
		ar = AUTO_RECORD_OFF;
	else
		ar = AUTO_RECORD_ASK;
	//
	QModelIndexList sel = listWidget->selectionModel()->selectedIndexes();
	while (!sel.isEmpty()) model->setData(sel.takeLast(), ar, Qt::UserRole);
}

// per caller model

void PerCallerModel::load()
{
	const QHash<QString, AUTO_RECORD_TYPE>& hash = settings.autoRecordTable();
	for(QHash<QString, AUTO_RECORD_TYPE>::const_iterator it=hash.constBegin(); it!=hash.constEnd(); ++it)
	{
		QPair<QString,AUTO_RECORD_TYPE> ar(it.key(), it.value());
		autorec_list.append(ar);
	}
}

int PerCallerModel::rowCount(const QModelIndex &) const
{
	return autorec_list.count();
}

QVariant PerCallerModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= autorec_list.size()) return QVariant();
	const QPair<QString,AUTO_RECORD_TYPE>& ar = autorec_list.at(index.row());
	switch(role)
	{
		case Qt::DisplayRole: return QString(" - ").arg(ar.first, autoRecordTitle(ar.second));
		case Qt::EditRole: return ar.first;
		case Qt::UserRole: return ar.second;
		default: return QVariant();
	}
}

bool PerCallerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid() || index.row() >= autorec_list.size()) return false;
	QPair<QString,AUTO_RECORD_TYPE>& ar = autorec_list[index.row()];
	switch(role)
	{
		case Qt::EditRole:
		{
			settings.removeAutoRecord(ar.first);
			ar.first = value.toString();
			settings.setAutoRecord(ar.first, ar.second);
			emit dataChanged(index, index);
			return true;
		}
		case Qt::UserRole:
		{
			ar.second = AUTO_RECORD_TYPE(value.toInt());
			settings.setAutoRecord(ar.first, ar.second);
			emit dataChanged(index, index);
			return true;
		}
		default: return false;
	}
}

bool PerCallerModel::insertRows(int position, int rows, const QModelIndex &)
{
	QPair<QString,AUTO_RECORD_TYPE> ar(QString(), settings.autoRecordGlobal());
	beginInsertRows(QModelIndex(), position, position + rows - 1);
	for (int i = 0; i < rows; ++i)
	{
		autorec_list.insert(position, ar);
	}
	endInsertRows();
	return true;
}

bool PerCallerModel::removeRows(int position, int rows, const QModelIndex &)
{
	beginRemoveRows(QModelIndex(), position, position + rows - 1);
	for (int i = 0; i < rows; ++i)
	{
		QString name = autorec_list.at(i).first;
		settings.removeAutoRecord(name);
		autorec_list.removeAt(position);
	}
	endRemoveRows();
	return true;
}

void PerCallerModel::sort(int, Qt::SortOrder)
{
	qSort(autorec_list);
	reset();
}

Qt::ItemFlags PerCallerModel::flags(const QModelIndex &index) const
{
	Qt::ItemFlags flags = QAbstractListModel::flags(index);
	if (!index.isValid() || index.row() >= autorec_list.size()) return flags;
	return flags | Qt::ItemIsEditable;
}
