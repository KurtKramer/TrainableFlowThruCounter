// *****************************************************************************
//
//     Copyright (c) 2010, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#define DEPTH ( 60 )


class FPSStabilizer
{
public:

    FPSStabilizer()
    {
        Reset();
    }

    bool IsTimeToDisplay( int aTargetFPS )
    {
        DWORD lNow = ::GetTickCount();

        bool lFirst = false;
        if ( mLast == 0 )
        {
            lFirst = true;
            mLast = lNow;
        }

        DWORD lDelta = lNow - mLast;
        double lSlice = 1000.0 / static_cast<double>( aTargetFPS );

        if ( mFull || ( mHead >= 4 ) )
        {
            // Compute average inter-frame spacing (ms), without current frame
            double lAvg0 = static_cast<double>( mSum ) / static_cast<double>( mCount );

            // Compute average inter-frame spacing (ms), including current frame
            __int64 lSum = mSum + lDelta;
            double lAvg1 = static_cast<double>( lSum ) / static_cast<double>( mCount + 1 );

            double lDistance0 = fabs( lSlice - lAvg0 );
            double lDistance1 = fabs( lSlice - lAvg1 );

            bool lCloser = lDistance1 < lDistance0;
            bool lMinGap = lDelta > ( lSlice * 0.25 );
            bool lDrifting = lDistance1 > mLastDistance;
            bool lInstant = lDelta >= lSlice;

            if ( ( lCloser || lDrifting || lInstant ) /* && lMinGap */ )
            {
                mLastDistance = DBL_MAX;
                Log( lNow );
                return true;
            }

            mLastDistance = lDistance1;
        }
        else
        {
            // Primitive, but good enough to boot the system
            if ( ( lDelta >= lSlice ) || lFirst )
            {
                Log( lNow );
                return true;
            }
        }

        return false;
    }

    int GetAvg()
    {
        return static_cast<int>( mAvg + 0.5 );
    }

    void Reset()
    {
        mSum = 0;
        mCount = 0;
        mHead = 0;
        mFull = false;
        mLast = 0;
        mAvg = 0;
        mLastDistance = DBL_MAX;
    }

protected:

    void Log( DWORD aNow )
    {
        if ( mFull )
        {
            mSum -= mTimes[ mHead ];
        }

        mTimes[ mHead ] = aNow - mLast;
        mSum += mTimes[ mHead ];
        ++mHead %= DEPTH;
        if ( mHead == 0 )
        {
            mFull = true;
        }

        mCount = ( mFull ) ? DEPTH : mHead;
        mAvg = 1000.0 / ( static_cast<double>( mSum ) / static_cast<double>( mCount ) );

        mLast = aNow;
    }

private:

    DWORD mLast;
    DWORD mTimes[ DEPTH ];
    __int64 mSum;
    int mCount;
    int mHead;
    bool mFull;
    double mAvg;
    double mLastDistance;
};


