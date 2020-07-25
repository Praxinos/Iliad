// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "IOdysseyStylusInputModule.h"

class FWintabStylusInputInterfaceImpl;

class FWintabStylusInputInterface : public IStylusInputInterfaceInternal
{
public:
	FWintabStylusInputInterface(TUniquePtr<FWintabStylusInputInterfaceImpl> InImpl);
	virtual ~FWintabStylusInputInterface();

	virtual void Tick() override;
	virtual int32 NumInputDevices() const override;
	virtual IStylusInputDevice* GetInputDevice(int32 Index) const override;

    virtual TWeakPtr<SWindow> Window() const override;
    virtual TWeakPtr<SWidget> Widget() const override;

private:
	// pImpl to avoid including Wintab headers.
	TUniquePtr<FWintabStylusInputInterfaceImpl> Impl;
};
