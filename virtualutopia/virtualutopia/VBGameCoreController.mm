//
//  VBGameCoreController.m
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/9/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "VBGameCoreController.h"
#import "VBGameCore.h"

@implementation VBGameCoreController


+ (void)initialize
{
    if(self == [VBGameCoreController class])
    {
        /*
         [self registerPreferenceViewControllerClasses:
         [NSDictionary dictionaryWithObject:[SMSPreferenceView class]
         forKey:OEControlsPreferenceKey]];
         */
    }
}

- (NSDictionary*)defaultControls
{
    /*
     NSDictionary *controls = [NSDictionary dictionaryWithObjectsAndKeys:
     [NSNumber numberWithUnsignedInt:kHIDUsage_KeyboardUpArrow], @"SMS_PAD1_UP",
     [NSNumber numberWithUnsignedInt:kHIDUsage_KeyboardRightArrow], @"SMS_PAD1_RIGHT",
     [NSNumber numberWithUnsignedInt:kHIDUsage_KeyboardLeftArrow], @"SMS_PAD1_LEFT",
     [NSNumber numberWithUnsignedInt:kHIDUsage_KeyboardDownArrow], @"SMS_PAD1_DOWN",
     [NSNumber numberWithUnsignedInt:kHIDUsage_KeyboardA], @"SMS_PAD1_A",
     [NSNumber numberWithUnsignedInt:kHIDUsage_KeyboardS], @"SMS_PAD1_B",
     [NSNumber numberWithUnsignedInt:kHIDUsage_KeyboardEscape], @"SMS_RESET",nil];
     return controls;
     */
    return nil;
}

- (NSArray *)genericControlNames
{
    return nil;
    /*
     static NSArray *genericControlNames = nil;
     if(genericControlNames == nil)
     {
     //genericControlNames = [[NSArray alloc] initWithObjects:SMSButtonNameTable count:8];
     }
     return genericControlNames;
     */
}

- (NSUInteger)playerCount
{
    return 1;
}

- (Class)gameCoreClass
{
    return [VBGameCore class];
}

@end
