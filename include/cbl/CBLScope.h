//
//  CBLScope.h
//
// Copyright (c) 2022 Couchbase, Inc All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once
#include "CBLBase.h"
#include "fleece/Fleece.h"

CBL_CAPI_BEGIN

/** \defgroup scope   Scope
    @{
    A \ref CBLScope represents a scope or namespace of the collections.
 
    The scope implicitly exists when there is at least one collection created under the scope.
    The default scope is exceptional in that it will always exists even there are no collections
    under it.
 
    ## `CBLScope` Lifespan
    `CBLScope` is ref-counted and is owned by the database object that creates it. Hence,
    most of the time there is no need to retain or release it. A `CBLScope` object and its
    reference remain valid until either the database is closed or the scope itself is invalidated
    as all collections in the scope have been deleted.

    If the scope reference needs to be kept longer, the scope object should be retained,
    and the reference will remain valid until it's released. Most operations on the invalid
    CBLScope object will fail with null or empty result.
 */

CBL_REFCOUNTED(CBLScope*, Scope);

/** \name  Default Scope Name
    @{
    The default scope name constant.
 */

/** The default scope's name. */
CBL_PUBLIC extern const FLString kCBLDefaultScopeName;

/** @} */

/** \name  Scope Accessors
    @{
    Getting information about a scope.
 */

/** Returns the name of the scope.
    @param scope  The scope.
    @return  The name of the scope. */
FLString CBLScope_Name(const CBLScope* scope) CBLAPI;

/** @} */

/** \name  Collections
    @{
    Accessing the collections under the scope.
 */

/** Returns the names of all collections in the scope.
    @note  You are responsible for releasing the returned array.
    @param scope  The scope.
    @return  The names of all collections in the scope. */
FLMutableArray CBLScope_CollectionNames(const CBLScope* scope) CBLAPI;

/** Returns an existing collection in the scope with the given name.
    @note  CBLCollection is ref-counted and is owned by the database object, and it will remain
           valid until the database is closed, or the collection itself is deleted. Therefore, there
           is no need to retain or release it. However if the reference needs to be kept longer,
           the object needs to be retained, and it will remain valid until it is released.
    @param scope  The scope.
    @param collectionName  The name of the collection.
    @return A \ref CBLCollection instance, or NULL if the collection doesn't exist. */
CBLCollection* _cbl_nullable CBLScope_Collection(const CBLScope* scope, FLString collectionName) CBLAPI;

/** @} */
/** @} */    // end of outer \defgroup

CBL_CAPI_END