//
//  VBGameCore.m
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/9/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#define SAMPLERATE 44100
#define SAMPLEFRAME 735
#define SIZESOUNDBUFFER SAMPLEFRAME*4

#import "VBGameCore.h"
#include "vip.h"
#import <OpenGL/gl.h>

@implementation VBGameCore

- (id)init
{
    self = [super init];
    if(self != nil)
    {
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)executeFrame
{
    uint32_t currentCycles = vb->cpu->cycles;
    
    while ((vb->cpu->cycles - currentCycles) < 333333)
        vb->cpu->step();
    
}

- (double)audioSampleRate
{
    return 48000;
}

+ (NSTimeInterval)defaultTimeInterval
{
    return 60.0f;
}

- (void)setupEmulation
{
}

- (BOOL)loadFileAtPath:(NSString*)path
{
    DLog(@"Loaded File");
    if (vb)
        delete vb;
    vb = new VB([path UTF8String]);
    return YES;
}
- (void)resetEmulation
{

}

- (void)stopEmulation
{
    [super stopEmulation];
}

- (IBAction)pauseEmulation:(id)sender
{
 
}

- (OEIntSize)bufferSize
{
    return OESizeMake(384, 224);
}

- (const void *)videoBuffer
{
    //memset(bmpData, 0xFF00FFFF, 384 * 256);
    for (int y = 0; y < 256; ++y)
    {
        for (int x = 0; x < 384; ++x)
        {
            int left = *(vb->vip->leftBmpData + (y * 384) + x);
            int right = *(vb->vip->rightBmpData + (y * 384) + x);
            bmpData[x + (y * 384)] = (0xFF << 24) | (left << 16) | right;
        }
    }
    
    return bmpData;
}

- (GLenum)pixelFormat
{
    return GL_RGBA;
}

- (GLenum)pixelType
{
    return GL_UNSIGNED_INT_8_8_8_8_REV;
}

- (GLenum)internalPixelFormat
{
    return GL_RGB8;
}

- (NSUInteger)soundBufferSize
{
    return SIZESOUNDBUFFER;
}

- (NSUInteger)frameSampleCount
{
    return SAMPLEFRAME;
}

- (NSUInteger)frameSampleRate
{
    return SAMPLERATE;
}

- (NSUInteger)channelCount
{
    return 2;
}

- (oneway void)didPushVBButton:(OEVBButton)button
{
    switch (button)
    {
        case OEVBButtonLeftUp:
            vb->nvc->SetButton(NVC::LU, true);
            break;
        case OEVBButtonLeftDown:
            vb->nvc->SetButton(NVC::LD, true);
            break;
        case OEVBButtonLeftLeft:
            vb->nvc->SetButton(NVC::LL, true);
            break;
        case OEVBButtonLeftRight:
            vb->nvc->SetButton(NVC::LR, true);
            break;
        case OEVBButtonRightUp:
            vb->nvc->SetButton(NVC::RU, true);
            break;
        case OEVBButtonRightDown:
            vb->nvc->SetButton(NVC::RD, true);
            break;
        case OEVBButtonRightLeft:
            vb->nvc->SetButton(NVC::RL, true);
            break;
        case OEVBButtonRightRight:
            vb->nvc->SetButton(NVC::RR, true);
            break;
        case OEVBButtonL:
            vb->nvc->SetButton(NVC::LT, true);
            break;
        case OEVBButtonR:
            vb->nvc->SetButton(NVC::RT, true);
            break;
        case OEVBButtonA:
            vb->nvc->SetButton(NVC::A, true);
            break;
        case OEVBButtonB:
            vb->nvc->SetButton(NVC::B, true);
            break;
        case OEVBButtonSelect:
            vb->nvc->SetButton(NVC::SELECT, true);
            break;
        case OEVBButtonStart:
            vb->nvc->SetButton(NVC::STA, true);
            break;
        default:
            break;
    }
}

- (void)didReleaseVBButton:(OEVBButton)button
{
    switch (button)
    {
        case OEVBButtonLeftUp:
            vb->nvc->SetButton(NVC::LU, false);
            break;
        case OEVBButtonLeftDown:
            vb->nvc->SetButton(NVC::LD, false);
            break;
        case OEVBButtonLeftLeft:
            vb->nvc->SetButton(NVC::LL, false);
            break;
        case OEVBButtonLeftRight:
            vb->nvc->SetButton(NVC::LR, false);
            break;
        case OEVBButtonRightUp:
            vb->nvc->SetButton(NVC::RU, false);
            break;
        case OEVBButtonRightDown:
            vb->nvc->SetButton(NVC::RD, false);
            break;
        case OEVBButtonRightLeft:
            vb->nvc->SetButton(NVC::RL, false);
            break;
        case OEVBButtonRightRight:
            vb->nvc->SetButton(NVC::RR, false);
            break;
        case OEVBButtonL:
            vb->nvc->SetButton(NVC::LT, false);
            break;
        case OEVBButtonR:
            vb->nvc->SetButton(NVC::RT, false);
            break;
        case OEVBButtonA:
            vb->nvc->SetButton(NVC::A, false);
            break;
        case OEVBButtonB:
            vb->nvc->SetButton(NVC::B, false);
            break;
        case OEVBButtonSelect:
            vb->nvc->SetButton(NVC::SELECT, false);
            break;
        case OEVBButtonStart:
            vb->nvc->SetButton(NVC::STA, false);
            break;
        default:
            break;
    }
}

@end
