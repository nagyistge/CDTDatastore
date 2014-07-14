//
//  TDReplicatorManager.h
//  TouchDB
//
//  Created by Jens Alfke on 2/15/12.
//  Copyright (c) 2012 Couchbase, Inc. All rights reserved.
//
//  Modifications for this distribution by Cloudant, Inc., Copyright (c) 2014 Cloudant, Inc.
//

#import "TD_Database.h"
@class TD_DatabaseManager;
@class TDReplicator;
@protocol TDAuthorizer;

/** Manages the creation and queueing of TDReplicator objects on a separate replication thread.
One must -start (-stop) the TDReplicatorManager in order to setup (destroy) the replication
 thread on which individual TDReplicators execute.
*/
@interface TDReplicatorManager : NSObject
{
    TD_DatabaseManager* _dbManager;
    NSThread* _thread;
    NSMutableDictionary* _replicatorsBySessionID;
    BOOL _updateInProgress;

    NSThread* _serverThread;
    BOOL _stopRunLoop;
}

- (id) initWithDatabaseManager: (TD_DatabaseManager*)dbManager;

- (void) start;
- (void) stop;

- (TDReplicator* ) createReplicatorWithProperties: (NSDictionary*) properties;
- (void) startReplicator: (TDReplicator*) repl;

@end
