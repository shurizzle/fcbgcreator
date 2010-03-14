/* 
 * Useful function for main program
 * DEVELOPER: shura, member of HUF, see https://hackers-uf.org/
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. 
 *
 */

#ifndef _UTILS_H
#define _UTILS_H

#define	MAX(a,b)	((a)>(b)?(a):(b))
typedef struct cowlist cowlist;
typedef struct rgb rgb;

struct cowlist {
	int size;
	char ** cowfile;
};

struct rgb{
	int r;
	int g;
	int b;
};

extern char * strdup (char *);
extern char * get_system (char *);
extern int * get_dims (char *);
extern int * get_screen_dims (void);
extern int create_im (char *, char *, int *, int *, rgb, rgb);
extern void show_list (void);
extern cowlist get_list (void);
extern void freecl (cowlist);
extern void cowfile_exist (char *);
extern int hex_to_X (char *, char);
extern rgb hex2rgb (char *);
extern rgb convrgb (char *);
extern void help (void);

#endif
