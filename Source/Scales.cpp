/*
  ==============================================================================

	Scales.cpp
	Created: 18 Oct 2024 4:37:51pm
	Author:  Jan-Willem Smaal <usenet@gispen.org>

  ==============================================================================
*/


/**
 * @file midi-scales.cpp
 * @author Jan-Willem Smaal <usenet@gispen.org>
 * @date 3/9/2020
 * @copyright APACHE 2.0
 */

#define JWS_DEBUG 1
#ifdef JWS_DEBUG 
#include <stdio.h>
#endif 

#include <string>
#include <iostream>
#include "Scales.h"

using namespace std;

struct MidiNotes
{
	uint8_t note;
	char name[3];
};


//-----------------------------------------------------------------

/** Scales implements a number of musical scales
 *
 *
 * @author: Jan-Willem Smaal <usenet@gispen.org>
 */

 // Constructors 
Scale::Scale(ScaleKinds kindOfScale, uint8_t modeOf) 
{
	int i;
	int tmp;

	// Most scales are 7 notes (heptatonic) 
	// unless specifically changed for some less common scales 
	// below  
	notes = 7;
	//some exotic scales have just one mode. 
	modes = 0;
	// Init the class with the mode given. 
	//mode = modeOf;

	SetMode(modeOf);
	SetScale(kindOfScale);
};


bool Scale::SetMode(uint8_t modeOf) 
{
	// TODO: check if the argument is in range
	Scale::mode = modeOf;
	SetScale(Scale::scale);
	return true;
}


bool Scale::SetScale(ScaleKinds kindOfScale) 
{
	int i;
	int tmp;

	// Most scales are 7 notes (heptatonic)
	// unless specifically changed for some less common scales
	// below
	notes = 7;
	//some exotic scales have just one mode.
	modes = 0;

	switch (kindOfScale) {
	case Scale::ScaleKinds::MAJOR:
		ptrToScale = &major_s[mode][0];
		modes = 7;
		scale = Scale::ScaleKinds::MAJOR;
		scaleName = "Major";
		switch (Scale::mode) 
		{
		case 0:
			modeName = "Ionian";
			break;
		case 1:
			modeName = "Dorian";
			break;
		case 2:
			modeName = "Phrygian";
			break;
		case 3:
			modeName = "Lydian";
			break;
		case 4:
			modeName = "Mixolydian";
			break;
		case 5:
			modeName = "Aeolian";
			break;
		case 6:
			modeName = "Locrian";
			break;
		default:
			modeName = "?";
			break;

		}
		break;
	case Scale::ScaleKinds::MINOR:
		ptrToScale = &minor_s[mode][0];
		modes = 7;
		scale = Scale::ScaleKinds::MINOR;
		scaleName = "Minor";
		switch (Scale::mode) {
		case 0:
			modeName = "Aeolian";
			break;
		case 1:
			modeName = "Locrian";
			break;
		case 2:
			modeName = "Ionian";
			break;
		case 3:
			modeName = "Dorian";
			break;
		case 4:
			modeName = "Phrygian";
			break;
		case 5:
			modeName = "Lydian";
			break;
		case 6:
			modeName = "Mixolydian";
			break;
		default:
			modeName = "?";
			break;
		}
		break;
	case Scale::ScaleKinds::MELODIC_MINOR:
		ptrToScale = &melodic_minor[mode][0];
		modes = 7;
		scale = Scale::ScaleKinds::MELODIC_MINOR;
		scaleName = "Melodic minor";
		switch (Scale::mode) {
		case 0:
			modeName = "Melodic minor";
			break;
		case 1:
			modeName = "Dorian b2";
			break;
		case 2:
			modeName = "Lydian augmented";
			break;
		case 3:
			modeName = "Mixolydian #11";
			break;
		case 4:
			modeName = "Mixolydian b6";
			break;
		case 5:
			modeName = "Locrian natural9";
			break;
		case 6:
			modeName = "Altered Dominant";
			break;
		default:
			modeName = "?";
			break;
		}

		break;
	case Scale::ScaleKinds::HARMONIC_MINOR:
		ptrToScale = &harmonic_minor[mode][0];
		modes = 7;
		scale = Scale::ScaleKinds::HARMONIC_MINOR;
		scaleName = "Harmonic minor";
		switch (Scale::mode) {
		case 0:
			modeName = "Harmonic minor";
			break;
		case 1:
			modeName = "Locrian natural6";
			break;
		case 2:
			modeName = "Ionian augmented";
			break;
		case 3:
			modeName = "Dorian #11";
			break;
		case 4:
			modeName = "Phrygian major";
			break;
		case 5:
			modeName = "Lydian #9";
			break;
		case 6:
			modeName = "Altered dominant bb7";
			break;
		default:
			modeName = "?";
			break;
		}
		break;
		// No modes in these ones
	case Scale::ScaleKinds::GYPSY:
		notes = 7;
		ptrToScale = &gypsy[0];
		scale = Scale::ScaleKinds::GYPSY;
		scaleName = "Gypsy";
		break;
	case Scale::ScaleKinds::SYMETRICAL:
		ptrToScale = &symetrical[0];
		scale = Scale::ScaleKinds::SYMETRICAL;
		scaleName = "Symetrical";
		break;
	case Scale::ScaleKinds::ENIGMATIC:
		ptrToScale = &enigmatic[0];
		scale = Scale::ScaleKinds::ENIGMATIC;
		scaleName = "Enigmatic";
		break;
	case Scale::ScaleKinds::ARABIAN:
		ptrToScale = &arabian[0];
		scale = Scale::ScaleKinds::ARABIAN;
		scaleName = "Arabian";
		break;
	case Scale::ScaleKinds::HUNGARIAN:
		ptrToScale = &hungarian[0];
		scale = Scale::ScaleKinds::HUNGARIAN;
		scaleName = "Hungarian";
		break;
		// These scales have 6 notes
	case Scale::ScaleKinds::WHOLE_TONE:
		notes = 6;
		ptrToScale = &whole_tone[0];
		scale = Scale::ScaleKinds::WHOLE_TONE;
		scaleName = "Whole tone";
		break;
		// Augmented has two modes not sure what to call
		// the second one
	case Scale::ScaleKinds::AUGMENTED:
		notes = 6;
		modes = 2;
		ptrToScale = &augmented[mode][0];
		scale = Scale::ScaleKinds::AUGMENTED;
		scaleName = "Augmented";
		break;
	case Scale::ScaleKinds::BLUES_MAJOR:
		notes = 6;
		ptrToScale = &blues_major[0];
		scale = Scale::ScaleKinds::BLUES_MAJOR;
		scaleName = "Blues major";
		break;
		// Blues minor has modes I guess...
	case Scale::ScaleKinds::BLUES_MINOR:
		notes = 6;
		modes = 6;
		if (mode >= 6) {
			mode = 0;
		}
		ptrToScale = &blues_minor[mode][0];
		scale = Scale::ScaleKinds::BLUES_MINOR;
		scaleName = "Blues minor";
		break;
		// These scales have 5 notes
	case Scale::ScaleKinds::PENTATONIC:
		notes = 5;
		ptrToScale = &pentatonic[0];
		scale = Scale::ScaleKinds::PENTATONIC;
		scaleName = "Pentatonic";
		break;
	case Scale::ScaleKinds::MINOR_PENTATONIC:
		notes = 5;
		ptrToScale = &minor_pentatonic[0];
		scale = Scale::ScaleKinds::MINOR_PENTATONIC;
		scaleName = "Minor Pentatonic";
		break;
		// 12 Notes is the max (in our Western system)
	case Scale::ScaleKinds::CHROMATIC:
		notes = 12;
		ptrToScale = &chromatic[0];
		scale = Scale::ScaleKinds::CHROMATIC;
		scaleName = "Chromatic";
		break;
		// 8 note keys
	case Scale::ScaleKinds::OCTATONIC:
		notes = 8;
		modes = 2;
		if (mode >= 2) {
			mode = 0;
		}
		ptrToScale = &octatonic[mode][0];
		scale = Scale::ScaleKinds::OCTATONIC;
		scaleName = "Octatonic";
		break;
	case Scale::ScaleKinds::DOMINANT_DIMINISHED:
		notes = 8;
		// ptrToScale = &dominant_diminished[0];
		// Dominant dim is the same as the first mode of the
		// octatonic so we could also do the below
		ptrToScale = &octatonic[0][0];
		scale = Scale::ScaleKinds::DOMINANT_DIMINISHED;
		scaleName = "Dominant Diminished";
		break;
	case Scale::ScaleKinds::DIMINISHED:
		notes = 8;
		// ptrToScale = &dominant_diminished[0];
		// dim is the same as the second mode of the
		// octatonic so we could also do the below
		ptrToScale = &octatonic[1][0];
		scale = Scale::ScaleKinds::DIMINISHED;
		scaleName = "Diminished";
		break;
	default:
		mode = 0;
		notes = 0;
		break;
	}
	return true;
}


/** Print out text representation of the scale starting at rootnote
  */
const std::string Scale::Text(uint8_t rootnote, bool flats)
{
	int i;
	uint8_t tmp;
	std::string str;

	tmp = rootnote;
	if (notes != 0) 
	{
		for (i = 0; i < (unsigned int)Scale::notes; i++) 
		{
			str = str + NoteToText(tmp, flats, false) + " ", tmp = tmp + (unsigned int)*(ptrToScale + i);
		}
	}
	return str;
}


/** Simply convert MIDI notename to a note (either using flats or sharps
 */
const std::string Scale::NoteToText(uint8_t midinote, bool flats, bool showoctave)
{
	int octave;
	uint8_t note;
	std::string strng;

	// For MIDI note 0 is written down as "C-2" hence the -2 below	
	octave = (midinote / 12) - 2;
	note = midinote - ((octave + 2) * 12);

	if (flats) 
	{
		switch (note) 
		{
		case 0:
			strng = "C";
			break;
		case 1:
			strng = "Db";
			break;
		case 2:
			strng = "D";
			break;
		case 3:
			strng = "Eb";
			break;
		case 4:
			strng = "E";
			break;
		case 5:
			strng = "F";
			break;
		case 6:
			strng = "Gb";
			break;
		case 7:
			strng = "G";
			break;
		case 8:
			strng = "Ab";
			break;
		case 9:
			strng = "A";
			break;
		case 10:
			strng = "Bb";
			break;
		case 11:
			strng = "B";
			break;
		default:
			strng = "!!";
			break;
		}
	}
	else 
	{
		switch (note) 
		{
		case 0:
			strng = "C";
			break;
		case 1:
			strng = "C#";
			break;
		case 2:
			strng = "D";
			break;
		case 3:
			strng = "D#";
			break;
		case 4:
			strng = "E";
			break;
		case 5:
			strng = "F";
			break;
		case 6:
			strng = "F#";
			break;
		case 7:
			strng = "G";
			break;
		case 8:
			strng = "G#";
			break;
		case 9:
			strng = "A";
			break;
		case 10:
			strng = "A#";
			break;
		case 11:
			strng = "B";
			break;
		default:
			strng = "!!";
			break;
		}
	}

	//octave = (midinote / 12) - 1; 
	if (showoctave) 
	{
		strng = strng + std::to_string(octave);
	}
	return strng;
}


//-----------------------------------------------------------------

/** Chord implements chords and inversions
  */
Chord::Chord(Scale* scl, Kinds KindOfChord, uint8_t rootnote) 
{
	int i, j;
	uint8_t nte = rootnote;

	Chord::rootnote = rootnote;
	Chord::bassnote = rootnote;
	scale = scl;

	// Basic triad is the only one implemented till now.
	for (i = 0; i < (unsigned int)scl->notes; i++) 
	{
		switch (i) 
		{
		case 0:
			notes[0] = nte;
			break;
		case 2:
			notes[1] = nte;
			break;
		case 4:
			notes[2] = nte;
			break;
		default:
			break;
		}
		nte = nte + (unsigned int)*(scl->ptrToScale + i);
	}
};


const std::string Chord::Text(bool flats) 
{

	return "( " + Scale::NoteToText(notes[0], flats, false) + "," +
				  Scale::NoteToText(notes[1], flats, false) + "," +
				  Scale::NoteToText(notes[2], flats, false) + ")";
}

void Scale::getMIDINotesFromScale(int *scaleNotes, int rootNote)
{
	if (!scaleNotes)
	{
		return;
	}
	
	scaleNotes[0] = rootNote;

	// Fill in the rest of the notes for the scale
	//for (int i = 1; i < notes; ++i)
	for (int i = 1; i < 8; ++i)
	{
		scaleNotes[i] = scaleNotes[i - 1] + ptrToScale[i - 1];
	}
	
}

/* EOF */