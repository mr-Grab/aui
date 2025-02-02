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
#include <AUI/Curl/ACurl.h>
#include <AUI/Util/kAUI.h>
#include <chrono>
#include <AUI/IO/AFileInputStream.h>
#include <AUI/IO/AFileOutputStream.h>
#include "AUI/IO/APath.h"

using namespace std::chrono;
using namespace std::chrono_literals;

TEST(Json, Performance) {
    AByteBuffer buffer;
    if (APath("tmp.json").isRegularFileExists()) {
        buffer = AByteBuffer::fromStream(AFileInputStream("tmp.json"));
    } else {
        buffer = ACurl::Builder("https://raw.githubusercontent.com/json-iterator/test-data/master/large-file.json").toByteBuffer();
        AFileOutputStream("tmp.json") << buffer;
    }

    auto start = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    auto j = AJson::fromBuffer(buffer);
    auto end = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch());
    auto delta = end - start;
    ASSERT_TRUE((delta < 3s)) << "too slow (" << delta.count() << "ms)";
    ASSERT_TRUE((delta > 5ms)) << "too fast (probably something went wrong)";
}
