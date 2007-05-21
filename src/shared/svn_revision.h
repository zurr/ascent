// $Id$

#ifndef _SVN_REVISION_H
#define _SVN_REVISION_H

static char * REVISION_TEXT = "$Rev$";
static int SVN_REVISION = 0;

inline void g_initRevision()
{
	char * p = REVISION_TEXT + 1;
	SVN_REVISION = atoi(p);
}

#endif
