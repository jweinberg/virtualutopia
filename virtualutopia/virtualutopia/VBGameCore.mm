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

uint32_t bmpData[384 * 256];
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
    DLog(@"Executing");
    //Get a reference to the emulator
    for (int i = 0; i < 1000000; ++i)
        vb->cpu->step();
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
    return OESizeMake(384, 256);
}

- (const void *)videoBuffer
{
    //memset(bmpData, 0xFF00FFFF, 384 * 256);
    
    char * internalData = vb->vip->leftFrameBuffer_0;
    
    for (int x = 0; x < 384; ++x)
    {
        for (int y = 0; y < 64; ++y)
        {
            for (int bt = 0; bt < 4; ++bt)
            {
                uint32_t pixel;
                char px = (*internalData >> (bt * 2)) & 0x3;
                if (px == 1)
                    pixel = 0xFF520052;
                else if (px == 2)
                    pixel = 0xFFAD00AD;
                else if (px == 3)
                    pixel = 0xFFFF00FF;
                else if (px == 0)
                    pixel = 0xFF000000;
                
                *(bmpData + (384 * (y * 4 + bt)) + x) = pixel;
            }
            internalData++;
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

@end
