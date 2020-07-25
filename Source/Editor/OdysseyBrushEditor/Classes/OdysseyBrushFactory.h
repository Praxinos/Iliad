// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Templates/SubclassOf.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyBrushFactory.generated.h"

UCLASS()
class UOdysseyBrushFactory : public UFactory
{
    GENERATED_UCLASS_BODY()

    // The parent class of the created brush
    UPROPERTY(EditAnywhere, Category=BlueprintFactory, meta=(AllowAbstract = "", BlueprintBaseOnly = ""))
    TSubclassOf< class  UObject > ParentClass;

    // UFactory interface
    virtual  UObject*  FactoryCreateNew(    UClass* Class,
                                            UObject* InParent,
                                            FName Name,
                                            EObjectFlags Flags,
                                            UObject* Context,
                                            FFeedbackContext* Warn)  override;
};
