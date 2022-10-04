//
// CollectionTest_Cpp.cc
//
// Copyright © 2022 Couchbase. All rights reserved.
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

#include "CBLTest_Cpp.hh"
#include "fleece/Fleece.hh"
#include "fleece/Mutable.hh"
#include <string>

#include "cbl++/CouchbaseLite.hh"

using namespace std;
using namespace fleece;
using namespace cbl;

class CollectionTest_Cpp : public CBLTest_Cpp {
    
public:
    
    void testInvalidCollection(Collection& col) {
        REQUIRE(col);
        
        ExpectingExceptions x;
        
        // Properties:
        CHECK(!col.name().empty());
        CHECK(!col.scopeName().empty());
        CHECK(col.count() == 0);
        
        // Document Functions:
        CBLError error {};
        MutableDocument doc("doc1");
        try { col.saveDocument(doc); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        auto conflictHandler = [](MutableDocument d1, Document d2) -> bool { return true; };
        try { (void) col.saveDocument(doc, conflictHandler); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { (void) !col.saveDocument(doc, kCBLConcurrencyControlLastWriteWins); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.getDocument("doc1"); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.getMutableDocument("doc1"); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.deleteDocument(doc); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { (void) !col.deleteDocument(doc, kCBLConcurrencyControlLastWriteWins); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.purgeDocument(doc); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.purgeDocument("doc1"); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.getDocumentExpiration("doc1"); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.setDocumentExpiration("doc1", CBL_Now()); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.createValueIndex("Value", {}); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.createFullTextIndex("FTS", {}); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        error = {};
        try { col.getIndexNames(); } catch (CBLError e) { error = e; }
        CheckNotOpenError(error);
        
        auto listener = [](const CollectionChange* change) { };
        auto token = col.addChangeListener(listener);
        token.remove();
        
        auto docListener = [](const DocumentChange* change) { };
        auto docToken = col.addDocumentChangeListener("doc1", docListener);
        docToken.remove();
    }
};


TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Default Collection", "[Collection]") {
    Collection col = db.getDefaultCollection();
    REQUIRE(col);
    CHECK(col.name() == "_default");
    CHECK(col.scopeName() == "_default");
    
    col = db.getCollection(kCBLDefaultCollectionName);
    REQUIRE(col);
    CHECK(col.name() == "_default");
    CHECK(col.scopeName() == "_default");
    
    MutableArray names = db.getCollectionNames();
    REQUIRE(names);
    CHECK(names.toJSONString() == R"(["_default"])");
}

TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Delete Default Collection", "[Collection]") {
    Collection col = db.getDefaultCollection();
    CHECK(col);
    
    db.deleteCollection(kCBLDefaultCollectionName);
    
    col = db.getDefaultCollection();
    CHECK(!col);
    
    MutableArray names = db.getCollectionNames();
    REQUIRE(names);
    CHECK(names.toJSONString() == R"([])");
    
    ExpectingExceptions ex;
    CBLError error {};
    try {
        db.createCollection(kCBLDefaultCollectionName);
    } catch (CBLError e) {
        error = e;
    }
    CHECK(error.domain == kCBLDomain);
    CHECK(error.code == kCBLErrorInvalidParameter);
}

TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Default Scope", "[Collection]") {
    MutableArray names = db.getScopeNames();
    REQUIRE(names);
    CHECK(names.toJSONString() == R"(["_default"])");
    
    // Delete Default Collection:
    db.deleteCollection(kCBLDefaultCollectionName);
    Collection col = db.getDefaultCollection();
    CHECK(!col);
    
    names = db.getScopeNames();
    REQUIRE(names);
    CHECK(names.toJSONString() == R"(["_default"])");
}

TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Create And Get Collection In Default Scope", "[Collection]") {
    Collection col = db.getCollection("colA", kCBLDefaultScopeName);
    REQUIRE(!col);
    
    SECTION("Specify scope name") {
        col = db.createCollection("colA", kCBLDefaultScopeName);
    }
    
    SECTION("Not specify scope name") {
        col = db.createCollection("colA");
    }
    
    REQUIRE(col);
    CHECK(col.name() == "colA");
    CHECK(col.scopeName() == "_default");
    
    col = db.getCollection("colA");
    REQUIRE(col);
    CHECK(col.name() == "colA");
    CHECK(col.scopeName() == "_default");
    
    MutableArray names = db.getCollectionNames();
    REQUIRE(names);
    CHECK(names.toJSONString() == R"(["_default","colA"])");
}

TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Create And Get Collection In Named Scope", "[Collection]") {
    Collection col = db.getCollection("colA", "scopeA");
    REQUIRE(!col);
    
    col = db.createCollection("colA", "scopeA");
    REQUIRE(col);
    CHECK(col.name() == "colA");
    CHECK(col.scopeName() == "scopeA");
    
    col = db.getCollection("colA", "scopeA");
    REQUIRE(col);
    CHECK(col.name() == "colA");
    CHECK(col.scopeName() == "scopeA");
    
    MutableArray names = db.getCollectionNames("scopeA");
    REQUIRE(names);
    CHECK(names.toJSONString() == R"(["colA"])");
    
    MutableArray scopeNames = db.getScopeNames();
    REQUIRE(scopeNames);
    CHECK(scopeNames.toJSONString() == R"(["_default","scopeA"])");
}

TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Create Existing Collection", "[Collection]") {
    Collection col1 = db.createCollection("colA", "scopeA");
    REQUIRE(col1);
    CHECK(col1.name() == "colA");
    CHECK(col1.scopeName() == "scopeA");
    
    Collection col2 = db.createCollection("colA", "scopeA");
    REQUIRE(col2);
    CHECK(col2.name() == "colA");
    CHECK(col2.scopeName() == "scopeA");
    CHECK(col1 == col2);
}

TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Delete Collection", "[Collection]") {
    Collection col = db.createCollection("colA", "scopeA");
    REQUIRE(col);
    CHECK(col.name() == "colA");
    CHECK(col.scopeName() == "scopeA");
    
    col = db.getCollection("colA", "scopeA");
    REQUIRE(col);
    CHECK(col.name() == "colA");
    CHECK(col.scopeName() == "scopeA");
    
    // Add some docs:
    createNumberedDocs(col, 100);
    CHECK(col.count() == 100);
    
    MutableArray scopeNames = db.getScopeNames();
    REQUIRE(scopeNames);
    CHECK(scopeNames.toJSONString() == R"(["_default","scopeA"])");
    
    // Delete collection:
    db.deleteCollection("colA", "scopeA");
    col = db.getCollection("colA", "scopeA");
    CHECK(!col);
    
    scopeNames = db.getScopeNames();
    REQUIRE(scopeNames);
    CHECK(scopeNames.toJSONString() == R"(["_default"])");
    
    // Recreate collection:
    col = db.createCollection("colA", "scopeA");
    REQUIRE(col);
    CHECK(col.name() == "colA");
    CHECK(col.scopeName() == "scopeA");
    CHECK(col.count() == 0);
    
    col = db.getCollection("colA", "scopeA");
    REQUIRE(col);
    CHECK(col.name() == "colA");
    CHECK(col.scopeName() == "scopeA");
    CHECK(col.count() == 0);
    
    scopeNames = db.getScopeNames();
    REQUIRE(scopeNames);
    CHECK(scopeNames.toJSONString() == R"(["_default","scopeA"])");
}

TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Use Invalid Collection", "[Collection]") {
    Collection col = db.createCollection("colA", "scopeA");
    REQUIRE(col);
    
    SECTION("Delete Collection") {
        db.deleteCollection("colA", "scopeA");
    }
    
    SECTION("Delete Collection from another db instance") {
        Database db2 = openDatabaseNamed(db.name());
        db2.deleteCollection("colA", "scopeA");
        db2.close();
    }
    
    SECTION("Close Database") {
        db.close();
        db = nullptr;
    }
    
    SECTION("Delete Database") {
        db.deleteDatabase();
        db = nullptr;
    }
    
    testInvalidCollection(col);
}

TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Create Indexes and Get Index Names", "[Collection]") {
    Collection col = db.createCollection("colA", "scopeA");
    REQUIRE(col);
    
    RetainedArray names = col.getIndexNames();
    REQUIRE(names);
    REQUIRE(names.count() == 0);
    
    CBLValueIndexConfiguration index1 = {kCBLN1QLLanguage, "id"_sl};
    col.createValueIndex("index1", index1);
    
    CBLValueIndexConfiguration index2 = {kCBLN1QLLanguage, "firstname, lastname"_sl};
    col.createValueIndex("index2", index2);
    
    names = col.getIndexNames();
    REQUIRE(names);
    REQUIRE(names.count() == 2);
    CHECK(names[0].asString() == "index1");
    CHECK(names[1].asString() == "index2");
}

TEST_CASE_METHOD(CollectionTest_Cpp, "C++ Delete Indexes", "[Collection]") {
    Collection col = db.createCollection("colA", "scopeA");
    REQUIRE(col);
    
    CBLValueIndexConfiguration index1 = {kCBLN1QLLanguage, "id"_sl};
    col.createValueIndex("index1", index1);
    
    CBLValueIndexConfiguration index2 = {kCBLN1QLLanguage, "firstname, lastname"_sl};
    col.createValueIndex("index2", index2);
    
    RetainedArray names = col.getIndexNames();
    REQUIRE(names);
    CHECK(names.toJSONString() == R"(["index1","index2"])");
    
    col.deleteIndex("index1");
    names = col.getIndexNames();
    REQUIRE(names);
    CHECK(names.toJSONString() == R"(["index2"])");
    
    col.deleteIndex("index2");
    names = col.getIndexNames();
    REQUIRE(names);
    CHECK(names.toJSONString() == R"([])");
}