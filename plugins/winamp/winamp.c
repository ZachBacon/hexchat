/********************* Wacup Plugin 0.3******************************
 *
 *   Distribution: GPL
 *
 *   Originally written by: Leo - leo.nard@free.fr
 *   Modified by: SilvereX - SilvereX@karklas.mif.vu.lt
 *   Modified again by: Derek Buitenhuis - daemon404@gmail.com
 *   Modified yet again by: Berke Viktor - berkeviktor@aol.com
 *   Modified yet again by: Zach Bacon - zachbacon@vba-m.com
 *********************************************************************/

#include "windows.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "hexchat-plugin.h"

#define PLAYING 1
#define PAUSED 3

static hexchat_plugin *ph;   /* plugin handle */

static int
Wacup(char *word[], char *word_eol[], void *userdata)
{
	HWND hwndWacup = FindWindowW(L"Winamp V1.x",NULL);

	if (hwndWacup)
	{
		if (!_stricmp("PAUSE", word[2]))
		{
			if (SendMessage(hwndWacup,WM_USER, 0, 104))
			{
				SendMessage(hwndWacup, WM_COMMAND, 40046, 0);
			
				if (SendMessage(hwndWacup, WM_USER, 0, 104) == PLAYING)
					hexchat_printf(ph, "Wacup: playing");
				else
					hexchat_printf(ph, "Wacup: paused");
			}
		}
		else if (!_stricmp("STOP", word[2]))
		{
			SendMessage(hwndWacup, WM_COMMAND, 40047, 0);
			hexchat_printf(ph, "Wacup: stopped");
		}
		else if (!_stricmp("PLAY", word[2]))
		{
			SendMessage(hwndWacup, WM_COMMAND, 40045, 0);
			hexchat_printf(ph, "Wacup: playing");
		}
		else if (!_stricmp("NEXT", word[2]))
		{
			SendMessage(hwndWacup, WM_COMMAND, 40048, 0);
			hexchat_printf(ph, "Wacup: next playlist entry");
		}
		else if (!_stricmp("PREV", word[2]))
		{
			SendMessage(hwndWacup, WM_COMMAND, 40044, 0);
			hexchat_printf(ph, "Wacup: previous playlist entry");
		}
		else if (!_stricmp("START", word[2]))
		{
			SendMessage(hwndWacup, WM_COMMAND, 40154, 0);
			hexchat_printf(ph, "Wacup: playlist start");
		}
		else if (!word_eol[2][0])
		{
			wchar_t wcurrent_play[2048];
			char *current_play, *p;
			int len = GetWindowTextW(hwndWacup, wcurrent_play, G_N_ELEMENTS(wcurrent_play));

			current_play = g_utf16_to_utf8 (wcurrent_play, len, NULL, NULL, NULL);
			if (!current_play)
			{
				hexchat_print (ph, "Wacup: Error getting song information.");
				return HEXCHAT_EAT_ALL;
			}

			if (strchr(current_play, '-'))
			{
				/* Remove any trailing text and whitespace */
				p = current_play + strlen(current_play) - 8;
				while (p >= current_play)
				{
					if (!_strnicmp(p, "- Wacup", 8))
						break;
					p--;
				}

				if (p >= current_play)
					p--;

				while (p >= current_play && *p == ' ')
					p--;
				*++p = '\0';

				/* Ignore any leading track number */
				p = strstr (current_play, ". ");
				if (p)
					p += 2;
				else
					p = current_play;

				if (*p != '\0')
					hexchat_commandf (ph, "me is now playing: %s", p);
				else
					hexchat_print (ph, "Wacup: No song information found.");
				g_free (current_play);
			}
			else
				hexchat_print(ph, "Wacup: Nothing being played.");
		}
		else
			hexchat_printf(ph, "Usage: /Wacup [PAUSE|PLAY|STOP|NEXT|PREV|START]\n");
	}
	else
	{
		hexchat_print(ph, "Wacup not found.\n");
	}
	return HEXCHAT_EAT_ALL;
}

int
hexchat_plugin_init(hexchat_plugin *plugin_handle,
					  char **plugin_name,
					  char **plugin_desc,
					  char **plugin_version,
					  char *arg)
{
	/* we need to save this for use with any hexchat_* functions */
	ph = plugin_handle;

	*plugin_name = "Wacup";
	*plugin_desc = "Wacup plugin for HexChat";
	*plugin_version = "0.6";

	hexchat_hook_command (ph, "Wacup", HEXCHAT_PRI_NORM, Wacup, "Usage: /Wacup [PAUSE|PLAY|STOP|NEXT|PREV|START] - control Wacup or show what's currently playing", 0);
   	hexchat_command (ph, "MENU -ishare\\music.png ADD \"Window/Display Current Song (Wacup)\" \"Wacup\"");

	hexchat_print (ph, "Wacup plugin loaded\n");

	return 1;	   /* return 1 for success */
}

int
hexchat_plugin_deinit(void)
{
	hexchat_command (ph, "MENU DEL \"Window/Display Current Song (Wacup)\"");
	hexchat_print (ph, "Wacup plugin unloaded\n");
	return 1;
}
