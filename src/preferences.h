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

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>
#include <QList>
#include <QString>
#include <QStringList>
#include <QAbstractListModel>
#include <QPointer>

#include "settings.h"

class QWidget;
class QListView;
class PerCallerModel;
class PerCallerPreferencesDialog;
class QRadioButton;
class QComboBox;
class QLineEdit;
class QDateTime;
class QLabel;

// The preferences dialog

class PreferencesDialog : public QDialog
{
	Q_OBJECT
public:
	PreferencesDialog();
	void closePerCallerDialog();

protected:
	void hideEvent(QHideEvent *);

private slots:
	void editPerCallerPreferences();
	void updatePatternToolTip(const QString &);
	void browseOutputPath();
	void updateAbsolutePathWarning(const QString &);
	//
	void setMp3Quality(int index);
	void setOggQuality(int index);

private:
	QWidget* createRecordingTab(QWidget* parent);
	QWidget* createPathTab(QWidget* parent);
	QWidget* createFormatTab(QWidget* parent);
	QWidget* createMiscTab(QWidget* parent);

private:
	QLabel* absolutePathWarningLabel;
	QComboBox* patternWidget;
	QLineEdit* filesPathEdit;
	QComboBox* mp3QualityWidget;
	QComboBox* oggQualityWidget;
	//
	QPointer<PerCallerPreferencesDialog> perCallerDialog;

	DISABLE_COPY_AND_ASSIGNMENT(PreferencesDialog);
};

// The per caller editor dialog

class PerCallerPreferencesDialog : public QDialog
{
	Q_OBJECT
public:
	PerCallerPreferencesDialog(QWidget* );

private slots:
	void add(const QString & = QString(), int = 1, bool = true);
	void remove();
	void selectionChanged();
	void radioChanged();
	void save();

private:
	QListView *listWidget;
	PerCallerModel *model;
	QRadioButton *radioYes;
	QRadioButton *radioAsk;
	QRadioButton *radioNo;

	DISABLE_COPY_AND_ASSIGNMENT(PerCallerPreferencesDialog);
};

// per caller model

class PerCallerModel : public QAbstractListModel
{
	Q_OBJECT
public:
	PerCallerModel(QObject *parent) : QAbstractListModel(parent) {}
	void load();
	int rowCount(const QModelIndex & = QModelIndex()) const;
	QVariant data(const QModelIndex &, int) const;
	bool setData(const QModelIndex &, const QVariant &, int = Qt::EditRole);
	bool insertRows(int, int, const QModelIndex &);
	bool removeRows(int, int, const QModelIndex &);
	void sort(int = 0, Qt::SortOrder = Qt::AscendingOrder);
	Qt::ItemFlags flags(const QModelIndex &) const;

private:
	QList<QPair<QString,AUTO_RECORD_TYPE> > autorec_list;
};

// the only instance of Preferences

extern Settings settings;

extern QString getFileName(const QString &, const QString &, const QString &,
	const QString &, const QDateTime &, const QString & = QString());

#undef X

#endif

