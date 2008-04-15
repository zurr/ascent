#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char * ASCENT_BRANCH = "4.3-TRUNK";
int g_revision = 0;
int g_oldRevision = -1;

int main(int argc, char* argv[])
{
	FILE * pFile = fopen("../.svn/entries", "r");
	FILE * pTagFile = fopen("../../ascent-tag", "r");
	if( pTagFile != NULL )
	{
		char str[1024];
		fgets(str, 1024, pTagFile);
		ASCENT_BRANCH = (char*)malloc(strlen(str)+1);
		memcpy(ASCENT_BRANCH, str, strlen(str)+1);
		fclose( pTagFile );
	}
	printf("SVN Revision Extractor\n");
	printf("Written by Burlex, 2008/02/20\n");
	printf("Branch: %s\n", ASCENT_BRANCH);

	if( pFile == NULL )
	{
		printf("Could not open entries file. This may not be a SVN checkout? Assuming revision 0.\n");
		return 1;
	}
	else
	{
		char * p;
		char * q;
		char * buffer;
		long sz;

		fseek( pFile, 0, SEEK_END );
		sz = ftell( pFile );
		fseek( pFile, 0, SEEK_SET );

		buffer = (char*)malloc( sz );
		if( buffer != NULL )
		{
			fread( buffer, sz, 1, pFile );

			p = strstr( buffer, "dir" );
			if( p != NULL )
			{
				while( !isdigit( *p ) )
					++p;

				q = p;
				while( isdigit( *q ) )
					++q;

				*q = 0;

				g_revision = atoi( p );
				printf("Revision found: %u\n", g_revision);
			}

			free( buffer );
		}

		fclose ( pFile );
		printf("Opening existing file...\n");
		
		pFile = fopen("svn_revision.h", "r");
		if( pFile != NULL )
		{
			fseek( pFile, 0, SEEK_END );
			sz = ftell( pFile );
			fseek( pFile, 0, SEEK_SET );

			buffer = (char*)malloc( sz );
			if( buffer != NULL )
			{
				fread( buffer, sz, 1, pFile );
				p = strstr( buffer, "BUILD_REVISION =");
				if( p != NULL )
				{
					while( !isdigit( *p ) )
						++p;

					q = p;
					while( isdigit( *q ) )
						++q;

					*q = 0;

					g_oldRevision = atoi( p );
				}

				free( buffer );
			}

			fclose( pFile );
		}
	}

	if( g_oldRevision == g_revision )
	{
		printf("Revisions match exiting file. Skipping.\n");
		return 0;
	}

	printf("Writing output file...\n");
	
	pFile = fopen( "svn_revision.h", "w" );
	if( pFile == NULL )
	{
		printf("Output file could not be opened.\n");
		return 2;
	}
	else
	{
		fprintf(pFile, "// This file was automatically generated by the SVN revision extractor.\n");
		fprintf(pFile, "// There is no need to modify it.\n");
		fprintf(pFile, "\n");
		fprintf(pFile, "#ifndef SVN_REVISION_H\n");
		fprintf(pFile, "#define SVN_REVISION_H\n\n");
		fprintf(pFile, "static const char * BUILD_TAG = \"%s\";\n", ASCENT_BRANCH);
		fprintf(pFile, "static int BUILD_REVISION = %u;\n\n", g_revision);
		fprintf(pFile, "#endif\t\t // SVN_REVISION_H\n\n");
		fflush( pFile );
		fclose( pFile );

		printf("Output file written. Exiting cleanly.\n");
	}
	return 0;
}