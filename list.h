/*
 * Copyright 2012, 2013 Fabian Ebner
 * The program is licensed under the GPL version 3 or (at your opinion) any later version, see the file COPYING for details.
 */

#ifndef __LIST_H__
#define __LIST_H__

#include "frame.h"
#include "wave.h"
/*
 * Sellt eine Liste gesprochener Woerter durch ihre MFCC-Merkmale dar.
 * (mfcc_frame *) frames	Die MFCC-Merkmale des aktuellen Wortes
 * (unsigned int) n		Die Anzahl der Merkmals-Rahmen
 * (char *) name		Die Bezeichnung des Wortes
 * (struct _word *) next	Das naechste Wort in der verketteten Liste
 */
typedef
	struct _mfcclist
	{
		mfcc_frame *frames;
		unsigned int n;
		char *name;
		struct _mfcclist *next;
	} mfcclist;

typedef
	struct _wavlist
	{
		wav *wav;
		struct _wavlist *next;
	} wavlist;


/*
 * Speichert ein neues Wort in das Verzeichnis words mit dem Dateinamen, die im Parameter name enthalten ist.
 * <<<INPUT>>>
 * (mfcc_frame *) frames	Der Vektor mit den MFCC-Merkmalen des Wortes
 * (unsigned int) n		Die Anzahl der Merkmals-Rahmen
 * (char *) bezeichnung		Die Bezeichnung des Wortes und gleichzeitig der Dateiname, unter welchem abgespeichert wird
 */
void new_mfcc(mfcc_frame *frames, unsigned int n, char *path);

/*
 * Liefert eine Liste aller im Verzeichnis words abgespeicherten Woertern.
 * Die Woerter bekommen als Bezeichnung den Dateinamen unter dem sie abgespeichert wurden.
 * Der Speicher der einzelnen Woerter in der Liste muss spaeter wieder frei gegeben werden.
 * <<<INPUT>>>
 * (word *) head	Wortlisten-Zeiger, in den der Beginn der Wortliste hineingeschrieben wird
 * <<<OUTPUT>>>
 * (int)		Die Anzahl der Woerter
 */
int get_mfcc_list(mfcclist **head);
void free_mfcc_list(mfcclist *head);
int get_wav_list(const char* path,wavlist **head);
void free_wav_list(wavlist *head);

#endif
