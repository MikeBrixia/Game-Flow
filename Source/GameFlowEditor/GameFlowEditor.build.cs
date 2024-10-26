// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GameFlowEditor : ModuleRules
{
	public GameFlowEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		
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
				"UnrealEd", 
				"AssetTools", 
				"AssetRegistry",
				"GraphEditor", 
				"BlueprintGraph", 
				"Projects", 
				"EditorSubsystem", 
				"ToolMenus",
				"DeveloperSettings", 
				"PropertyEditor", 
				"EditorFramework",
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