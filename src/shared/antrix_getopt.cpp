/****************************************************************************
 *
 * General Object Type File
 * Copyright (c) 2007 Team Ascent
 *
 * This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0
 * License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to Creative Commons,
 * 543 Howard Street, 5th Floor, San Francisco, California, 94105, USA.
 *
 * EXCEPT TO THE EXTENT REQUIRED BY APPLICABLE LAW, IN NO EVENT WILL LICENSOR BE LIABLE TO YOU
 * ON ANY LEGAL THEORY FOR ANY SPECIAL, INCIDENTAL, CONSEQUENTIAL, PUNITIVE OR EXEMPLARY DAMAGES
 * ARISING OUT OF THIS LICENSE OR THE USE OF THE WORK, EVEN IF LICENSOR HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 *
 */

#include "Common.h"
#include "antrix_getopt.h"

int arg_counter = 1;
char antrix_optarg[514];
int antrix_getopt_long_only(int ___argc, char *const *___argv, const char *__shortopts, const struct antrix_option *__longopts, int *__longind)
{
	// burlex todo: handle the shortops, at the moment it only works with longopts.

	if (___argc == 1 || arg_counter == ___argc)			// No arguments (apart from filename)
		return -1;

	const char * opt = ___argv[arg_counter];
//	int return_val = 0;

	// if we're not an option, return an error.
	if (strnicmp(opt, "--", 2) != 0)
		return 1;
	else
		opt += 2;


	// parse argument list
	int i = 0;
	for (; __longopts[i].name != 0; ++i)
	{
		if (!strnicmp(__longopts[i].name, opt, strlen(__longopts[i].name)))
		{
			// woot, found a valid argument =)
			char * par = 0;
			if ((arg_counter + 1) != ___argc)
			{
				// grab the parameter from the next argument (if its not another argument)
				if (strnicmp(___argv[arg_counter+1], "--", 2) != 0)
				{
					arg_counter++;		// Trash this next argument, we won't be needing it.
					par = ___argv[arg_counter];
				}
			}			

			// increment the argument for next time
			arg_counter++;

			// determine action based on type
			if (__longopts[i].has_arg == antrix_required_argument)
			{
				if(!par)
					return 1;

				// parameter missing and its a required parameter option
				if(__longopts[i].flag)
				{
					*__longopts[i].flag = atoi(par);
					return 0;
				}
			}

			// store argument in optarg
			if (par)
				strncpy(antrix_optarg, par, 514);

			if (__longopts[i].flag != 0)
			{
				// this is a variable, we have to set it if this argument is found.
				*__longopts[i].flag = 1;
				return 0;
			}
			else
			{
				if (__longopts[i].val == -1 || par == 0)
					return 1;

				return __longopts[i].val;
			}			
			break;
		}
	}

	// return 1 (invalid argument)
	return 1;
}

