// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;
using System.IO;

public class UE4Bson : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string LibbsonPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Resources/libbson/")); }
    }

    public UE4Bson(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
                "UE4Bson/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "UE4Bson/Private",
				// ... add other private include paths required here ...
			}
			);

        PublicDefinitions.Add("BSON_COMPILATION");

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(LibbsonPath, "lib", "bson-static-1.0.lib"));
            PublicIncludePaths.Add(Path.Combine(LibbsonPath, "include"));
        }
        else
        {
            Console.WriteLine("Unsupported OS - only win64 is currently supported.");
        }

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
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
                "Json",
                "JsonUtilities",
				// ... add private dependencies that you statically link with here ...	
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
