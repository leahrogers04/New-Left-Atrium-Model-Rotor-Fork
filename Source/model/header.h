#ifndef HEADER_H
#define HEADER_H

/*
 This file contains all include files, the #defines, structures and extern
 globals used in the simulation. All of the functions are prototyped here.
*/

// External include files
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <vector> //needed for VBOs

// Needed to make 
#include <cuda_runtime.h>

// OpenGL headers - GLAD must come BEFORE GLFW
#include "../include/glad/glad.h"
#include <GL/glu.h>
#include <GLFW/glfw3.h>

// ImGui headers - use quotes for local includes, not angle brackets
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

using namespace std;

// Cuda defines
#define BLOCKNODES 256
#define BLOCKMUSCLES 256
#define BLOCKCENTEROFMASS 512
#define FLOATMAX 3.4028235e+38f
#define INTMAX 2147483647

// Defines for terminal print
#define BOLD_ON  "\e[1m"
#define BOLD_OFF   "\e[m"

// Math defines.
#define PI 3.141592654
#define ASUMEZERO 0.0000001f

// Structure defines. 
// This sets how many muscle can be connected to a node.
#define MUSCLES_PER_NODE 20

// Structures
struct nodeAttributesStructure
{
    float4 position;
    float4 velocity;
    float4 force;
    float mass;
    float area;
    bool isBeatNode;
    float beatPeriod;
    float beatTimer;
    bool isFiring;
    bool isAblated;
    bool isDrawNode;
    float4 color;
    int muscle[MUSCLES_PER_NODE];
};

struct muscleAttributesStructure
{
    int nodeA;
    int nodeB;    
    int apNode;
    bool isOn;
    bool isEnabled;
    float timer;
    float mass;
    float naturalLength;
    float relaxedStrength;
    float compressionStopFraction;
    float conductionVelocity;
    float conductionDuration;
    float refractoryPeriod;
    float absoluteRefractoryPeriodFraction;
    float contractionStrength;
    float4 color;
};

struct simulationSwitchesStructure
{
    bool isPaused;
    bool isInAblateMode;
    bool isInEctopicBeatMode;
    bool isInEctopicEventMode;
    bool isInAdjustMuscleAreaMode;
    bool isInAdjustMuscleLineMode;
    bool isInFindNodeMode;
    bool isInFindMuscleMode;
    bool isInMouseFunctionMode;
    bool isRecording;
    bool ContractionisOn; 
    int ViewFlag; 
    int DrawNodesFlag; 
    int DrawFrontHalfFlag;
    bool nodesFound;
    int frontNodeIndex;
    int topNodeIndex;
    bool guiCollapsed;
};

// extern globals
extern int NumberOfNodes;
extern int NumberOfMuscles;
extern int NumberOfNodesInBachmannsBundle;

extern nodeAttributesStructure *Node;
extern nodeAttributesStructure *NodeGPU;

extern muscleAttributesStructure *Muscle;
extern muscleAttributesStructure *MuscleGPU;

extern int *BachmannsBundle;

extern simulationSwitchesStructure Simulation;

extern FILE* MovieFile;
extern unsigned char* Buffer;
extern int CaptureWidth, CaptureHeight;

extern dim3 BlockNodes, GridNodes;
extern dim3 BlockMuscles, GridMuscles;

extern cudaStream_t ComputeStream, MemoryStream;

extern GLuint SphereVBO, SphereIBO;
extern GLuint NumSphereVertices, NumSphereIndices;

extern int PulsePointNode;
extern int UpNode;
extern int FrontNode;

extern char ViewName[256];

extern float RefractoryPeriodAdjustmentMultiplier;
extern float MuscleConductionVelocityAdjustmentMultiplier;

extern int NodesMusclesFileOrPreviousRunsFile;
extern char NodesMusclesFileName[256];
extern char PreviousRunFileName[256];
extern float LineWidth;
extern float NodeRadiusAdjustment;
extern float NodePointSize;
extern float4 BackGround;

extern double BaseMuscleRefractoryPeriod;
extern double MuscleRefractoryPeriodSTD;
extern double BaseAbsoluteRefractoryPeriodFraction;
extern double AbsoluteRefractoryPeriodFractionSTD;
extern double BaseMuscleConductionVelocity;
extern double MuscleConductionVelocitySTD;
extern double BachmannsBundleMultiplier;
extern double BeatPeriod;
extern double PrintRate;
extern int DrawRate;
extern double Dt;
extern float4 ReadyColor;
extern float4 DepolarizingColor;
extern float4 RepolarizingColor;
extern float4 RelativeRepolarizingColor;
extern float4 DeadColor;
extern float4 BachmannColor;

extern double WallThicknessFraction;        
extern double MyocyteLength; 
extern double MyocyteDiameter;
extern double MyocyteContractionForce;
extern double MyocardialTissueDensity;
extern double MyocyteForcePerMassMultiplier;
extern double MyocyteForcePerMassSTD;
extern double DiastolicPressureLA;
extern double SystolicPressureLA;
extern double PressureMultiplier;
extern double Drag;
extern double MuscleRelaxedStrengthFraction;
extern double MuscleCompressionStopFraction;
extern double MuscleCompressionStopFractionSTD;

extern double RadiusOfLeftAtrium;
extern double MassOfLeftAtrium;
extern double MyocyteForcePerMassFraction;

extern double MouseX, MouseY, MouseZ;
extern int MouseWheelPos;
extern float HitMultiplier;
extern int ScrollSpeedToggle;
extern double ScrollSpeed;

extern int RecenterCount;
extern int RecenterRate;

extern double RunTime;

extern float4 CenterOfSimulation;
extern float4 AngleOfSimulation;

extern GLFWwindow* Window;
extern int XWindowSize;
extern int YWindowSize;
extern double Near;
extern double Far;
extern double EyeX;
extern double EyeY;
extern double EyeZ;
extern double CenterX;
extern double CenterY;
extern double CenterZ;
extern double UpX;
extern double UpY;
extern double UpZ;

// function prototypes (same as before)

// Functions in the SVT.cu file.
void nBody(double);
void allocateMemory();
void readBasicSimulationSetupParameters();
void readIntermediateSimulationSetupParameters();
void readAdvancedSimulationSetupParameters();
void setup();
int main(int, char**);

// forward declarations for other modules are provided by their own headers

#endif // HEADER_H

