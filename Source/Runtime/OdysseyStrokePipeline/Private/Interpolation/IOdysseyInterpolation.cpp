// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "Interpolation/IOdysseyInterpolation.h"

#define LOCTEXT_NAMESPACE "IOdysseyInterpolation"

//---

IOdysseyInterpolation::~IOdysseyInterpolation()
{
}

IOdysseyInterpolation::IOdysseyInterpolation()
    : mInputPoints()
    , mResultPoints()
    , mStep( 1.f )
    , mLastDrawnLength( 0.f )
    , mTotalStrokeLength( 0.f )
{
}

//---

void
IOdysseyInterpolation::Reset()
{
    mInputPoints.Empty();
    mResultPoints.Empty();
    mLastDrawnLength = 0.f;
    mTotalStrokeLength = 0.f;
}

void
IOdysseyInterpolation::AddPoint( const FOdysseyStrokePoint& iPoint )
{
    mInputPoints.Add( iPoint );
}

float
IOdysseyInterpolation::GetStep() const
{
    return mStep;
}

void
IOdysseyInterpolation::SetStep( float iValue )
{
    mStep = iValue;
}

#undef LOCTEXT_NAMESPACE
