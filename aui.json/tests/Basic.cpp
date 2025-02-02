/*
 * =====================================================================================================================
 * Copyright (c) 2021 Alex2772
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 * Original code located at https://github.com/aui-framework/aui
 * =====================================================================================================================
 */

//
// Created by alex2 on 30.08.2020.
//

#include <gtest/gtest.h>
#include <AUI/Common/AString.h>
#include <AUI/Json/AJson.h>
#include <AUI/Json/AJson.h>


// check const access
void check_girlfriend(const AJson& o) {
    EXPECT_TRUE(o["girlfriend"].isNull());

    // unexisting object
    EXPECT_TRUE(o["unexisting_object"].isEmpty());
}

TEST(Json, ObjectAssignValue)
{
    // arrange data
    AJson o;
    o["name"] = "Alex2772";
    o["year"] = 2020;
    o["girlfriend"] = nullptr;

    // check for resulting json
    ASSERT_EQ(AJson::toString(o), R"({"girlfriend":null,"name":"Alex2772","year":2020})");
    check_girlfriend(o);
}

TEST(Json, ObjectAssignObject)
{
    // arrange data
    AJson o;
    o["name"] = "Alex2772";
    o["year"] = 2020;

    AJson root;
    // assign object to another object. this is what we want check for
    root["user"] = o;

    // check for resulting json
    ASSERT_EQ(AJson::toString(root), R"({"user":{"name":"Alex2772","year":2020}})");
}
TEST(Json, StringEscape)
{
    // arrange data
    AJson root;
    root["user"] = "u\"";
    auto s = AJson::toString(root);

    // check for whole composition
    ASSERT_EQ(s, "{\"user\":\"u\\\"\"}");

    // check for string itself
    auto deserialized = AJson::fromString(s);
    ASSERT_EQ(deserialized["user"].asString(), "u\"");
}

TEST(Json, BraceInitialization)
{
    // arrange data
    AJson root = {
        {"array", AJson::Array{
            "value1",
            "value2",
        }},
        {"key", 1},
    };

    // check for resulting json
    auto str = AJson::toString(root);
    ASSERT_EQ(str, R"({"array":["value1","value2"],"key":1})");
}

TEST(Json, Array)
{
    // arrange data
    AJson root = AJson::Array{1, 2, 3};

    // check for resulting json
    ASSERT_EQ(AJson::toString(root), R"([1,2,3])");

    // check push
    root.push_back(10);
    root.push_back(9);
    ASSERT_EQ(AJson::toString(root), R"([1,2,3,10,9])");
    ASSERT_EQ(root[1].asInt(), 2);
}

TEST(Json, SerializationDeserialization)
{
    // arrange data
    const char* str = R"({"a":null,"b":[1,2,3],"c":false,"d":true,"e":{"v":"123"}})";
    ASSERT_EQ(AJson::toString(AJson::fromString(str)), str);
}


