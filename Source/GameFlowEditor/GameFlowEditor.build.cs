// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameFlowEditor : ModuleRules
{
	public GameFlowEditor(ReadOnlyTargetRules Target) : base(Target)
	{

		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
		);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
		);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"GameFlow", 
				"AssetTools", 
				"AssetRegistry",
				"GraphEditor", 
				"BlueprintGraph", 
				"Projects", 
				"ToolMenus",
				"DeveloperSettings", 
				"PropertyEditor", 
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"EditorScriptingUtilities",
				"UMG",
				"UMGEditor",
				"ToolWidgets",
				"InputCore",
				"ApplicationCore",
				"UnrealEd",
				"EditorSubsystem",
				"EditorFramework"
				// .. add private dependencies that you statically link with here ...	
			}
		);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}