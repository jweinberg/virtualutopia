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
    
    while ((vb->cpu->cycles - currentCycles) < 400000)
        vb->cpu->step();
}

+ (NSTimeInterval)defaultTimeInterval
{
    return 50.0f;
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
    memcpy(bmpData,  vb->vip->bmpData, sizeof(uint32_t) * 384 * 256);
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

- (void)didPushVBButton:(OEVBButton)button
{
    switch (button)
    {
        case OEVBButtonLeftUp:
            vb->nvc->SDHR.LU = 1;
            break;
        case OEVBButtonLeftDown:
            vb->nvc->SDHR.LD = 1;
            break;
        case OEVBButtonLeftLeft:
            vb->nvc->SDHR.LL = 1;
            break;
        case OEVBButtonLeftRight:
            vb->nvc->SDHR.LR = 1;
            break;
        case OEVBButtonRightUp:
            vb->nvc->SDLR.RU = 1;
            break;
        case OEVBButtonRightDown:
            vb->nvc->SDHR.RD = 1;
            break;
        case OEVBButtonRightLeft:
            vb->nvc->SDHR.RL = 1;
            break;
        case OEVBButtonRightRight:
            vb->nvc->SDLR.RR = 1;
            break;
        case OEVBButtonL:
            vb->nvc->SDLR.LT = 1;
            break;
        case OEVBButtonR:
            vb->nvc->SDLR.RT = 1;
            break;
        case OEVBButtonA:
            vb->nvc->SDLR.A = 1;
            break;
        case OEVBButtonB:
            vb->nvc->SDLR.B = 1;
            break;
        case OEVBButtonSelect:
            vb->nvc->SDHR.SEL = 1;
            break;
        case OEVBButtonStart:
            vb->nvc->SDHR.STA = 1;
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
            vb->nvc->SDHR.LU = 0;
            break;
        case OEVBButtonLeftDown:
            vb->nvc->SDHR.LD = 0;
            break;
        case OEVBButtonLeftLeft:
            vb->nvc->SDHR.LL = 0;
            break;
        case OEVBButtonLeftRight:
            vb->nvc->SDHR.LR = 0;
            break;
        case OEVBButtonRightUp:
            vb->nvc->SDLR.RU = 0;
            break;
        case OEVBButtonRightDown:
            vb->nvc->SDHR.RD = 0;
            break;
        case OEVBButtonRightLeft:
            vb->nvc->SDHR.RL = 0;
            break;
        case OEVBButtonRightRight:
            vb->nvc->SDLR.RR = 0;
            break;
        case OEVBButtonL:
            vb->nvc->SDLR.LT = 0;
            break;
        case OEVBButtonR:
            vb->nvc->SDLR.RT = 0;
            break;
        case OEVBButtonA:
            vb->nvc->SDLR.A = 0;
            break;
        case OEVBButtonB:
            vb->nvc->SDLR.B = 0;
            break;
        case OEVBButtonSelect:
            vb->nvc->SDHR.SEL = 0;
            break;
        case OEVBButtonStart:
            vb->nvc->SDHR.STA = 0;
            break;
        default:
            break;
    }   
}

@end
