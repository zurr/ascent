// $Id$

#ifndef _SVN_REVISION_H
#define _SVN_REVISION_H

static char * REVISION_TEXT = "$Rev$";

inline int g_getRevision()
{
	char * p = REVISION_TEXT + 6;
	return atoi(p);
}
///////////////////////////////
#endif
