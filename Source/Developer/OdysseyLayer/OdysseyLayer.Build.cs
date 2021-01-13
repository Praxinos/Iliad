// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using UnrealBuildTool;

public class OdysseyLayer : ModuleRules
{
    public OdysseyLayer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Engine",
                "OdysseyImaging",
                "OdysseyTransaction",
                "ULIS",
                "ULISLoader",
                "OdysseySerializable",
                "OdysseyMaths"
            }
        );
    }
}