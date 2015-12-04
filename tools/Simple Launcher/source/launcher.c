/*
--- Simplified BSD License ---------------------------------------------------
Copyright (c) 2015, Robert Planas Jimenez
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.
--------------------------------------------------------------------------------

This launcher is compiled using gcc, with MinGW on windows. 
Compilation commands:
	windres xyz.rc xyz.rc.o
	gcc -c launcher.c
	gcc -o launcher.exe launcher.o xyz.rc.o
*/

#include <stdio.h>
#include <string.h>

#define ARG_SIZE 40
#define VAL_SIZE 100
#define COM_SIZE 600

#define VERBOSE 1

#if defined(_WIN32) || defined(WIN32)
	#define _WIN32_IE 0x0400
	#include <Windows.h>
	#include <shlobj.h>
#endif

char * launcher_name;

char default_config_file[] = \
"player: engine/blenderplayer\n"
"blend: data/game.blend\n\n"
"#video settings\n"
"resolution: 1280 768\n"
"fullscreen: true\n"
"stereomode: nostereo\n"
"anti-aliasing: 8\n\n"
"#game engine options\n"
"debug: false\n"
"fixedtime: false\n"
"nomipmap: false\n"
"show_framerate: false\n"
"show_properties: false\n"
"show_profile: false\n"
"blender_material: false\n"
"ignore_deprecation_warnings: true\n\n"
"#Custom command line\n"
"#exec: ./blenderplayer mygame.blend\n";

int file_exist (char *filename) {
	FILE * pFile;
	pFile = fopen(filename , "r");
	if (pFile == NULL) return 0;
	else {fclose(pFile); return 1;}
}

void replace_char(char *text, char a, char b) {
	while (*text != 0) {
		if (*text == a) (*text) = b;
		text++;
	}
}

void remove_char(char *text, char a) {
	char * pT = text;
	while (*pT != 0) {
		(*text) = (*pT);
		if (*pT != a) text++;
		pT++;
	}
	(*text) = (*pT);
}

void printHelp() {
	printf("This is the blenderplayer launcher, writted by Robert Planas. (Free for non-comercial use)\n\n");
	printf("-help or -h\tShow this dialog.\n");
	printf("-g\t\tGenerate a config file in the current directory. (Overwrite if exists)\n\n");
	printf("Remember to use -h on the blenderplayer to see the aviable commands.\n\n");
}

void generateConfigFileText() {
	char * code = default_config_file;
	
	FILE *f = fopen("config.txt", "w");
	if (f == NULL) {perror("Error trying to write the config file. \n"); printf("So, it will be print here: \n\n%s", code);}
	else fprintf(f, "%s", code);
}

void parseConfig(FILE *pFile, char * command, char * filename, char * exepath) {
	typedef enum {ARGUMENT, VALUE,	__IGNORE__} etype;
	char c, n = 0;
	etype mode = ARGUMENT;
	char arg[ARG_SIZE] = {0}, val[VAL_SIZE] = {0};
	
	fseek(pFile, 0, SEEK_END);
	int length = ftell(pFile);
	rewind(pFile);
	
	size_t s; for (s=0;s<=length-1;s++) {
	c = fgetc (pFile);
	
	if (c=='\n') {
		val[n] = '\0';
		int val_p = n-1;
		mode=ARGUMENT; n=0;
		
		//Commands
		#ifdef WIN32
		if (strcmp(arg, "player")==0) {
			strncat(exepath, val, val_p);
			if (VERBOSE) printf("EXEPATH (player): %s\n", exepath);
		}
		#else
		if (strcmp(arg, "player")==0) {
			strncat(command, val, val_p);
			if (VERBOSE) printf("COMMAND (player): %s\n", command);
		}	
		#endif
		if (strcmp(arg, "blend")==0) {
			strncat(filename, val, val_p);
			if (VERBOSE) printf("FILENAME (blend): %s\n", filename);
		}
		
		if (strcmp(arg, "resolution")==0) {			
			strncat(command, " -w ", 4);
			strncat(command, val, val_p);
			if (VERBOSE) printf("COMMAND (resolution): %s\n", command);
		}
		if (strcmp(arg, "stereomode")==0) {
			strncat(command, " -s ", 4);
			strncat(command, val, val_p);
			if (VERBOSE) printf("COMMAND (stereomode): %s\n", command);
		} 
		if (strcmp(arg, "anti-aliasing")==0) {
			strncat(command, " -m ", 4);
			strncat(command, val, val_p);
			if (VERBOSE) printf("COMMAND (anti-aliasing): %s\n", command);
		}
		
		//If you enable space offset then remember to enable this to.
		//if (strcmp(arg, "resolution")==0)	{strcat(command, " -w "); char * r = strtok (val,"x"); strcat(command, r); 
		//					 r = strtok (NULL,"x"); strcat(command, " "); strcat(command, r); } //Warning!
		
		if (strcmp(arg, "fullscreen")==0 && strncmp(val, "true", 4)==0) {
			strncat(command, " -f", 3);
			if (VERBOSE) printf("COMMAND (fullscreen): %s\n", command);
		}
		if (strcmp(arg, "debug")==0 && strncmp(val, "true", 4)==0) {
			strncat(command, " -d", 3);
			if (VERBOSE) printf("COMMAND (debug): %s\n", command);		
		}
		
		
		if (strcmp(arg, "fixedtime")==0 && strncmp(val, "true", 4)==0) strcat(command, " -g fixedtime = 1");
		if (strcmp(arg, "nomipmap")==0 && strncmp(val, "true", 4)==0) strcat(command, " -g nomipmap = 1");
		if (strcmp(arg, "show_framerate")==0 && strncmp(val, "true", 4)==0) strcat(command, " -g show_framerate = 1");
		if (strcmp(arg, "show_properties")==0 && strncmp(val, "true", 4)==0) strcat(command, " -g show_properties = 1");
		if (strcmp(arg, "show_profile")==0 && strncmp(val, "true", 4)==0) strcat(command, " -g show_profile = 1");
		if (strcmp(arg, "blender_material")==0 && strncmp(val, "true", 4)==0) strcat(command, " -g blender_material = 1");
		if (strcmp(arg, "ignore_deprecation_warnings")==0 && strncmp(val, "false", 4)==0) strcat(command, " -g ignore_deprecation_warnings = 0");
		
		if (strcmp(arg, "exec")==0 && strcmp(val, "")!=0) {strcpy(command, val); strcpy(filename, ""); break;}
		
		//Clean
		memset(arg, 0, sizeof(arg));
		memset(val, 0, sizeof(val));
		continue;
	}
		if (mode == __IGNORE__) continue;
	
	//Select modes
	if (c=='#') {mode = __IGNORE__; continue;}
	if (c==':') {mode = VALUE; n=0; fseek(pFile, 1, SEEK_CUR); continue;}
	if (mode==ARGUMENT) arg[n] = c;
	if (mode==VALUE) val[n] = c;
	
	++n; 
	}
	if (VERBOSE) printf("COMMAND (ALL): %s\n", command);
}

char * findPlayerPath(char ** filepaths) {
	while (*filepaths != NULL) {
		char * path = (*filepaths);
		if (file_exist(path)) return path;
		filepaths++;
	}
	return NULL;
}

#if defined(_WIN32) || defined(WIN32)
void getCommand(char * command, char * filename, char * exepath) {
	char abspath[VAL_SIZE];
	char quoted[VAL_SIZE];
	DWORD win_filename = GetModuleFileName(NULL, abspath, VAL_SIZE);
	char * n = strrchr(abspath, '\\');
	(*(++n)) = 0;
	replace_char(abspath, '\\', '/');
	strcat(quoted, command);
	sprintf(quoted, "\"%s%s\"\0", abspath, exepath);
	strcpy(abspath, quoted);
	remove_char(abspath, '"');

	if (file_exist(abspath) == 0) {
		TCHAR pf[MAX_PATH], pf86[MAX_PATH];
		SHGetSpecialFolderPath(NULL, pf, CSIDL_PROGRAM_FILES, FALSE);
		SHGetSpecialFolderPath(NULL, pf86, CSIDL_PROGRAM_FILESX86, FALSE);
		replace_char(pf, '\\', '/'); replace_char(pf86, '\\', '/');
		strcat(pf, "/Blender Foundation/Blender/blenderplayer.exe");
		strcat(pf86, "/Blender Foundation/Blender/blenderplayer.exe");
		char ** paths = (char *[]) {pf, pf86, "engine/Blender/blenderplayer.exe", NULL};
		char ** paths = (char *[]) {NULL};
		
		char * path = findPlayerPath(paths);

		if (path == NULL) {

			MessageBox(NULL, "The blender player is missing. You can download a blender player form http://www.blender.org", launcher_name, MB_OK);
			return;
		}
		sprintf(quoted, "\"%s\"\0", path);
	}
	
	strcat(quoted, command);
	sprintf(command, "\"%s%s\"\0", quoted, filename);

	strcat(command, filename);
}
#else
void getCommand(char * command, char * filename, char * exepath) {
	return;
}
#endif

int main(int argc, char * argv[])
{
	// --- handle arguments -------------------------------
	launcher_name = argv[0];
	if (argc>1) {
		if(strcmp(argv[1],"--help") || strcmp(argv[1],"-h")==0) printHelp();
		if(strcmp(argv[1],"-g")==0) generateConfigFileText();
		return 0;
	}	
	
	// --- normal use -------------------------------------------
	 #ifdef WIN32
	 	char command[COM_SIZE] = "";
	 #else
		char command[COM_SIZE] = "./";
	 #endif
	 char filename[VAL_SIZE] = {0};
	 char exepath[VAL_SIZE] = {0};
	 
	 FILE * pFile;
	 pFile = fopen ("config.txt" , "r");
	 if (pFile == NULL) {perror ("Error opening config file. Try -help or -h\n"); return 0;} 
	 else {
		parseConfig(pFile, command, filename, exepath);
		fclose (pFile);
	}
	
	//printf("COMMAND: %s FILENAME: %s EXEPATH: %s\n", command, filename, exepath);
	printf("COMMAND: %s \nFILENAME: %s\n", command, filename);
	
	getCommand(command, filename, exepath);
	//printf("Command: %s\n", command);
	system(command);
	 
	return 0;
}
