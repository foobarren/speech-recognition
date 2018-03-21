/*
 * Copyright 2012, 2013 Fabian Ebner
 * The program is licensed under the GPL version 3 or (at your opinion) any later version, see the file COPYING for details.
 */

#include "fft.h"
#include "split.h"
#include "frame.h"
#include "list.h"
#include "compare.h"
#include "wave.h"
#include "audio.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define FRAMES 6
#define SAMPLES (FRAMES+1)*(N - OVERLAP)
#define WINSTEP ((N-OVERLAP)*1000.0/WAVE_SAMPLE_RATE)

#define MAX_SIZE 255

char curpath[MAX_SIZE];
int isDebug = 0;

void words_capture(int how);
void create_basemfcc();
void mfcc_compare(int type);

int main(int argc,char *argv[])
{
	if(argc==2)
	{
		if (memcmp(argv[1],"-v",2)== 0)
		{
			isDebug = 1;
		}
	} 

	char ans[50];
	do
	{
		printf("(m)make mfcc files from waves path, (l)list mfcc path files, (c)compare files in compare path,(b)best score,  (e)exit: ");
		scanf("%s", (char *)ans);
		// printf("enter %s \n",ans);
		// getcwd(curpath,sizeof(curpath));
		// printf("path is %s \n",curpath);
		switch (ans[0])
		{
			case 'm':
				// usleep(200000);
				create_basemfcc();
				break;
			case 'l':
				system("ls mfcc");
				break;
			case 'c':
				// usleep(200000);
				mfcc_compare(1);
				break;
			case 'b':
				// usleep(200000);
				mfcc_compare(0);
				break;
		}
	}
	while (ans[0] != 'e');
	return 0;
}
void create_basemfcc()
{
	unsigned int size = 0;
	wavlist *head=NULL,*node=NULL;	
	get_wav_list("waves",&head);
	node = head;
	chdir("basemfcc");
	while (node != NULL)
	{
		void* buffer=NULL;		
		frame *frames = NULL;
		mfcc_frame *mfccframes = NULL;
		int frame_n =0;
		size = read_pcm(node->wav,&buffer);
		size = size*8/WAVE_BITS_PER_SAMPLE;
		frame_n = make_frames_hamming((int16_t*)buffer, size, &frames);
		mfccframes = (mfcc_frame*)malloc(sizeof(mfcc_frame) * frame_n);
		mfcc_features(frames, frame_n, mfccframes);
		char *path = (char*)malloc(strlen(node->wav->name) + 6);
		char *ext = ".mfcc";
		memcpy(path, node->wav->name, strlen(node->wav->name));
		memcpy(path + strlen(node->wav->name), ext, 6);
		new_mfcc(mfccframes, frame_n, path);
		free(mfccframes);
		free(frames);
		// free(buffer);
		free(path);
		node = node->next;
	}
	free_wav_list(head);
	chdir("..");
}
//type 1 found immediately break
//type 0 found best score
void mfcc_compare(int type)
{
	unsigned int size = 0;
	wavlist *head=NULL,*node=NULL;
	if(get_wav_list("compare",&head))
	{
		node = head;
		mfcclist *words = NULL,*wordhead=NULL;
		unsigned int i = 0;
		if (get_mfcc_list(&words))
		{
			wordhead = words;
			while (node != NULL)
			{
				void* buffer=NULL;
				size = read_pcm(node->wav,&buffer);
				size = size/WAVE_BYTE_PER_SAMPLE;
				unsigned int isfound = 0;
				unsigned int beginsample = 0; //(FRAMES+1)*(N - OVERLAP);
				unsigned int nsamples = size/(N - OVERLAP)-1;
				struct timeval tv1,tv2,tv3;
				struct timezone tz1,tz2,tz3;
				
				// gettimeofday(&tv2, &tz2);
				for (i=0;i<nsamples;i++)
				{							
					frame *frames = NULL;
					mfcc_frame *mfcc_frames = NULL;
					int frame_n =0;
					gettimeofday(&tv1, &tz1);
					frame_n = make_frames_hamming((int16_t*)(buffer+beginsample*WAVE_BYTE_PER_SAMPLE), SAMPLES, &frames);
					mfcc_frames = (mfcc_frame*)malloc(sizeof(mfcc_frame) * frame_n);
					mfcc_features(frames, frame_n, mfcc_frames);
					gettimeofday(&tv2, &tz2);
					if(isDebug)
						printf("i:%d  mfcc time %ld . %ld\n",i,tv2.tv_sec-tv1.tv_sec,tv2.tv_usec-tv1.tv_usec);
					words = wordhead;
					while (words != NULL)
					{
						double now = compare(mfcc_frames, frame_n, words->frames, frame_n);
						if(now <0.9){
							isfound =1;
							printf("match at second:%f  score:%f mfcc:%s wav:%s \n", i*WINSTEP/1000,now, words->name,node->wav->name);
							if(type)
								break;
						}							
						words = words->next;
					}					
					beginsample = beginsample+ (N - OVERLAP);
					if(isDebug)
					{
						char *path = (char*)malloc(strlen(node->wav->name) + 9);
						char *ext = ".mfcc";
						char str_int[4];
						sprintf(str_int, "%d", i); 
						memcpy(path, node->wav->name, strlen(node->wav->name));
						memcpy(path + strlen(node->wav->name), str_int, strlen(str_int));
						memcpy(path + strlen(node->wav->name)+strlen(str_int), ext, 6);
					new_mfcc(mfcc_frames, frame_n, path);
						free(path);
					}						
					free(mfcc_frames);
					free(frames);
					gettimeofday(&tv3, &tz3);
					if(isDebug)
						printf("i:%d  compare time %ld . %ld\n",i,tv3.tv_sec-tv1.tv_sec,tv3.tv_usec-tv1.tv_usec);
					if(isfound && type)
						break;
				}
				if(isfound==0)
					printf("wav %s match  failed!!! \n",node->wav->name);
				// gettimeofday(&tv3, &tz3);
				// printf("compare time %ld . %ld\n",tv3.tv_sec-tv2.tv_sec,tv3.tv_usec-tv2.tv_usec);
				node=node->next;
			}	
			free_mfcc_list(wordhead);		
		}
		free_wav_list(head);		
	}
}
void words_capture(int how)
{
	void *buf = NULL;
	unsigned int size = 0, n = 0, i = 0;
	// char ans[500];
	voice_signal *signals = NULL;
	printf("Please speak now (end with return)\n");
	// capture_start(NULL);
	// getchar();
	// getchar();
	// capture_stop(&size, &buf);
	n = split((int16_t*)buf, size / 2, &signals);
	for (i = 0; i < n; i++)
	{
		frame *frames = NULL;
		mfcc_frame *mfcc_frames = NULL;
		int frame_n = make_frames_hamming(signals[i].buffer, signals[i].number, &frames);
		mfcc_frames = (mfcc_frame*)malloc(sizeof(mfcc_frame) * frame_n);
		mfcc_features(frames, frame_n, mfcc_frames);
		if (!how)
		{
			// play(NULL, signals[i].buffer, signals[i].number * 2);
			// printf("Enter identifier (x to skip): ");
			// scanf("%s", (char *)ans);
			// if (!(ans[0] == 'x' && ans[1] == '\0'))
			// {
			// 	new_word(frames, frame_n, (char *)ans);
			// 	chdir("waves");
			// 	char *path = malloc(strlen(ans) + 5);
			// 	char *ext = ".wav";
			// 	memcpy(path, ans, strlen(ans));
			// 	memcpy(path + strlen(ans), ext, 5);
			// 	write_pcm(signals[i].buffer, signals[i].number * 2, path);
			// 	free(path);
			// 	chdir("..");
			// }
		}
		else
		{
			mfcclist *words = NULL;
			double best = 1e10;
			char *name = NULL;
			void **word_adresses;
			unsigned int n = 0, i = 0, count = 0;
			if ((n = get_mfcc_list(&words)))
			{
				word_adresses = (void**)malloc(n * sizeof(void *));
				while (words != NULL)
				{
					double now = compare(mfcc_frames, frame_n, words->frames, words->n);
					word_adresses[count++] = words;
					if (now < best)
					{
						best = now;
						name = words->name;
					}
					words = words->next;
				}
				for (i = 0; i < count; i++)
					free(word_adresses[i]);
				free(word_adresses);
			}
			if (best < 3.5)
				printf("%f %s", best, name);
		}
		free(mfcc_frames);
		free(frames);
	}
	if (how)
		printf("\n");
	free(buf);
}
