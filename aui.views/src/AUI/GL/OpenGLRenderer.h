#pragma once


#include <AUI/GL/Shader.h>
#include <AUI/GL/Vao.h>
#include "AUI/Render/IRenderer.h"

class OpenGLRenderer: public IRenderer {
friend class OpenGLPrerenderedString;
friend class OpenGLMultiStringCanvas;
public:
    struct FontEntryData {
        Util::SimpleTexturePacker texturePacker;
        gl::Texture2D texture;
        bool isTextureInvalid = true;
    };

private:
    gl::Shader mSolidShader;
    gl::Shader mGradientShader;
    gl::Shader mRoundedSolidShader;
    gl::Shader mRoundedSolidShaderAntialiased;
    gl::Shader mRoundedSolidShaderAntialiasedBorder;
    gl::Shader mRoundedGradientShaderAntialiased;
    gl::Shader mSolidTransformShader;
    gl::Shader mBoxShadowShader;
    gl::Shader mTexturedShader;
    gl::Shader mSymbolShader;
    gl::Shader mSymbolShaderSubPixel;
    gl::Vao mTempVao;


    struct CharacterData {
        glm::vec4 uv;
    };

    ADeque<CharacterData> mCharData;
    ADeque<FontEntryData> mFontEntryData;


    AVector<glm::vec3> getVerticesForRect(const glm::vec2& position,
                                          const glm::vec2& size);

    void uploadToShaderCommon();

    void endDraw(const ABrush& brush);
    FontEntryData* getFontEntryData(const AFontStyle& fontStyle);
protected:
    ITexture* createNewTexture() override;

public:
    OpenGLRenderer();
    ~OpenGLRenderer() override = default;
    bool isVaoAvailable();

    void drawRect(const ABrush& brush,
                  const glm::vec2& position,
                  const glm::vec2& size) override;

    void drawRoundedRect(const ABrush& brush,
                         const glm::vec2& position,
                         const glm::vec2& size,
                         float radius) override;

    void drawRoundedRectAntialiased(const ABrush& brush,
                                    const glm::vec2& position,
                                    const glm::vec2& size,
                                    float radius) override;

    void drawRectBorder(const ABrush& brush,
                       const glm::vec2& position,
                       const glm::vec2& size,
                       float lineWidth) override;

    void drawRectBorder(const ABrush& brush,
                        const glm::vec2& position,
                        const glm::vec2& size,
                        float radius,
                        int borderWidth) override;

    void drawBoxShadow(const glm::vec2& position,
                       const glm::vec2& size,
                       float blurRadius,
                       const AColor& color) override;

    void drawString(const glm::vec2& position,
                    const AString& string,
                    const AFontStyle& fs) override;

    _<IPrerenderedString> prerenderString(const glm::vec2& position, const AString& text, const AFontStyle& fs) override;

    void drawRectImpl(const glm::vec2& position, const glm::vec2& size);

    void setBlending(Blending blending) override;

    _<IMultiStringCanvas> newMultiStringCanvas(const AFontStyle& style) override;

    glm::mat4 getProjectionMatrix() const override;

    void drawLine(const ABrush& brush, glm::vec2 p1, glm::vec2 p2) override;

    void drawLines(const ABrush& brush, AArrayView<glm::vec2> points) override;

    void drawLines(const ABrush& brush, AArrayView<std::pair<glm::vec2, glm::vec2>> points) override;

    void pushMaskBefore() override;
    void pushMaskAfter() override;
    void popMaskBefore() override;
    void popMaskAfter() override;
};


