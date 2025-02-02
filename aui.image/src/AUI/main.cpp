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

#if AUI_PLATFORM_WIN
#include <windows.h>
#include <AUI/Url/AUrl.h>

#elif AUI_PLATFORM_ANDROID

#elif AUI_PLATFORM_APPLE
#else
#include <AUI/Logging/ALogger.h>

#endif


#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>
#include "AUI/Common/Plugin.h"
#include "AUI/Image/bmp/BmpImageLoader.h"
#include <AUI/Logging/ALogger.h>
#include <AUI/Image/jpg/JpgImageLoader.h>
#include <AUI/Image/png/PngImageLoader.h>
#include <AUI/Image/AImageLoaderRegistry.h>
#include <AUI/Image/svg/SvgImageLoader.h>


struct AImageInit
{
    AImageInit() {
        AImageLoaderRegistry::inst().registerImageLoader(_new<SvgImageLoader>());
        AImageLoaderRegistry::inst().registerImageLoader(_new<PngImageLoader>());
        AImageLoaderRegistry::inst().registerImageLoader(_new<JpgImageLoader>());
        AImageLoaderRegistry::inst().registerImageLoader(_new<BmpImageLoader>());
    }
} _aimageinit;