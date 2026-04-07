#ifndef GUI_H
#define GUI_H

#include "core.h"
#include "view.h"
#include "callbacks.h"
#include "utils.h"

void createGUI()
{

	// Get actual viewport size -- this is the size of the window, not the size of the the openGL viewport
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	//status panel: always visible so the user knows whether simulation is in GUI mode or mouse mode.
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + 10, viewport->WorkPos.y + 10), ImGuiCond_Always, ImVec2(0.0f, 0.0f));
	ImGuiWindowFlags status_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings;
	ImGui::Begin("Interaction Mode", NULL, status_flags);
	if (!Simulation.isInMouseFunctionMode)
	{
		ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.2f, 1.0f), "GUI Mode");
		ImGui::TextUnformatted("Mouse editing disabled");
	}
	else
	{
		ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.2f, 1.0f), "Mouse Mode");
		if (Simulation.mouseMode == MOUSE_MODE_STANDARD) ImGui::TextUnformatted("Section: Standard");
		else if (Simulation.mouseMode == MOUSE_MODE_BACHMANNS_BUNDLE) ImGui::TextUnformatted("Section: Bachmann's Bundle");
		else if (Simulation.mouseMode == MOUSE_MODE_APPENDAGE) ImGui::TextUnformatted("Section: Appendage");
		else if (Simulation.mouseMode == MOUSE_MODE_SCAR_TISSUE) ImGui::TextUnformatted("Section: Scar Tissue");
		else ImGui::TextUnformatted("Section: None");
	}
	ImGui::TextUnformatted("Tab: Toggle GUI/Mouse mode");
	if (BinarySaveStatusMessage[0] != '\0')
	{
		ImGui::Separator();
		ImGui::TextWrapped("%s", BinarySaveStatusMessage);
	}
	ImGui::End();

	// Mouse mode hides the control panel to match model behavior.
	if (Simulation.isInMouseFunctionMode)
	{
		return;
	}

	//Set in top right corner of the window, 10px offset from both edges
	//ImGUICond_Always means the position will always be set to this value, regardless of previous positions
	//last arg anchors to the right and top of the window
	ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - 10, viewport->WorkPos.y + 10), ImGuiCond_Always,  ImVec2(1.0f, 0.0f));

    // Setup ImGui window flags
    ImGuiWindowFlags window_flags = 0; // Initialize window flags to 0, flags are used to set window properties, like size, position, etc. 0 means no flags are set
    window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing; // Always resize the window to fit the content
    

	// Set the collapsed state if guiCollapsed is true (toggled by ctrl + h callback)
    ImGui::SetNextWindowCollapsed(Simulation.guiCollapsed, ImGuiCond_Always);


    // Main Controls Window
    ImGui::Begin("Control Panel", NULL, window_flags); //title of the window, NULL means no pointer to a bool to close the window, window_flags are the flags we set above
    
	//update bool to match current state (makes sure clicking also works in addition to ctrl + h)
	Simulation.guiCollapsed = ImGui::IsWindowCollapsed();


    
    // General simulation controls
    if (ImGui::CollapsingHeader("Simulation Controls", ImGuiTreeNodeFlags_DefaultOpen)) //open by default
    {
        // View controls
        bool frontHalf = Simulation.DrawFrontHalfFlag == 1; //Needed because ImGui needs a bool for a checkbox, can make a dropbox if more display options are needed
        if(ImGui::Checkbox("Draw Front Half Only", &frontHalf)) //checkbox for if we only want to draw the first half of the nodes
        {
			//when the button is pressed it will change the value of frontHalf to the opposite of what it was before
            Simulation.DrawFrontHalfFlag = frontHalf ? 1 : 0;
            drawPicture();
        }
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(F2)");
			ImGui::EndTooltip();
		}
        
        // Node display options
        const char* nodeOptions[] = { "Off", "Half", "Full" }; //array of options for the dropdown menu
        int nodeDisplay = Simulation.DrawNodesFlag;

		//Combo makes a dropdown menu with the options in the array
        if(ImGui::Combo("Show Nodes", &nodeDisplay, nodeOptions, 3)) //args are menu name, pointer to the selected option, array of text options, # of options
        {
            if (nodeDisplay != Simulation.DrawNodesFlag) // Only update if the value changes
            {
                Simulation.DrawNodesFlag = nodeDisplay;
                drawPicture();
            }
        }
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(F3)");
			ImGui::EndTooltip();
		}

        // Change view
	

	}       
       
	// View presets
	if (ImGui::CollapsingHeader("View Controls", ImGuiTreeNodeFlags_DefaultOpen))//2nd arg is the flags, DefaultOpen means it will be open by default
	{

		if (ImGui::Button("PA"))
		{ 
			setView(4); 
			//copyNodesToGPU(); 
			drawPicture(); 
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(7)\nPosterior-Anterior View\nView from back to front");
			ImGui::EndTooltip();
		}
		
		ImGui::SameLine();
		if (ImGui::Button("AP"))  
		{
			setView(2); 
			//copyNodesToGPU(); 
			drawPicture(); 
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(8)\nAnterior-Posterior View\nView from front to back");
			ImGui::EndTooltip();
		}
		
		ImGui::SameLine();
		if (ImGui::Button("Ref"))
		{ 
			setView(6); 
			//copyNodesToGPU(); 
			drawPicture(); 
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(9)\nReference View\nStandard orientation with pulmonary veins visible");
			ImGui::EndTooltip();
		}
		
		if (ImGui::Button("LAO"))
		{ 
			setView(1); 
			//copyNodesToGPU(); 
			drawPicture(); 
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(4)\nLeft Anterior Oblique\nAngled view from front-left");
			ImGui::EndTooltip();
		}
		
		ImGui::SameLine();
		if (ImGui::Button("RAO"))
		{ 
			setView(3); 
			//copyNodesToGPU(); 
			drawPicture(); 
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(5)\nRight Anterior Oblique\nAngled view from front-right");
			ImGui::EndTooltip();
		}
		
		ImGui::SameLine();
		if (ImGui::Button("LL"))
		{ 
			setView(7); 
			//copyNodesToGPU(); 
			drawPicture(); 
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(6)\nLeft Lateral\nDirect view from left side");
			ImGui::EndTooltip();
		}

		if (ImGui::Button("RL"))
		{ 
			setView(9); 
			//copyNodesToGPU(); 
			drawPicture(); 
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(1)\nRight Lateral\nDirect view from right side");
			ImGui::EndTooltip();
		}
		
		ImGui::SameLine();
		if (ImGui::Button("SUP"))
		{ 
			setView(8); 
			//copyNodesToGPU(); 
			drawPicture(); 
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(2)\nSuperior View\nView from above (top-down)");
			ImGui::EndTooltip();
		}
		
		ImGui::SameLine();
		if (ImGui::Button("INF"))
		{ 
			setView(5); 
			//copyNodesToGPU(); 
			drawPicture(); 
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(3)\nInferior View\nView from below (bottom-up)");
			ImGui::EndTooltip();
		}
	}
	
	// Mouse mode selection
	if (ImGui::CollapsingHeader("Mouse Functions", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// TODO: Rewrite this for the new mode system
		/* // Display current mouse mode
		ImGui::Text("Current Mode: ");
		if (!Simulation.isInMouseFunctionMode) 
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Mouse Off");
		}
		else if (Simulation.isInAblateMode) 
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Ablate Mode");
			ImGui::Text("Alt + Q to exit mouse mode");
			ImGui::Text("(Left Click: Ablate, Right Click: Undo)");
		}
		else if (Simulation.isInEctopicBeatMode) 
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Ectopic Beat");
			ImGui::Text("Alt + Q to exit mouse mode");
		} 
		else if (Simulation.isInEctopicEventMode) 
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "Ectopic Trigger");
			ImGui::Text("Alt + Q to exit mouse mode");
		} 
		else if (Simulation.isInAdjustMuscleAreaMode) 
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Adjust Area");
			ImGui::Text("Alt + Q to exit mouse mode");
		} 
		else if (Simulation.isInAdjustMuscleLineMode) 
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Adjust Line");
			ImGui::Text("Alt + Q to exit mouse mode");
		} 
		else if (Simulation.isInFindNodeMode) 
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.5f, 0.0f, 1.0f, 1.0f), "Identify Node");
			ImGui::Text("Alt + Q to exit mouse mode");
		}*/

		// Mouse mode buttons
		// TODO: Implement the mouse mode defines here.
		if (ImGui::Button("Mouse Off"))
		{
			setMouseMode(&Simulation, MOUSE_MODE_OFF); // OR whatever number for simulation off 
		}
		if (ImGui::IsItemHovered())
		{
		}

		if (ImGui::Button("Set Standard Node")) 
		{
			setMouseMode(&Simulation, MOUSE_MODE_STANDARD); 
		}
		// TODO: Make helper functions for tooltips
		if (ImGui::IsItemHovered())
		{  
			ImGui::BeginTooltip();
			ImGui::Text("(F4)\nSet selected nodes to Standard");
			ImGui::EndTooltip();
		}

		if (ImGui::Button("Set Bachmann's Bundle")) 
		{
			setMouseMode(&Simulation, MOUSE_MODE_BACHMANNS_BUNDLE);
		}
		if(ImGui::IsItemHovered()) 
		{ 
			ImGui::BeginTooltip();
			ImGui::Text("(F5)\nSet selected nodes to Bachmann's Bundle");
			ImGui::EndTooltip();
		}
		if (ImGui::Button("Set Appendage")) 
		{
			setMouseMode(&Simulation, MOUSE_MODE_APPENDAGE);
		}
		if(ImGui::IsItemHovered()) 
		{ 
			ImGui::BeginTooltip();
			ImGui::Text("(F6)\nSet selected nodes to Appendage");
			ImGui::EndTooltip();
		}
		if (ImGui::Button("Scar Tissue")) 
		{
			setMouseMode(&Simulation, MOUSE_MODE_SCAR_TISSUE); 
		}
		if(ImGui::IsItemHovered()) 
		{ 
			ImGui::BeginTooltip();
			ImGui::Text("(F7)\nSet selected nodes to Scar Tissue");
			ImGui::EndTooltip();
		}

	}
    
    // Utility functions
	if (ImGui::CollapsingHeader("Utilities", ImGuiTreeNodeFlags_DefaultOpen))
    {
		if (ImGui::Button("Save Binary"))
		{
			saveBinary();
		}
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(Ctrl + Shift + S)\nSave node/muscle attributes to\n<NodesMusclesFileName>_<timestamp>.bin");
			ImGui::EndTooltip();
		}

		if (BinarySaveStatusMessage[0] != '\0')
		{
			ImGui::TextWrapped("%s", BinarySaveStatusMessage);
		}

		//Save settings button
        if (ImGui::Button("Save Settings"))
		{
            saveSettings();
        }
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text("(F6)\nSave current muscle properties and simulation\nsettings to a file for later use");
			ImGui::EndTooltip();
		}
    }

	//Display movement controls
	if (ImGui::CollapsingHeader("Keyboard Controls"))
	{

		ImGui::Text("Quit: esc");
		ImGui::NewLine(); //add a new line for spacing
		ImGui::Text("Rotate X-axis: a/d; Left/Right");
		ImGui::Text("Rotate Y-axis: w/s; Up/Down");
		ImGui::Text("Rotate Z-axis: z/Z; Shift + Left/Right");
		ImGui::Text("Zoom In/Out: e/E; Shift + Up/Down");
		ImGui::Text("Toggle GUI/Mouse mode: Tab");

		ImGui::Text("Collapse/Expand GUI: Ctrl + h");
		
	}
    
    ImGui::End(); //end the main controls window
    
  
}
#endif // GUI_H
