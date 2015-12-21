#include <stdio.h>
#include <algorithm>
#include <string.h>

#define MAX_ASS_COUNT 10
#define MAX_FILENAME_LENGTH 256
#define MAX_LINE_LENGTH 1024
#define MAX_TIMESTR_LENGTH 11   // 00:00:00.00

const int ERR_OPF_LEN = strlen("CAN'T OPEN FILE ");

struct node {
	int pos;
	bool is_start;
	char* assn;
};

struct clip {
	int start;
	int end;
	int assc;
	char assn[MAX_ASS_COUNT][MAX_FILENAME_LENGTH];
};

void err(char msg[]){
	printf(msg);
	exit(-1);
}

double str2sec(char str[]){   // 00:00:00.00 or 0:00:00.00
	int ret = 0;
	char tmp[] = "00";
	char* t1 = strstr(str, ":");
	memcpy(tmp, str, t1 - str);
	ret += atoi(tmp) * 360000;
	memcpy(tmp, t1 + 1, 2);
	ret += atoi(tmp) * 6000;
	memcpy(tmp, t1 + 4, 2);
	ret += atoi(tmp) * 100;
	memcpy(tmp, t1 + 7, 2);
	ret += atoi(tmp);
	return (double) ret / 100;
}

void get_ass_clip(char assn[], node nodearr[], int& pos, double frate, int maxf){
	FILE* fp = fopen(assn, "r");
	if(fp == NULL){
		char msg[MAX_FILENAME_LENGTH + ERR_OPF_LEN];
		sprintf(msg, "CAN'T OPEN FILE %s", assn);
		err(msg);
	}
	int min = INT_MAX, max = INT_MIN, ts, te;
	char strstart[MAX_TIMESTR_LENGTH], strend[MAX_TIMESTR_LENGTH];
	char line[MAX_LINE_LENGTH];
	while(fgets(line, MAX_LINE_LENGTH, fp) != NULL){
		if(strncmp("Dialogue: ", line, strlen("Dialogue: ")) == 0){
			char* t1 = strstr(line, ",");
			char* t2 = strstr(++t1, ",");
			char* t3 = strstr(++t2, ",");
			memcpy(strstart, t1, t2 - t1 - 1);
			memcpy(strend, t2, t3 - t2);
			ts = (int)(str2sec(strstart) * frate + .5d);
			te = (int)(str2sec(strend) * frate + .5d);
			min = ts < min ? ts : min;
			max = te > max ? te : max;
			if(min >= maxf - 1) return;
		}
	}
	max = max >= maxf ? maxf - 1 : max;
	printf("%s: start: %d, end: %d\n", assn, min, max);
	nodearr[pos++] = {min, true, assn};
	nodearr[pos++] = {max, false, assn};
}

int main(int argc, char **argv){
	int assc = argc - 4; //e.g.  .\ASSOA 24000 1001 20000 a.ass b.ass c.ass
	char assn[MAX_ASS_COUNT][MAX_FILENAME_LENGTH];
	node nodearr[MAX_ASS_COUNT * 2];
	int pos = 0;
	if(argc < 5) err("NEED MORE ARGUMENT");
	double frate = atof(argv[1]) / atof(argv[2]);
	int maxf = atoi(argv[3]);
	for(int i = 0;i < assc;i++){
		if(strlen(argv[i + 4]) > MAX_FILENAME_LENGTH) err("FILE NAME TOO LONG");
  		strcpy(assn[i], argv[i + 4]);
		get_ass_clip(assn[i], nodearr, pos, frate, maxf);
	}
	if(pos == 0) err("NO AVAILABLE CLIP");
	
	for(int i = pos - 1; i > 0; i--)   //since only 10 ass files, use bubble sort
		for(int j = 0; j < i; j++)
			if(nodearr[j].pos > nodearr[j + 1].pos)
				std::swap(nodearr[j], nodearr[j + 1]);

	clip cliparr[MAX_ASS_COUNT * 2 - 1];
	int clipc = 0, tassc = 0;
	char tassn[MAX_ASS_COUNT][MAX_FILENAME_LENGTH];
	for(int i = 0;i < pos - 1;i++){
		if(nodearr[i].is_start) strcpy(tassn[tassc++], nodearr[i].assn);
		if(!nodearr[i].is_start){
			for(int j = 0; j < tassc;j++){
				if(strcmp(tassn[j], nodearr[i].assn) == 0){
					for(int k = j;k < tassc;k++)
						strcpy(tassn[k], tassn[k + 1]);
					tassc--;
					break;
				}
			}
		}
		if(nodearr[i].pos != nodearr[i + 1].pos){
			cliparr[clipc] = {nodearr[i].pos, nodearr[i + 1].pos - 1, tassc};
			memcpy(cliparr[clipc].assn, tassn, sizeof(tassn));
			clipc++;
		}
	}
	cliparr[clipc - 1].end++;
	for(int i = 0;i < clipc;i++){
		printf("[%d -> %d], assc: %d, assn: [%s", cliparr[i].start, cliparr[i].end, cliparr[i].assc, cliparr[i].assn[0]);
		for(int j = 1;j < cliparr[i].assc;j++) printf(", %s", cliparr[i].assn[j]);
		printf("]\n");
	}
	return 0;
}
