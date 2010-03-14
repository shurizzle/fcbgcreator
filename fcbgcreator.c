/* 
 * Background creator
 * DEVELOPER: shura, member of HUF, see https://hackers-uf.org/
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <gdfontl.h>

#include "utils.h"

int
main (int argc, char ** argv)
{
	char * penis;
	char * path = "";
	char * cowfile = "";
	char * cmd;
    char * phrase = "";
	int * sub;
	int * res;
    int c, option_index;

    const   struct option long_options[] =
    {
        {"cowfile", required_argument, 0, 'c'},
        {"backrgb", required_argument, 0, 'b'},
        {"backhex", required_argument, 0, 'B'},
        {"forergb", required_argument, 0, 'f'},
        {"forehex", required_argument, 0, 'F'},
        {"help", no_argument, 0, 'h'},
        {"listfiles", no_argument, 0, 'l'},
        {"input", required_argument, 0, 'i'},
        {"output", required_argument, 0, 'o'}
    };

	rgb back = {0, 0, 0}, fore = {255, 255, 255};

	while (1)
	{
        option_index = 0;
        c = getopt_long (argc, argv, "c:b:B:f:F:hli:o:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case 'c':
                cowfile = strdup (optarg);
            break;
            case 'b':
                back = convrgb (optarg);
            break;
            case 'B':
                back = hex2rgb (optarg);
            break;
            case 'f':
                fore = convrgb (optarg);
            break;
            case 'F':
                fore = hex2rgb (optarg);
            break;
            case 'i':
                phrase = strdup (optarg);
            break;
            case 'o':
                path = strdup (optarg);
            break;
            case 'l':
                show_list ();
            break;
            case 'h':
                help ();
            break;
            case '?':
                exit (0);
            break;
            default:
                puts ("Options not valid, show helps with '-h' or '--help'");
                exit (0);
            break;
        }
	}

	if (strcmp (phrase, "") == 0)
    {
        cmd = (char *) malloc (16 * sizeof (char));
	    strcpy (cmd, "fortune | cowsay");
    }
    else
    {
        cmd = (char *) malloc ((16 + strlen (phrase) * sizeof (char)));
        sprintf (cmd, "echo \"%s\" | cowsay", phrase);
    }

	if (strcmp (cowfile, "") != 0)
	{
		cowfile_exist (cowfile);
		cmd = (char *) realloc (cmd, (strlen (cmd) + 4 + strlen (cowfile)) *  sizeof (char));
		sprintf (cmd, "%s -f %s", cmd, cowfile);
	}

	penis = (char *) malloc (0);

	res = get_screen_dims ();
	do
	{
		free (penis);
		penis = get_system (cmd);
		sub = get_dims (penis);
	}
	while ((sub[1] * gdFontGetLarge ()->h) > res[1]);
	
	if (strcmp (path, "") == 0)
	{
		path = (char *) malloc (512 * sizeof (char));
		sprintf (path, "%s/.fcground.jpeg", getenv ("HOME"));
	}

	create_im (path, penis, res, sub, back, fore);

	free (penis);
	free (sub);
	free (res);
	free (cmd);

	return 0;
}
