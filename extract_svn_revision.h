#!/bin/sh

# SVN Revision Extractor for Unix systems
# Written by Burlex, 2008/02/20

echo "// This file was automatically generated by the SVN revision extractor." > svn_revision.h
echo "// There is no need to modify it." >> svn_revision.h
echo "" >> svn_revision.h
echo "#ifndef SVN_REVISION_H" >> svn_revision.h
echo "#define SVN_REVISION_H" >> svn_revision.h
echo "" >> svn_revision.h
echo "static const char * BUILD_TAG = \"`cat ascent-tag`\";" >> svn_revision.h
echo "static int BUILD_REVISION =" `svn info | grep Revision | cut -d' ' -f2`";" >> svn_revision.h
echo "" >> svn_revision.h
echo "#endif         // SVN_REVISION_H" >> svn_revision.h
echo "" >> svn_revision.h

mv svn_revision.h src/ascent-shared

echo "1"
