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

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <X11/Intrinsic.h>
#include <gd.h>
#include <gdfontl.h>
#include <ctype.h>

#include "utils.h"

char *
strdup (char * str)
{
    char * res = (char *) malloc ((strlen (str) + 1) * sizeof (char));
    strcpy (res, str);
    return res;
}

char *
get_system (char * command)
{
	int		br[2] = {-1, -1};
	int		childpid;
	int		len = 0;
	char	ch[1];
	char	*	result;

	result = (char *) malloc (512 * sizeof (char));

	if (pipe (br) < 0)
	{
		return "Error";
	}

	#define	READ	br[0]
	#define	WRITE	br[1]

	if ((childpid = fork ()) < 0)
	{
		return "Error";
	}
	
	if (childpid == 0)
	{
		close (READ);
		
		dup2 (WRITE, 1);
		close (WRITE);

		system (command);
	}

	if (childpid > 0)
	{
		close (WRITE);

		wait ((int *) 0);

		while (read (READ, ch, 1))
		{
			if ((len + 1) % 512)
			{
				result = (char *) realloc (result, (len + 512 + 1) * sizeof (char));
			}
			result[len++] = ch[0];
		}

		close (READ);
	}
	result[len - 1] = '\0';
	result = realloc (result, len * sizeof (char));

	return result;
}

int *
get_dims (char * string)
{
	size_t  i;
	int		* ret;
	int		x = 0;

	ret = (int *) malloc (2 * sizeof (int));
	ret[0] = ret[1] = 0;
	for (i = 0; i < strlen (string); i++)
	{
		x++;
		if (string[i] == '\n')
		{
			ret[1] += 1;
			ret[0] = MAX(ret[0], x);
			x = 0;
		}
	}

	return ret;
}

int *
get_screen_dims ()
{
	int		* ret;
	Display	* display;
	Screen	* screen;

	ret = (int *) malloc (2 * sizeof (int));
	ret[0] = ret[1] = 0;

	display = XOpenDisplay (NULL);
	screen = XDefaultScreenOfDisplay (display);
	ret[0] = XWidthOfScreen (screen);
	ret[1] = XHeightOfScreen (screen);

	return ret;
}

int
create_im (char * name,
		   char * string,
		   int * res,
		   int * sub,
		   rgb backg,
		   rgb foreg)
{
	gdImagePtr	im;
	FILE		* background;
	int			act[2];

	int			back;
	int			fore;

	char		* buf;
	int			y = 0;
    size_t      i;

	act[0] = res[0] / 2 - (sub[0] * gdFontGetLarge ()->w) / 2;
	act[1] = res[1] / 2 - (sub[1] * gdFontGetLarge ()->h) / 2;

	buf = (char *) malloc (((int) get_screen_dims ()[1] / gdFontGetLarge ()->w) * sizeof (char));

	im = gdImageCreate (res[0], res[1]);

	back = gdImageColorAllocate (im, backg.r, backg.g, backg.b);
	fore = gdImageColorAllocate (im, foreg.r, foreg.g, foreg.b);

	for (i = 0; i < strlen (string); i++)
	{
		if (string[i] != '\n')
			buf[y++] = string[i];
		else
		{
			buf[y] = '\0';
			y = 0;
			gdImageString (im, gdFontGetLarge (), act[0], act[1], (unsigned char *) buf, fore);
			act[1] += gdFontGetLarge ()->h;
		}
	}

	buf[y] = '\0';
	gdImageString (im, gdFontGetLarge (), act[0], act[1], (unsigned char *) buf, fore);

	background = fopen (name, "wb");
	gdImageJpeg (im, background, -1);
	fclose (background);
	gdImageDestroy (im);
	return 1;
}

void
show_list ()
{
	char 	* list = get_system ("cowsay -l");
	size_t  i = 0;

	while (list[++i] != '\n');

	while (++i < strlen (list))
		putchar (list[i]);

	puts ("");

	exit (0);
}

cowlist
get_list ()
{
	char	* list = get_system ("cowsay -l");
	size_t  i = 0;
	int		len = 0;
	int		slen = 0;

	char 	** ret;
	char	* buf;

	cowlist	res;

	ret = (char **) malloc (50 * sizeof (char *));
	buf = (char *) malloc (512 * sizeof (char));

	while (list[++i] != '\n');

	while (++i < strlen (list))
	{
		if (list[i] == '\n' || list[i] == ' ')
		{
			if ((len + 1) % 50)
				ret = (char **) realloc (ret, (len + 50 + 1) * sizeof (char *));
			buf[slen++] = '\0';
			buf = (char *) realloc (buf, slen * sizeof (char));
			ret[len++] = buf;
			buf = (char *) malloc (512 * sizeof (char));
			slen = 0;
		}
		else
		{
			if ((slen + 1) % 50)
				buf = (char *) realloc (buf, (slen + 512 + 1) * sizeof (char *));
			buf[slen++] = list[i];
		}
	}
	
	if ((len + 1) % 50)
		ret = (char **) realloc (ret, (len + 50 + 1) * sizeof (char *));
	buf[slen] = '\0';
	buf = (char *) realloc (buf, strlen (buf) * sizeof (char));
	ret[len++] = buf;

	res.cowfile = ret;
	res.size = len;

	return res;
}

void
freecl (cowlist list)
{
	int i;

	for (i = 0; i < list.size; i++)
		free (list.cowfile[i]);
	free (list.cowfile);
}

void
cowfile_exist (char * cowfile)
{
	int i;
	cowlist cl = get_list ();

	for (i = 0; i < cl.size; i++)
	{
		if (strcmp (cl.cowfile[i], cowfile) == 0)
		{
			freecl (cl);
			return;
		}
	}

	freecl (cl);
	puts ("Cowfile doesn't exist");
	exit (-1);
}

int
hex_to_X (char * hexd,
          char X ) 
{
    unsigned int n = 0;
    char buf[2];
    int i, o;

    if (X == 'R' || X == 'r')
        o = 1;
    if (X == 'G' || X == 'g')
        o = 3;
    if (X == 'B' || X == 'b')
        o = 5;

    for (i = 0; i < 2; i++)
        buf[i] = hexd[i + o];

    sscanf (buf, "%x", &n);
    return n;
}

rgb
hex2rgb (char * hex)
{
    rgb res = {0, 0, 0};

    if (hex[0] == '#' || strlen (hex) == 7)
    {
        res.r = hex_to_X (hex, 'r');
        res.g = hex_to_X (hex, 'g');
        res.b = hex_to_X (hex, 'b');
    }

    return res;
}

rgb
convrgb (char * rgbs)
{
    size_t  i;
    char *  buf = (char *) calloc (4, sizeof (char));
    rgb res = {0, 0, 0};

    for (i = 0; rgbs[i] != ','; i++)
        sprintf (buf, "%s%c", buf, rgbs[i]);
    i++;
    res.r = atoi (buf);
    buf[0] = 0;
    for (; rgbs[i] != ','; i++)
        sprintf (buf, "%s%c", buf, rgbs[i]);
    i++;
    res.g = atoi (buf);
    buf[0] = 0;
    for (; i < strlen (rgbs); i++)
        sprintf (buf, "%s%c", buf, rgbs[i]);
    i++;
    res.b = atoi (buf);

    return res;
}

void
help ()
{
	puts ("Distributed under GPLv3 licence");
	puts ("-l                    - List all cowfiles");
	puts ("-f cowfile            - Select a cowfile");
	puts ("-backrgb R,G,B        - Select background color in rgb format");
    puts ("-backhex #hex         - Select background color in hex format");
	puts ("-forergb R,G,B        - Select foreground color in rgb format");
    puts ("-forehex #hex         - Select foreground color in hex format");
	puts ("-o /path/to/file.jpeg - Select an alternative output path");
    puts ("-i phrase             - Phrase to write into image");
	puts ("-h, --help            - Show this help");

	exit (0);
}
