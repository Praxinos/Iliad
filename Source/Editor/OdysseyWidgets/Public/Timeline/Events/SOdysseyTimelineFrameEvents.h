// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnGenerateContextMenu, const FGeometry&, const FPointerEvent&)
DECLARE_DELEGATE_TwoParams(FOnDragEnter, const FGeometry&, const FDragDropEvent&)
DECLARE_DELEGATE_OneParam(FOnDragLeave, const FDragDropEvent&)
DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnDragDetected, const FGeometry&, const FPointerEvent&)

