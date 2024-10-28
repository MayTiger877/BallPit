/*
  ==============================================================================

	Scales.h
	Created: 18 Oct 2024 4:37:51pm
	Author:   Jan-Willem Smaal <usenet@gispen.org>
	
	give the deserved credit :)

  ==============================================================================
*/


/**
 * @file midi-scales.hpp
 * @author Jan-Willem Smaal <usenet@gispen.org>
 * @date 3/9/2020
 * @copyright APACHE-2.0
 */

#pragma once
#ifndef __midi_scales_h_hpp
#define __midi_scales_h_hpp 

#include <inttypes.h>
#include <string>
#include <JuceHeader.h>

 /*
  * "Intervallen" in Dutch
  */
#define PRIME               0
#define KLEINE_SECUNDE      1
#define GROTE_SECUNDE       2
#define KLEINE_TERTS        3
#define GROTE_TERTS         4
#define REINE_KWART         5
#define OVERMATIGE_KWART    6
#define VERMINDERDE_KWINT   6
#define TRITONUS            6
#define REINE_KWINT         7
#define KLEINE_SEXT         8
#define GROTE_SEXT          9
#define KLEINE_SEPTIEM     10
#define GROTE_SEPTIEM      11
#define OCTAAF             12

  /*
   * Intervals in ENGLISH with the DUTCH equivalent
   */
#define PERFECT_UNISON      PRIME               // 0
#define AUGMENTED_UNISON    KLEINE_SECUNDE      // 1
#define MINOR_SECOND        KLEINE_SECUNDE      // 1
#define MAJOR_SECOND        GROTE_SECUNDE       // 2
#define AUGMENTED_SECOND    KLEINE_TERTS        // 3
#define MINOR_THIRD         KLEINE_TERTS        // 3
#define MAJOR_THIRD         GROTE_TERTS         // 4
#define PERFECT_FOURTH      REINE_KWART         // 5
#define TRITONE             OVERMATIGE_KWART    // 6
#define DIMINISHED_FIFTH    VERMINDERDE_KWINT   // 6
#define PERFECT_FIFTH       REINE_KWINT         // 7
#define AUGMENTED_FIFTH     KLEINE_SEXT         // 8
#define MINOR_SIXTH         KLEINE_SEXT         // 8
#define MAJOR_SIXTH         GROTE_SEXT          // 9
#define AUGMENTED_SIXTH     KLEINE_SEPTIEM      // 10
#define MINOR_SEVENTH       KLEINE_SEPTIEM      // 10
#define MAJOR_SEVENTH       GROTE_SEPTIEM       // 11
#define PERFECT_OCTAVE      OCTAAF              // 12


// Too much hassle to change.
enum InterVal {
	H = 1,
	W = 2,
	WH = 3,
	WW = 4
};


/** Scale is a musical scale class
 * @author Jan-Willem Smaal <usenet@gispen.org>
 */
class Scale {
public:
	enum class ScaleKinds {
		CHROMATIC,
		OCTATONIC,
		DOMINANT_DIMINISHED,
		DIMINISHED,
		MAJOR,
		MINOR,
		MELODIC_MINOR,
		HARMONIC_MINOR,
		GYPSY,
		SYMETRICAL,
		ENIGMATIC,
		ARABIAN,
		HUNGARIAN,
		WHOLE_TONE,
		AUGMENTED,
		BLUES_MAJOR,
		BLUES_MINOR,
		PENTATONIC,
		MINOR_PENTATONIC
	};

	// Constructor
	Scale(ScaleKinds kindOfScale, uint8_t modeOf);

	// These setters change the object.
	bool SetMode(uint8_t modeOf);
	bool SetScale(ScaleKinds kindOfScale);

	ScaleKinds scale;
	std::string scaleName;
	std::string modeName;
	uint8_t notes;
	uint8_t mode;

	// Max number of modes starting with 0 == first mode
	uint8_t modes;
	// These don't modify the Object
	const std::string Text(uint8_t rootnote,
		bool flats);
	static const std::string NoteToText(uint8_t midinote,
		bool flats,
		bool showoctave);
	//private:
	uint8_t* ptrToScale;

	/*
	 * This is just here to trace 'FACADE' in the flash
	 */
	uint8_t  facade[3] = {
	   0xfa,0xca,0xde
	};


	/*
	 * Scales are listed below as arrays of
	 * uint8_t indicating the either a 1/2 step as 1
	 * or a whole step as 2.
	 * minor 3'rd as 3
	 */
	 /*
	  * CHROMATIC Scale 12 note
	  */
	uint8_t  chromatic[12] = {
	   H,H,H,H,H,H,H,H,H,H,H,H,
	};

	/*
	 * OCTATONIC 8 notes (of course)
	 */
	uint8_t  octatonic[2][8] = {
	   {H,W,H,W,H,W,H,W},
	   {W,H,W,H,W,H,W,H}
	};

	/*
	 * Dominant Diminished (Dom13, b9,#9, b5)   8 note scale
	 * same as "first mode of OCTATONIC" see above
	 */
	uint8_t  dominant_diminished[8] = {
	   H,W,H,W,H,W,H,W
	};

	/*
	 * Diminished (Dim7, Maj/b9)  8 note scale
	 * same as "second mode of OCTATONIC" see above
	 */
	uint8_t  diminished[8] = {
	   W,H,W,H,W,H,W,H
	};


	/*
	 * HEPTATONIC scales used by the
	 * majority of western music
	 */

	 /*
	  * MAJOR Scale modes  7 notes
	  */
	  /* Be aware when putting these things in ORY
	   * we need a MACRO to access them e.g. like below
	   * uint8_t (*scale)[7] = pgm_read_ptr(&major[0]);
	   */
	uint8_t major_s[7][7] = {
	   {W,W,H,W,W,W,H}, // IONIAN
	   {W,H,W,W,W,H,W}, // DORIAN
	   {H,W,W,W,H,W,W}, // PHRYGIAN
	   {W,W,W,H,W,W,H}, // LYDIAN
	   {W,W,H,W,W,H,W}, // MIXOLYDIAN
	   {W,H,W,W,H,W,W}, // AEOLIAN
	   {H,W,W,H,W,W,W}  // LOCRIAN
	};

	/*
	 * MINOR Scale modes  7 notes
	 */
	uint8_t  minor_s[7][7] = {
	   {W,H,W,W,H,W,W}, // AEOLIAN
	   {H,W,W,H,W,W,W}, // LOCRIAN
	   {W,W,H,W,W,W,H}, // IONIAN
	   {W,H,W,W,W,H,W}, // DORIAN
	   {H,W,W,W,H,W,W}, // PHRYGIAN
	   {W,W,W,H,W,W,H}, // LYDIAN
	   {W,W,H,W,W,H,W} // MIXOLYDIAN
	};

	/*
	 * MELODIC MINOR Scale modes  7 notes
	 */
	uint8_t  melodic_minor[7][7] = {
	   {W,H,W,W,W,W,H}, // Melodic minor     (minor major7)
	   {H,W,W,W,W,H,W}, // DORIAN bW         (minor7 sus4 b9)
	   {W,W,W,W,H,W,H}, // LYDIAN augmented  (major7 #4 #5)
	   {W,W,W,H,W,H,W}, // MIXOLYDIAN #HH    (dominant7 b5)
	   {W,W,H,W,H,W,W}, // MIXOLYDIAN b6     (dominant7 b6)
	   {W,H,W,H,W,W,W}, // LOCRIAN natural 9 (minor9 b6)
	   {H,W,H,W,W,W,W}  // Altered Dominant  (dominant7, #9, b5, #5)
	};

	/*
	 * HARMONIC MINOR Scale modes  7 notes
	 */
	uint8_t  harmonic_minor[7][7] = {
	   {W,H,W,W,H,WH,H}, // Harmonic minor    (minor major7)
	   {H,W,W,H,WH,H,W}, // LOCRIAN Nat.6     (minor7 b5)
	   {W,W,H,WH,H,W,H}, // IONIAN Augmented  (major7 sus4, #5)
	   {W,H,WH,H,W,H,W}, // DORIAN #HH        (minor7 #HH)
	   {H,WH,H,W,H,W,W}, // PHRYGIAN major    (dominant7 sus4, b9, #5
	   {WH,H,W,H,W,W,H}, // LYDIAN #9         (major7 #9,#HH)
	   {H,W,H,W,W,H,WH} // ALTERED DOM bb7   (dim7)
	};

	/*
	 * Gypsy scale
	 */
	uint8_t  gypsy[7] = {
	   W,H,WH,H,H,WH,H
	};


	/*
	 * Symetrical scale
	 */
	uint8_t  symetrical[7] = {
	   H,W,W,WH,H,H,W
	};

	/*
	 * Enigmatic scale
	 */
	uint8_t  enigmatic[7] = {
	   H,WH,W,W,W,H,H
	};

	/*
	 * Arabian scale
	 */
	uint8_t  arabian[7] = {
	   W,W,H,H,W,W,W
	};

	/*
	 * Hungarian scale
	 */
	uint8_t  hungarian[7] = {
	   WH,H,W,H,W,H,W
	};

	/*
	 * Whole tone (Dom7 #5, b6)   6 note scale
	 */
	uint8_t  whole_tone[6] = {
	   W,W,W,W,W,W
	};
	//  uint8_t *hexatonic  = whole_tone;


	/*
	 * Augmented (Aug)   6 note scale
	 * (two modes? how does one call this second one then)
	 */
	uint8_t  augmented[2][6] = {
	   {WH,H,WH,H,WH,H},
	   {H,WH,H,WH,H,WH}
	};

	/*
	 * Blues major  6 note scale
	 */
	uint8_t  blues_major[6] = {
	   W,H,H,WH,W,WH
	};

	/*
	 * Blues minor  6 note scale
	 * not sure if these are called "modes"
	 */
	uint8_t  blues_minor[6][6] = {
	   {WH,W,H,H,WH,W},
	   {W,H,H,WH,W,WH},      // Same as blues major scale
	   {H,H,WH,W,WH,W},
	   {H,WH,W,WH,W,H},
	   {WH,W,WH,W,H,H},
	   {W,WH,W,H,H,WH}
	};

	/*
	 * Major Pentatonic  5 note scale
	 */
	uint8_t  pentatonic[5] = {
	   W,W,WH,W,WH
	};

	/*
	 * Minor Pentatonic  5 note scale
	 */
	uint8_t  minor_pentatonic[5] = {
	   WH,W,W,WH,W
	};

	/*
	 * Evey embedded program needs a
	 * a piece of dead beef
	 */
	uint8_t  deadbeef[4] = {
	   0xde,0xad,0xbe,0xef,
	};


	void getMIDINotesFromScale(int *scaleNotes, int rootNote);
};

/////////////////////////////////////////////////////

/** Chord implements chords and inversions
  */
class Chord {
public:
	enum class Kinds {
		POWER,
		BASIC,
		SEVENTH,
		NINE,
		ELEVEN,
		THIRTEEN,
		SUS4
	};
	// Basic chord
	Chord(Scale* scl,
		Kinds kindOfChord,
		uint8_t rootnote);
	// Slash Chords have a different
	// bassnote from the rootnote
	Chord(Scale* scl,
		Kinds kindOfChord,
		uint8_t rootnote,
		uint8_t bassnote);
	// Invert the chord leaving the bassnote intact
	void Invert(unsigned int);
	// Return a text representation of the chord
	const std::string Text(bool flats);
private:
	Scale* scale;
	uint8_t notes[3];
	uint8_t rootnote;
	uint8_t bassnote;
};


/* End of header file  */
#endif 
