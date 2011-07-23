/*
	SkypeRec
	Copyright 2008-2009 by jlh <jlh@gmx.ch>
	Copyright 2010-2011 by Peter Savichev  (proton) <psavichev@gmail.com>

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

#ifndef COMMON_H
#define COMMON_H

#include <QtDebug>

#define PROGRAM_NAME "SkypeRec"

class Recorder;
class QString;

extern void debug(const QString &);

extern Recorder *recorderInstance;

// this is provided by the generated version.cpp file
extern const char *const recorderCommit;
extern const char *const recorderDate;
extern const char *const recorderVersion;

#define DISABLE_COPY_CONSTRUCTOR(c) \
	private: c(const c &)
#define DISABLE_ASSIGNMENT_OP(c) \
	private: c &operator=(const c &)
#define DISABLE_COPY_AND_ASSIGNMENT(c) \
	DISABLE_COPY_CONSTRUCTOR(c); \
	DISABLE_ASSIGNMENT_OP(c)

const long skypeSamplingRate = 16000;
extern const char *const websiteURL;

#endif

