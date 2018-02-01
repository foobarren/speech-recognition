/*
 * Copyright 2012, 2013 Fabian Ebner
 * The program is licensed under the GPL version 3 or (at your opinion) any later version, see the file COPYING for details.
 */

#include "list.h"
#include "wave.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdint.h>
// #include <sys/types.h>


void new_mfcc(mfcc_frame *frames, unsigned int n, char *name)
{
	FILE *f = NULL;
	unsigned int i = 0, j = 0;
	// chdir("mfcc");
	f = fopen(name, "w");
	fprintf(f, "%u\n", n);
	//In der Datei weden einfach alle Merkmale von Leerzeichen getrennt ausgegeben
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < N_MFCC; j++)
			fprintf(f, "%lf ", frames[i].features[j]);
		fprintf(f, "\n");
	}
	fclose(f);
	// chdir("..");
}

int get_mfcc_list(mfcclist **head)
{
	DIR *d = opendir("mfcc");
	FILE *f = NULL;
	struct dirent *dentry = readdir(d);
	unsigned int i = 0, j = 0;
	unsigned int count = 0;

	*head = NULL;
	mfcclist *mfcc=NULL;
	//Holen der Woerter
	chdir("mfcc");
	while (dentry != NULL)
	{
		//Ignoriere die Standardeintraege
		if (!memcmp(dentry->d_name, ".", 1))
		{
			dentry = readdir(d);
			continue;
		}
		if (!memcmp(dentry->d_name+strlen(dentry->d_name)-5, "mfcc", 4))
		{
			dentry = readdir(d);
			continue;
		}
		if(*head ==NULL){
			mfcc = malloc(sizeof(mfcclist));
			*head = mfcc;
			mfcc->next=NULL;
		}else	
		{
			mfcc->next = malloc(sizeof(mfcclist));
			mfcc = mfcc->next;	
			mfcc->next=NULL;
		}
		//Eintrag mit Daten fuellen
		f = fopen(dentry->d_name, "r");
		mfcc->name = malloc(strlen(dentry->d_name) + 1);
		memcpy(mfcc->name, dentry->d_name, strlen(dentry->d_name) + 1);
		fscanf(f, "%u", &(mfcc->n));
		mfcc->frames = malloc(sizeof(mfcc_frame) * mfcc->n);
		//Auslesen
		for (i = 0; i < mfcc->n; i++)
			for (j = 0; j < N_MFCC; j++)
				fscanf(f, "%lf", &(mfcc->frames[i].features[j]));
		fclose(f);

		dentry = readdir(d);
		count++;
		//Erzeuge neuen Eintrag, wenn benoetigt und ignoriere die Standardeintraege			
	}
	chdir("..");
	closedir(d);
	return count;
}
void free_mfcc_list(mfcclist *head)
{
	mfcclist *mfcc= head;
	while (mfcc != NULL)
	{		
		free(mfcc->frames);
		free(mfcc->name);
		mfcclist *oldmfcc = mfcc;
		mfcc = mfcc->next;
		free(oldmfcc);
	}
}
int get_wav_list(const char* path,wavlist **head)
{
	DIR *d = opendir(path);
	// FILE *f = NULL;
	struct dirent *dentry = readdir(d);
	// unsigned int i = 0, j = 0;
	unsigned int count = 0;

	*head = NULL;
	wavlist *wavs=NULL;
	
	chdir(path);
	while (dentry != NULL)
	{
		//Ignoriere die Standardeintraege
		if (!memcmp(dentry->d_name, ".", 1))
		{
			dentry = readdir(d);
			continue;
		}
		wav* wav= NULL;
		int ret = open_wave(dentry->d_name,&wav);
		if(ret==0)
		{
			if(*head ==NULL){
				wavs = malloc(sizeof(wavlist));
				*head = wavs;
				wavs->next=NULL;
			}else	
			{
				wavs->next = malloc(sizeof(wavlist));
				wavs = wavs->next;
				wavs->next=NULL;
			}
			wavs->wav = wav;			
			count++;
		}else{
			printf("open file error is %d \n",ret);
		}
		dentry = readdir(d);
	}
	chdir("..");
	closedir(d);
	return count;
}
void free_wav_list(wavlist *head)
{
	wavlist *wavs= head;
	while (wavs != NULL)
	{		
		close_wave(wavs->wav);
		// free(wavs->wav);
		wavlist *oldwavs = wavs;
		wavs = wavs->next;
		free(oldwavs);
	}
}