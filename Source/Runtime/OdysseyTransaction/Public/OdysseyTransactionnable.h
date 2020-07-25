// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "OdysseyModification.h"
#include "OdysseyTransaction.h"
#include "OdysseyUndoHistory.h"

/**
 * Describes a transactionnable, which can be modified and put as a state inside a Undo History Widget
 */
class FOdysseyTransactionnable
{
public:
    FOdysseyTransactionnable(); // Historyless constructor, no transaction will be done in this transactionnable
    FOdysseyTransactionnable( FOdysseyUndoHistory* iUndoHistoryPtr );
    ~FOdysseyTransactionnable();

public:
    template <typename T>
    void ModifyAsState( T iData, T* iReferencedObject, void( *iCallbackPtr )() = 0 );

    void Record( FName iName );
    void EndRecord();
    FOdysseyUndoHistory* GetUndoHistoryPtr();

private:
    FOdysseyUndoHistory* mUndoHistoryPtr;
};

#include "OdysseyTransactionnable.inl"
