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
// Created by alex2 on 5/23/2021.
//


#include <AUI/Platform/AClipboard.h>
#include "AAbstractTypeableView.h"


#include "AUI/Platform/Platform.h"
#include "AUI/Render/Render.h"
#include "AUI/Render/RenderHints.h"
#include "AUI/Util/ARaiiHelper.h"
#include <AUI/Util/AMetric.h>
#include <AUI/Action/AMenu.h>
#include <AUI/Util/kAUI.h>
#include <AUI/i18n/AI18n.h>

ATimer& AAbstractTypeableView::blinkTimer()
{
    static ATimer t(500);
    if (!t.isStarted())
    {
        t.start();
    }
    return t;
}

void AAbstractTypeableView::updateCursorBlinking()
{
    blinkTimer().restart();
    mCursorBlinkVisible = true;
    mCursorBlinkCount = 0;
    redraw();
}

void AAbstractTypeableView::updateCursorPos()
{
    auto absoluteCursorPos = -mHorizontalScroll + int(getFontStyle().getWidth(getDisplayText().substr(0, mCursorIndex)));

    const int SCROLL_ADVANCEMENT = getContentWidth() * 4 / 10;

    if (absoluteCursorPos < 0)
    {
        mHorizontalScroll += absoluteCursorPos - SCROLL_ADVANCEMENT;
    }
    else if (absoluteCursorPos >= getContentWidth())
    {
        mHorizontalScroll += absoluteCursorPos - getContentWidth() + SCROLL_ADVANCEMENT;
    }
    mHorizontalScroll = glm::clamp(mHorizontalScroll, 0, glm::max(int(getFontStyle().getWidth(getDisplayText())) - getContentWidth() + 1, 0));
}



AAbstractTypeableView::AAbstractTypeableView()
{
    connect(blinkTimer().fired, this, [&]()
    {
        if (hasFocus() && mCursorBlinkCount < 60) {
            mCursorBlinkVisible = !mCursorBlinkVisible;
            mCursorBlinkCount += 1;
            redraw();
        }
    });

}

void AAbstractTypeableView::onFocusAcquired() {
    AView::onFocusAcquired();
    updateCursorBlinking();
}

AAbstractTypeableView::~AAbstractTypeableView()
{
}

bool AAbstractTypeableView::handlesNonMouseNavigation() {
    return true;
}

int AAbstractTypeableView::getContentMinimumHeight()
{
    return getFontStyle().size;
}

void AAbstractTypeableView::onKeyDown(AInput::Key key)
{
    AView::onKeyDown(key);
    onKeyRepeat(key);
}

void AAbstractTypeableView::onKeyRepeat(AInput::Key key)
{
    if (AInput::isKeyDown(AInput::LBUTTON))
        return;


    auto fastenSelection = [&]() {
        if (!AInput::isKeyDown(AInput::LSHIFT) && !AInput::isKeyDown(AInput::RSHIFT)) {
            mCursorSelection = -1;
        } else if (mCursorSelection == -1)
        {
            mCursorSelection = mCursorIndex;
        }
    };

    mTextChangedFlag = true;
    switch (key)
    {
        case AInput::DEL:
            if (hasSelection()) {
                auto sel = selection();
                typeableErase(sel.begin, sel.end);
                invalidatePrerenderedString();
                mCursorSelection = -1;
                mCursorIndex = sel.begin;
            } else
            {
                if (mCursorIndex < length())
                {
                    typeableErase(mCursorIndex, mCursorIndex + 1);
                    invalidatePrerenderedString();
                }
            }
            break;

        case AInput::LEFT:
            fastenSelection();
            if (mCursorIndex) {
                if (AInput::isKeyDown(AInput::LCONTROL)) {
                    if (mCursorIndex <= 1) {
                        mCursorIndex = 0;
                    } else {
                        mCursorIndex = typeableReverseFind(' ', mCursorIndex - 2) + 1;
                    }
                } else {
                    mCursorIndex -= 1;
                }
            }
            break;

        case AInput::RIGHT:
            fastenSelection();
            if (mCursorIndex < length()) {
                if (AInput::isKeyDown(AInput::LCONTROL)) {
                    auto index = typeableFind(' ', mCursorIndex);
                    if (index == AString::NPOS) {
                        mCursorIndex = length();
                    } else {
                        mCursorIndex = index + 1;
                    }
                } else {
                    mCursorIndex += 1;
                }
            }
            break;

        case AInput::HOME:
            fastenSelection();
            mCursorIndex = 0;
            break;
        case AInput::END:
            fastenSelection();
            mCursorIndex = length();
            break;

        default:
            if (AInput::isKeyDown(AInput::LCONTROL) || AInput::isKeyDown(AInput::RCONTROL)) {
                switch (key) {
                    case AInput::A: // select all
                        selectAll();
                        break;

                    case AInput::C: // copy
                        copyToClipboard();
                        break;

                    case AInput::X: // cut
                        cutToClipboard();
                        break;

                    case AInput::V: // paste
                        pasteFromClipboard();
                        break;

                    default:
                        return;
                }
            } else {
                return;
            }
    }

    if (textChanging) {
        emit textChanging(text());
    }

    updateCursorPos();
    updateCursorBlinking();

    redraw();
}

void AAbstractTypeableView::pasteFromClipboard() {
    auto pastePos = mCursorIndex;
    std::optional<AString> prevContents;
    if (mCursorSelection != -1) {
        prevContents = text();
        auto sel = selection();
        pastePos = sel.begin;
        typeableErase(sel.begin, sel.end);
    }
    auto toPaste = AClipboard::pasteFromClipboard();
    if (mMaxTextLength <= length())
        return;
    if (!mIsMultiline) {
        toPaste = toPaste.replacedAll("\n", "");
    }
    if (typeableInsert(pastePos, toPaste)) {
        mCursorIndex = pastePos + toPaste.length();
        mCursorSelection = -1;

        invalidatePrerenderedString();
        updateCursorPos();
        emit textChanged;
    } else if (prevContents) {
        setText(*prevContents);
    }
}

void AAbstractTypeableView::cutToClipboard() {
    auto sel = selection();
    AClipboard::copyToClipboard(selectedText());
    typeableErase(sel.begin, sel.end);
    mCursorIndex = sel.begin;
    mCursorSelection = -1;
    invalidatePrerenderedString();
}

void AAbstractTypeableView::copyToClipboard() const { AClipboard::copyToClipboard(selectedText()); }

void AAbstractTypeableView::selectAll() { ACursorSelectable::selectAll(); }

void AAbstractTypeableView::enterChar(wchar_t c)
{
    if (AInput::isKeyDown(AInput::LBUTTON) ||
        AInput::isKeyDown(AInput::LCONTROL) ||
        AInput::isKeyDown(AInput::RCONTROL) ||
        c == '\t')
        return;
    if (c == '\r') {
        c = '\n';
    }

    mTextChangedFlag = true;
    auto cursorIndexCopy = mCursorIndex;

    if (hasSelection()) {
        auto sel = selection();
        typeableErase(sel.begin, sel.end);

        switch (c)
        {
            case '\b':
                mCursorIndex = sel.begin;
                break;
            default:
                if (typeableInsert(sel.begin, c)) {
                    mCursorIndex = sel.begin + 1;
                } else {
                    mCursorIndex = sel.begin;
                }
        }
        mCursorSelection = -1;
    } else {
        switch (c)
        {
            case '\b':
                if (mCursorIndex != 0) {
                    typeableErase(mCursorIndex - 1, mCursorIndex);
                    mCursorIndex -= 1;
                }
                break;
            default:
                if (mMaxTextLength <= length())
                    return;
                if (!typeableInsert(mCursorIndex, c)) {
                    return;
                }
                ++mCursorIndex;

        }
    }
    invalidatePrerenderedString();
    updateCursorBlinking();
    updateCursorPos();

    if (!AInput::isKeyDown(AInput::LSHIFT) && !AInput::isKeyDown(AInput::RSHIFT))
    {
        mCursorSelection = -1;
    }

    redraw();
}

void AAbstractTypeableView::onFocusLost()
{
    AView::onFocusLost();
    if (mTextChangedFlag)
    {
        mTextChangedFlag = false;
        if (textChanged) {
            emit textChanged(text());
        }
    }

}

void AAbstractTypeableView::onMousePressed(glm::ivec2 pos, AInput::Key button)
{
    AView::onMousePressed(pos, button);
    ACursorSelectable::handleMousePressed(pos, button);
    updateCursorBlinking();
}

void AAbstractTypeableView::onMouseMove(glm::ivec2 pos)
{
    AView::onMouseMove(pos);
    ACursorSelectable::handleMouseMove(pos);
}

bool AAbstractTypeableView::isLButtonPressed() {
    return isMousePressed();
}

void AAbstractTypeableView::onMouseReleased(glm::ivec2 pos, AInput::Key button)
{
    AView::onMouseReleased(pos, button);

    if (button == AInput::RBUTTON) {
        AMenu::show({
                            { "aui.cut"_i18n, [&]{cutToClipboard();}, AInput::LCONTROL + AInput::X, hasSelection() },
                            { "aui.copy"_i18n, [&]{copyToClipboard();}, AInput::LCONTROL + AInput::C, hasSelection() },
                            { "aui.paste"_i18n, [&]{pasteFromClipboard();}, AInput::LCONTROL + AInput::V, !AClipboard::isEmpty() },
                            AMenu::SEPARATOR,
                            { "aui.select_all"_i18n, [&]{selectAll();}, AInput::LCONTROL + AInput::A, !text().empty() }
                    });
    } else {
        ACursorSelectable::handleMouseReleased(pos, button);
    }
}
void AAbstractTypeableView::setText(const AString& t)
{
    mHorizontalScroll = 0;
    updateSelectionOnTextSet(t);
    updateCursorBlinking();

    invalidatePrerenderedString();
    emit textChanged(t);
}

void AAbstractTypeableView::updateSelectionOnTextSet(const AString& t) {
    mCursorIndex = t.length();
    mCursorSelection = 0;
}

void AAbstractTypeableView::onMouseDoubleClicked(glm::ivec2 pos, AInput::Key button) {
    AView::onMouseDoubleClicked(pos, button);
    ACursorSelectable::handleMouseDoubleClicked(pos, button);
    updateCursorBlinking();
}

glm::ivec2 AAbstractTypeableView::getMouseSelectionPadding() {
    return {mPadding.left, mPadding.top + getVerticalAlignmentOffset() };
}

glm::ivec2 AAbstractTypeableView::getMouseSelectionScroll() {
    return {mHorizontalScroll, 0};
}

AFontStyle AAbstractTypeableView::getMouseSelectionFont() {
    return getFontStyle();
}

AString AAbstractTypeableView::getDisplayText() {
    return text();
}


void AAbstractTypeableView::doRedraw() {
    redraw();
}