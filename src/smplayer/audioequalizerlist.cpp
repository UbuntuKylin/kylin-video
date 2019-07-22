/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2018 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "audioequalizerlist.h"
#include <QDebug>

#define ANEQUALIZER_CHANNELS 2
#define ANEQUALIZER_WIDTH 1000
#define ANEQUALIZER_TYPE 0
#define ANEQUALIZER_SCALE 20 / 240

#define FIREQUALIZER_SCALE 24 / 240

QString AudioEqualizerHelper::equalizerListToString(AudioEqualizerList values, AudioEqualizerType type) {
	QString s;

	if (type == Equalizer) {
		for (int n = 0; n < 10; n++) {
			double v = (double) values[n].toInt() / 10;
			s += QString::number(v);
			if (n < 9) s += ":";
		}
	}
	else
	if (type == Anequalizer) {
		for (int ch = 0; ch < ANEQUALIZER_CHANNELS; ch++) {
			double freq = 31.25;
			for (int f = 0; f < 10; f++) {
				double v = (double) values[f].toInt() * ANEQUALIZER_SCALE;
				s += QString("c%1 f=%2 w=%4 g=%3 t=%5|").arg(ch).arg(freq).arg(v).arg(ANEQUALIZER_WIDTH).arg(ANEQUALIZER_TYPE);
				freq = freq * 2;
			}
		}
	}
	else
	if (type == Firequalizer) {
		s = "gain_entry='";
		double freq = 31.25;
		for (int f = 0; f < 10; f++) {
            double v = (double) values[f].toInt() * FIREQUALIZER_SCALE;//Segmentation fault
			if (f == 0) {
				double v1 = (double) values[1].toInt() * FIREQUALIZER_SCALE;
				s += QString("entry(0,%1)").arg(2 * v - v1);
			} else {
				s += QString("entry(%1,%2)").arg(freq).arg(v);
			}
			if (f < 9) s += ";";
			freq = freq * 2;
		}
		s += "'";
	}

	return s;
}
