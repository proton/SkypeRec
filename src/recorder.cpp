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

#include <QTranslator>

#include <QMessageBox>
#include <QDir>
#include <QProcess>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>
#include <QDateTime>
#include <iostream>
#include <cstdlib>

#include <QMap>

#include "recorder.h"
#include "common.h"
#include "gui.h"
#include "trayicon.h"
#include "preferences.h"
#include "settings.h"
#include "skype.h"
#include "call.h"

Recorder::Recorder(int &argc, char **argv) :
	QApplication(argc, argv)
{
	recorderInstance = this;

	debug("Initializing application");

	// check for already running instance
	if (!lockFile.lock(QDir::homePath() + "/.skyperec.lock")) {
		debug("Other instance is running");
		QTimer::singleShot(0, this, SLOT(quit()));
		return;
	}

	setupGUI();
	setupSkype();
	setupCallHandler();
}

Recorder::~Recorder() {
	// the Recorder object must still exist while the child objects are
	// being destroyed, because debug() must still be available.  Qt would
	// do it automatically but only after Recorder ceased to exist, in the
	// QObject destructor

	delete preferencesDialog;
	delete callHandler;
	delete skype;
	delete trayIcon;
}

void Recorder::setupGUI() {
	setWindowIcon(QIcon(":/res/tray-red.png"));
	setQuitOnLastWindowClosed(false);

	trayIcon = new TrayIcon(this);
	connect(trayIcon, SIGNAL(requestQuit()),               this, SLOT(quitConfirmation()));
	connect(trayIcon, SIGNAL(requestAbout()),              this, SLOT(about()));
	connect(trayIcon, SIGNAL(requestOpenPreferences()),    this, SLOT(openPreferences()));
	connect(trayIcon, SIGNAL(requestBrowseCalls()),        this, SLOT(browseCalls()));

	debug("GUI initialized");

	if (settings.guiFirstLaunch())
	{
		new FirstRunDialog();
		settings.setGuiFirstLaunch(false);
	}
}

void Recorder::setupSkype() {
	skype = new Skype(this);
	connect(skype, SIGNAL(notify(const QString &)),           this, SLOT(skypeNotify(const QString &)));
	connect(skype, SIGNAL(connected(bool)),                   this, SLOT(skypeConnected(bool)));
	connect(skype, SIGNAL(connectionFailed(const QString &)), this, SLOT(skypeConnectionFailed(const QString &)));

	connect(skype, SIGNAL(connected(bool)), trayIcon, SLOT(setColor(bool)));
}

void Recorder::setupCallHandler() {
	callHandler = new CallHandler(this, skype);

	connect(trayIcon, SIGNAL(startRecording(int)),         callHandler, SLOT(startRecording(int)));
	connect(trayIcon, SIGNAL(stopRecording(int)),          callHandler, SLOT(stopRecording(int)));
	connect(trayIcon, SIGNAL(stopRecordingAndDelete(int)), callHandler, SLOT(stopRecordingAndDelete(int)));

	connect(callHandler, SIGNAL(startedCall(int, const QString &)), trayIcon, SLOT(startedCall(int, const QString &)));
	connect(callHandler, SIGNAL(stoppedCall(int)),                  trayIcon, SLOT(stoppedCall(int)));
	connect(callHandler, SIGNAL(startedRecording(int)),             trayIcon, SLOT(startedRecording(int)));
	connect(callHandler, SIGNAL(stoppedRecording(int)),             trayIcon, SLOT(stoppedRecording(int)));
}

void Recorder::about()
{
	if (!aboutDialog)
		aboutDialog = new AboutDialog;

	aboutDialog->raise();
	aboutDialog->activateWindow();
}

void Recorder::openPreferences()
{
	debug("Show preferences dialog");

	if (!preferencesDialog)
	{
		preferencesDialog = new PreferencesDialog();
	}

	preferencesDialog->raise();
	preferencesDialog->activateWindow();
}

void Recorder::closePerCallerDialog()
{
	debug("Hide per-caller dialog");
	if (preferencesDialog)
		preferencesDialog->closePerCallerDialog();
}

void Recorder::browseCalls()
{
	QString path(settings.filesDirectory());
	QDir().mkpath(path);
	QUrl url = QUrl(QString("file://") + path);
	bool ret = QDesktopServices::openUrl(url);

	if (!ret)
		QMessageBox::information(NULL, PROGRAM_NAME,
			QString("Failed to open URL %1").arg(QString(url.toEncoded())));
}

void Recorder::quitConfirmation()
{
	debug("Request to quit");
	quit();
}

void Recorder::skypeNotify(const QString &s)
{
	QStringList args = s.split(' ');
	QString cmd = args.takeFirst();
	if (cmd == "CALL")
		callHandler->callCmd(args);
}

void Recorder::skypeConnected(bool conn)
{
	if (conn) debug("skype connection established");
	else debug("skype not connected");
}

void Recorder::skypeConnectionFailed(const QString &reason)
{
	debug("skype connection failed, reason: " + reason);

	QMessageBox::critical(NULL, PROGRAM_NAME " - Error",
		QString("The connection to Skype failed!  %1 cannot operate without this "
		"connection, please make sure you haven't blocked access from within Skype.\n\n"
		"Internal reason for failure: %2").arg(PROGRAM_NAME, reason));
}

void Recorder::debugMessage(const QString &s)
{
	//Q_UNUSED(s)
	std::cout << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ").toLocal8Bit().constData()
		<< s.toLocal8Bit().constData() << "\n";
}

int main(int argc, char **argv)
{
	Recorder recorder(argc, argv);
	QMap<QLocale::Language, QString> translations;

	//adding translations search paths
	QStringList translation_dirs;
	translation_dirs << QCoreApplication::applicationDirPath();
	translation_dirs << QCoreApplication::applicationDirPath()+"/translations";
#ifdef PROGRAM_DATA_DIR
	translation_dirs << QString(PROGRAM_DATA_DIR)+"/translations";
#endif
#ifdef Q_WS_MAC
	translation_dirs << QCoreApplication::applicationDirPath()+"/../Resources";
#endif
	//looking for qm-files in translation directories
	QStringListIterator dir_path(translation_dirs);
	while(dir_path.hasNext())
	{
		QDir dir(dir_path.next());
		QStringList fileNames = dir.entryList(QStringList("skyperec_*.qm"));
		for(int i=0; i < fileNames.size(); ++i)
		{
			QString filename(fileNames[i]);
			QString fullpath(dir.absoluteFilePath(filename));
			filename.remove(0, filename.indexOf('_') + 1);
			filename.chop(3);
			QLocale locale(filename);
			if(!translations.contains(locale.language()))
			{
				translations[locale.language()]=fullpath;
			}
		}
	}
	translations[QLocale::English]="";

	QLocale::Language language = QLocale::system().language();
#ifdef unix
	//Fixing Qt's problem on unix systems...
	QString system_lang(qgetenv("LANG").constData());
	system_lang.truncate(system_lang.indexOf('.'));
	if(system_lang.size()>0) language = QLocale(system_lang).language();
#endif

	QTranslator translator;
	translator.load(translations[language]);
	qApp->installTranslator(&translator);

	return recorder.exec();
}

