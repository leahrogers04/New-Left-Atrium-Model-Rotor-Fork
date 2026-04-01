#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <math.h>


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
 The file is written to:
 ../NodesMuscles/bin/<NodesMusclesFileName>_<timestamp>.bin

 Binary layout (version 2):
 - version
 - NumberOfNodes
 - per node: type(int), position(float4), muscle[MUSCLES_PER_NODE](int)
 - NumberOfMuscles
 - per muscle: type(int), nodeA(int), nodeB(int), naturalLength(float)
*/
void saveBinary()
{
	FILE *binaryFile;
	char fileName[512];

	if(Node == NULL || Muscle == NULL)
	{
		printf("\n\n Node/Muscle data is not loaded.\n");
		snprintf(BinarySaveStatusMessage, sizeof(BinarySaveStatusMessage), "Binary save failed: Node/Muscle data is not loaded.");
		return;
	}

	if(NodesMusclesFileName[0] == '\0')
	{
		printf("\n\n NodesMusclesFileName is empty.\n");
		snprintf(BinarySaveStatusMessage, sizeof(BinarySaveStatusMessage), "Binary save failed: NodesMusclesFileName is empty.");
		return;
	}

	// Ensure muscle types reflect the current node typing before export.
	if(!setMuscleTypes())
	{
		snprintf(BinarySaveStatusMessage, sizeof(BinarySaveStatusMessage), "Binary save failed: Unknown node type in setMuscleTypes().");
		return;
	}

	std::string timeStamp = getTimeStamp();
	strcpy(fileName, "../NodesMuscles/bin/");
	strcat(fileName, NodesMusclesFileName);
	strcat(fileName, "_");
	strcat(fileName, timeStamp.c_str());
	strcat(fileName, ".bin");

	binaryFile = fopen(fileName, "wb");
	if(binaryFile == NULL)
	{
		printf("\n\n Could not create binary file %s.\n", fileName);
		snprintf(BinarySaveStatusMessage, sizeof(BinarySaveStatusMessage), "Binary save failed: Could not create output file.");
		return;
	}

	// Header for basic validation by future readers.
	int version = 1; // Increment this if we change the binary layout in a way that is not backwards compatible.
	fwrite(&version, sizeof(int), 1, binaryFile);

	// Save nodes.
	fwrite(&NumberOfNodes, sizeof(int), 1, binaryFile);
	for(int i = 0; i < NumberOfNodes; i++)
	{
		fwrite(&Node[i].type, sizeof(int), 1, binaryFile);
		fwrite(&Node[i].position, sizeof(float4), 1, binaryFile);
		fwrite(Node[i].muscle, sizeof(int), MUSCLES_PER_NODE, binaryFile);
	}

	// Save muscles.
	fwrite(&NumberOfMuscles, sizeof(int), 1, binaryFile);
	for(int i = 0; i < NumberOfMuscles; i++)
	{
		float dx = Node[Muscle[i].nodeA].position.x - Node[Muscle[i].nodeB].position.x;
		float dy = Node[Muscle[i].nodeA].position.y - Node[Muscle[i].nodeB].position.y;
		float dz = Node[Muscle[i].nodeA].position.z - Node[Muscle[i].nodeB].position.z;
		float naturalLength = sqrtf(dx*dx + dy*dy + dz*dz);

		fwrite(&Muscle[i].type, sizeof(int), 1, binaryFile);
		fwrite(&Muscle[i].nodeA, sizeof(int), 1, binaryFile);
		fwrite(&Muscle[i].nodeB, sizeof(int), 1, binaryFile);
		fwrite(&naturalLength, sizeof(float), 1, binaryFile);
	}

	fclose(binaryFile);
	printf("\n Binary file saved: %s\n", fileName);
	snprintf(BinarySaveStatusMessage, sizeof(BinarySaveStatusMessage), "Binary saved successfully.");
}

#endif // UTILITIES_H
