﻿/*
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

#pragma once

#include <glm/glm.hpp>

/**
 * @brief Represents a rectangle fields. Useful for margin and padding around AViews.
 * @ingroup core
 */
struct ABoxFields
{
    int left = 0;
    int right = 0;
    int top = 0;
    int bottom = 0;


    [[nodiscard]]
    glm::ivec2 leftTop() const noexcept {
        return { left, top };
    }

    [[nodiscard]]
    glm::ivec2 rightTop() const noexcept {
        return { right, top };
    }

    [[nodiscard]]
    glm::ivec2 leftBottom() const noexcept {
        return { left, bottom };
    }

    [[nodiscard]]
    glm::ivec2 rightBottom() const noexcept {
        return { right, bottom };
    }

    [[nodiscard]]
    int horizontal() const noexcept
    {
        return left + right;
    }

    [[nodiscard]]
    int vertical() const noexcept
    {
        return top + bottom;
    }
};
