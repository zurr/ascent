// $Id: svn_revision.h 1484 2007-09-11 23:05:34Z LordDraike $

#ifndef _SVN_REVISION_H
#define _SVN_REVISION_H

static const char * REVISION_TEXT = "$Rev: 1484 $";

inline int g_getRevision()
{
	const char * p = REVISION_TEXT + 6;
	return atoi(p);
}

//////////////////
#endif