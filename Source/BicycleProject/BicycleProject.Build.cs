// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class BicycleProject : ModuleRules
{
	public BicycleProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// 共通
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "XRBase", "Niagara", "Foliage", "AIModule"});

		// Windowsだけ
        if (Target.Platform == UnrealTargetPlatform.Win64)
            PublicDependencyModuleNames.AddRange(new string[] { "ASerialCom" });
		// Androidだけ
        else if(Target.Platform == UnrealTargetPlatform.Android)
            PublicDependencyModuleNames.AddRange(new string[] { "BleGoodies", "AndroidPermission" });
			

        PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
