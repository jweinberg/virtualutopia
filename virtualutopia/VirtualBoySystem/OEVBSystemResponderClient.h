//
//  OESMSSystemResponderClient.h
//  OpenEmu
//
//  Created by Remy Demarest on 24/04/2011.
//  Copyright 2011 NuLayer Inc. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol OESystemResponderClient;

typedef enum _OEVBButton
{
    OEVBButtonLeftUp,
    OEVBButtonLeftDown,
    OEVBButtonLeftLeft,
    OEVBButtonLeftRight,
    OEVBButtonRightUp,
    OEVBButtonRightDown,
    OEVBButtonRightLeft,
    OEVBButtonRightRight,
    OEVBButtonL,
    OEVBButtonR,
    OEVBButtonA,
    OEVBButtonB,
    OEVBButtonSelect,
    OEVBButtonStart,
    OEVBButtonCount,
} OEVBButton;

@protocol OEVBSystemResponderClient <OESystemResponderClient, NSObject>

- (void)didPushVBButton:(OEVBButton)button;
- (void)didReleaseVBButton:(OEVBButton)button;

@end
