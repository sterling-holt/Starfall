////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <NsCore/String.h>
#include "MessageBuffer.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::MessageBuffer::MessageBuffer() : mContentLength(0), mCurrentState(0)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::MessageBuffer::HandleChar(const char c)
{
    mBuffer.PushBack(c);
    switch (mCurrentState)
    {
        case 0:
        {
            if (StrStartsWith(mBuffer.Str(), "Content-"))
            {
                int endOfLinePos = mBuffer.Find("\r\n");
                if (endOfLinePos == -1)
                {
                    endOfLinePos = mBuffer.Find("\n");
                }
                if (endOfLinePos != -1)
                {
                    if (StrStartsWith(mBuffer.Str(), "Content-Length"))
                    {
                        const int delimiterPosition = mBuffer.Find(":");
                        if (delimiterPosition != -1)
                        {
                            const String contentLengthValue = mBuffer.Substr(delimiterPosition 
                                + 2, endOfLinePos - delimiterPosition - 2);

                            uint32_t parsed;
                            const uint32_t contentLength = StrToUInt32(contentLengthValue.Str(), 
                                &parsed);

                            if (parsed > 0)
                            {
                                mContentLength = contentLength;
                            }
                            else
                            {
                                NS_ERROR("Invalid Content-Length header");
                            }
                        }
                    }
                    mBuffer.Clear();
                }
            }
            else if (StrEndsWith(mBuffer.Str(), "\n"))
            {
                if (mContentLength == 0)
                {
                    NS_ERROR("MessageBuffer missing Content-Length header: \n%s\n\n",
                        mBuffer.Str());
                    break;
                }

                mBuffer.Clear();
                mCurrentState = 1;
            }

            break;            
        }
        case 1:
        {
            if (mBuffer.Size() == mContentLength) 
            {
                mCurrentState = 2;
            }

            break;
        }
        default: break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::JsonObject Noesis::MessageBuffer::GetBody() const
{
    if (!IsMessageCompleted())
    {
        NS_ERROR("GetBody failed, MessageBuffer does not have a completed message");
        return {};
    }
    return { mBuffer.Begin(), mBuffer.End() };
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const char* Noesis::MessageBuffer::GetBufferStr() const
{
    return mBuffer.Str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Noesis::MessageBuffer::IsMessageCompleted() const
{
    return (mCurrentState == 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::MessageBuffer::Clear()
{
    mBuffer.Clear();
    mContentLength = 0;
    mCurrentState = 0;
}
