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

 Binary layout (version 1):
 - version
 - NumberOfNodes
 - NumberOfMuscles
 - PulsePointNode
 - UpNode
 - FrontNode
 - per node: type(int), position(float4), muscle[MUSCLES_PER_NODE](int), color(float4)
 - per muscle: type(int), nodeA(int), nodeB(int), naturalLength(float), color(float4)
*/
void saveBinary()
{
	FILE *binaryFile;
	char fileName[512];

	// We cannot save unless both arrays are loaded in memory.
	if(Node == NULL || Muscle == NULL)
	{
		printf("\n\n Node/Muscle data is not loaded.\n");
		snprintf(BinarySaveStatusMessage, sizeof(BinarySaveStatusMessage), "Binary save failed: Node/Muscle data is not loaded.");
		return;
	}

	// Require a base file name so we can generate a proper output path.
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

	// Build output file path with timestamp suffix to avoid name collisions.
	std::string timeStamp = getTimeStamp();
	strcpy(fileName, "../NodesMuscles/bin/");
	strcat(fileName, NodesMusclesFileName);
	strcat(fileName, "_");
	strcat(fileName, timeStamp.c_str());
	strcat(fileName, ".bin");

	// Open output file in binary write mode.
	binaryFile = fopen(fileName, "wb");
	if(binaryFile == NULL)
	{
		printf("\n\n Could not create binary file %s.\n", fileName);
		snprintf(BinarySaveStatusMessage, sizeof(BinarySaveStatusMessage), "Binary save failed: Could not create output file.");
		return;
	}

	// Header for basic validation by future readers.
	int version = 1; // Keep this aligned with the binary readers in model/config.
	fwrite(&version, sizeof(int), 1, binaryFile);

	// Save counts and required orientation nodes.
	fwrite(&NumberOfNodes, sizeof(int), 1, binaryFile);
	fwrite(&NumberOfMuscles, sizeof(int), 1, binaryFile);
	fwrite(&PulsePointNode, sizeof(int), 1, binaryFile);
	fwrite(&UpNode, sizeof(int), 1, binaryFile);
	fwrite(&FrontNode, sizeof(int), 1, binaryFile);

	// Save nodes.
	for(int i = 0; i < NumberOfNodes; i++)
	{
		// Write node type for section classification.
		fwrite(&Node[i].type, sizeof(int), 1, binaryFile);
		// Write node position used to reconstruct geometry.
		fwrite(&Node[i].position, sizeof(float4), 1, binaryFile);
		// Write node-to-muscle connectivity map.
		fwrite(Node[i].muscle, sizeof(int), MUSCLES_PER_NODE, binaryFile);
		// Write node color so section coloring can be restored later.
		fwrite(&Node[i].color, sizeof(float4), 1, binaryFile);
	}

	// Save muscles information
	for(int i = 0; i < NumberOfMuscles; i++)
	{
		// Recompute rest length from node positions so geometry and mechanics stay in sync.
		float dx = Node[Muscle[i].nodeA].position.x - Node[Muscle[i].nodeB].position.x;
		float dy = Node[Muscle[i].nodeA].position.y - Node[Muscle[i].nodeB].position.y;
		float dz = Node[Muscle[i].nodeA].position.z - Node[Muscle[i].nodeB].position.z;
		float naturalLength = sqrtf(dx*dx + dy*dy + dz*dz);

		// Write muscle section type.
		fwrite(&Muscle[i].type, sizeof(int), 1, binaryFile);
		// Write both endpoint node ids.
		fwrite(&Muscle[i].nodeA, sizeof(int), 1, binaryFile);
		fwrite(&Muscle[i].nodeB, sizeof(int), 1, binaryFile);
		// Write natural length used by mechanics.
		fwrite(&naturalLength, sizeof(float), 1, binaryFile);
		// Write muscle color so section coloring can be restored later.
		fwrite(&Muscle[i].color, sizeof(float4), 1, binaryFile);
	}

	// Close file handle before reporting success.
	fclose(binaryFile);
	printf("\n Binary file saved: %s\n", fileName);
	snprintf(BinarySaveStatusMessage, sizeof(BinarySaveStatusMessage), "Binary saved: %s", fileName);
}

#endif // UTILITIES_H
