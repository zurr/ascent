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

#ifndef _ANTRIX_GETOPT_H
#define _ANTRIX_GETOPT_H

/* getopt() wrapper */
#define antrix_no_argument            0
#define antrix_required_argument      1
#define antrix_optional_argument      2
struct antrix_option
{
	char *name;
	int has_arg;
	int *flag;
	int val;
};
extern char antrix_optarg[514];
int antrix_getopt_long_only (int ___argc, char *const *___argv, const char *__shortopts, const struct antrix_option *__longopts, int *__longind);

#endif
