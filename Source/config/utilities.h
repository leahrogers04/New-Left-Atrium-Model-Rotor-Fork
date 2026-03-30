#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>


/*
 This function is used to clear the print buffer.
*/
void clearStdin()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        /* discard characters */
    }
}

/*
 This function returns a timestamp in M-D-Y-H.M.S format.
 This is use so each file that is created has a unique name. 
 Note: You cannot create more than one file in a second or you will over write the previous file.
*/
std::string getTimeStamp()
{
	// Want to get a time stamp string representing current date/time, so we have a
	// unique name for each video/screenshot taken.
	time_t t = time(0); 
	struct tm * now = localtime( & t );
	int month = now->tm_mon + 1, day = now->tm_mday, year = now->tm_year, 
				curTimeHour = now->tm_hour, curTimeMin = now->tm_min, curTimeSec = now->tm_sec;

	std::stringstream smonth, sday, syear, stimeHour, stimeMin, stimeSec;

	smonth << month;
	sday << day;
	syear << (year + 1900); // The computer starts counting from the year 1900, so 1900 is year 0. So we fix that.
	stimeHour << curTimeHour;
	stimeMin << curTimeMin;
	stimeSec << curTimeSec;
	std::string timeStamp;

	if (curTimeMin <= 9)
	{
		timeStamp = smonth.str() + "-" + sday.str() + "-" + syear.str() + '_' + stimeHour.str() + ".0" + stimeMin.str() + 
					"." + stimeSec.str();
	}
	else
	{		
		timeStamp = smonth.str() + "-" + sday.str() + '-' + syear.str() + "_" + stimeHour.str() + "." + stimeMin.str() +
					"." + stimeSec.str();
	}

	return timeStamp;
}

/*
 Saves node/muscle attributes to one binary file using a simple fwrite layout.
 The file is named after the raw directory used for input:
 ../NodesMuscles/raw/<NodesMusclesFileName>/<NodesMusclesFileName>.bin
*/
void saveBinary()
{
	FILE *binaryFile;
	char fileName[512];

	if(Node == NULL || Muscle == NULL)
	{
		printf("\n\n Node/Muscle data is not loaded.\n");
		return;
	}

	if(NodesMusclesFileName[0] == '\0')
	{
		printf("\n\n NodesMusclesFileName is empty.\n");
		return;
	}

	strcpy(fileName, "../NodesMuscles/raw/");
	strcat(fileName, NodesMusclesFileName);
	strcat(fileName, "/");
	strcat(fileName, NodesMusclesFileName);
	strcat(fileName, ".bin");

	binaryFile = fopen(fileName, "wb");
	if(binaryFile == NULL)
	{
		printf("\n\n Could not create binary file %s.\n", fileName);
		return;
	}

	// Header for basic validation by future readers.
	char magic[8] = {'S', 'V', 'T', 'B', 'I', 'N', '1', '\0'};
	int version = 1;
	fwrite(magic, sizeof(char), 8, binaryFile);
	fwrite(&version, sizeof(int), 1, binaryFile);

	// Node record: type, position, color, connected muscle list.
	fwrite(&NumberOfNodes, sizeof(int), 1, binaryFile);
	for(int i = 0; i < NumberOfNodes; i++)
	{
		fwrite(&Node[i].type, sizeof(int), 1, binaryFile);
		fwrite(&Node[i].position, sizeof(float4), 1, binaryFile);
		fwrite(&Node[i].color, sizeof(float4), 1, binaryFile);
		fwrite(Node[i].muscle, sizeof(int), MUSCLES_PER_NODE, binaryFile);
	}

	// Muscle record: type, connected nodes, color.
	fwrite(&NumberOfMuscles, sizeof(int), 1, binaryFile);
	for(int i = 0; i < NumberOfMuscles; i++)
	{
		fwrite(&Muscle[i].type, sizeof(int), 1, binaryFile);
		fwrite(&Muscle[i].nodeA, sizeof(int), 1, binaryFile);
		fwrite(&Muscle[i].nodeB, sizeof(int), 1, binaryFile);
		fwrite(&Muscle[i].color, sizeof(float4), 1, binaryFile);
	}

	fclose(binaryFile);
	printf("\n Binary file saved: %s\n", fileName);
}

#endif // UTILITIES_H
