#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <map>
#include <vector>
#include <set>

#include "adt.h"
#include "mpq_libmpq.h"

bool model::Load(const char * filename)
{
	/* fix filename - change .mdx -> .m2 */
	char * temp_filename = strdup(filename);
	char * p = strrchr(temp_filename, '.');
	*(p+1) = 'm';
	*(p+2) = '2';
	*(p+3) = '\0';
	printf("Loading model: %s\n", temp_filename);

	MPQFile f(temp_filename);
	if(f.isEof())
	{
		free(temp_filename);
		printf("Error! Could not open %s\n", temp_filename);
		return false;
	}
	free(temp_filename);
	
	f.read(&header, sizeof(ModelHeader));
	
	/* TODO: calculate bounding box */
	return true;
}