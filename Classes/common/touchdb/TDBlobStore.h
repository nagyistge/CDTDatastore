//
//  TDBlobStore.h
//  TouchDB
//
//  Created by Jens Alfke on 12/10/11.
//  Copyright (c) 2011 Couchbase, Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

#import <FMDB/FMDB.h>

#ifdef GNUSTEP
#import <openssl/md5.h>
#import <openssl/sha.h>
#else
#define COMMON_DIGEST_FOR_OPENSSL
#import <CommonCrypto/CommonDigest.h>
#endif

#import "CDTEncryptionKeyProvider.h"
#import "CDTBlobReader.h"
#import "CDTBlobWriter.h"

extern NSString *const CDTBlobStoreErrorDomain;

typedef NS_ENUM(NSInteger, CDTBlobStoreError) {
    CDTBlobStoreErrorNoFilenameGenerated
};

/** Key identifying a data blob. This happens to be a SHA-1 digest. */
typedef struct TDBlobKey
{
    uint8_t bytes[SHA_DIGEST_LENGTH];
} TDBlobKey;

/** A persistent content-addressable store for arbitrary-size data blobs.
    Each blob is stored as a file named by its SHA-1 digest. */
@interface TDBlobStore : NSObject {
    NSString* _tempDir;
}

/**
 Initialise a blob store.
 
 @param dir Directory where attachments will be stored (it will be created if it does not exist)
 @param provider It will return the key to cipher the attachments (if it return nil,
 the attachments will not be encrypted)
 @param outError It will point to an error if there is any.
 */
- (id)initWithPath:(NSString *)dir
    encryptionKeyProvider:(id<CDTEncryptionKeyProvider>)provider
                    error:(NSError **)outError;

- (id<CDTBlobReader>)blobForKey:(TDBlobKey)key withDatabase:(FMDatabase *)db;

- (BOOL)storeBlob:(NSData *)blob
      creatingKey:(TDBlobKey *)outKey
     withDatabase:(FMDatabase *)db
            error:(NSError *__autoreleasing *)outError;

- (NSUInteger)countWithDatabase:(FMDatabase *)db;

- (NSInteger)deleteBlobsExceptWithKeys:(NSSet*)keysToKeep withDatabase:(FMDatabase *)db;

@end

typedef struct
{
    uint8_t bytes[MD5_DIGEST_LENGTH];
} TDMD5Key;

/** Lets you stream a large attachment to a TDBlobStore asynchronously, e.g. from a network
 * download. */
@interface TDBlobStoreWriter : NSObject {
   @private
    TDBlobStore* _store;
    NSString* _tempPath;
    id<CDTBlobWriter> _blobWriter;
    UInt64 _length;
    SHA_CTX _shaCtx;
    MD5_CTX _md5Ctx;
    TDBlobKey _blobKey;
    TDMD5Key _MD5Digest;
}

- (id)initWithStore:(TDBlobStore*)store;

/** Appends data to the blob. Call this when new data is available. */
- (void)appendData:(NSData*)data;

/** Call this after all the data has been added. */
- (void)finish;

/** Call this to cancel before finishing the data. */
- (void)cancel;

/** Installs a finished blob into the store. */
- (BOOL)installWithDatabase:(FMDatabase *)db;

/** The number of bytes in the blob. */
@property (readonly) UInt64 length;

/** After finishing, this is the key for looking up the blob through the TDBlobStore. */
@property (readonly) TDBlobKey blobKey;

/** After finishing, this is the MD5 digest of the blob, in base64 with an "md5-" prefix.
    (This is useful for compatibility with CouchDB, which stores MD5 digests of attachments.) */
@property (readonly) NSString* MD5DigestString;
@property (readonly) NSString* SHA1DigestString;

@end
