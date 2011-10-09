//
//  VBGameCore.h
//  virtualutopia
//
//  Created by Joshua Weinberg on 10/9/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <OpenEmuBase/OEGameCore.h>
#import "vb.h"
#import "OEVBSystemResponderClient.h"
@interface VBGameCore : OEGameCore <OEVBSystemResponderClient>
{
    VB *vb;
}

@end
