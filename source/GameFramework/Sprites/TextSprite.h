#pragma once


#include "s3e.h"
#include "Iw2D.h"

#include "Sprite.h"

class TextSprite : public Sprite
{
    CIw2DFontAlign horizontalAlignment;
    CIw2DFont *textFont;
    CIwColour textColor, backgroundColor, borderColor;
    IColorSource *textColorSource, *backgroundColorSource, *borderColorSource;
    CIwVec2 textPosition, textSize, borderPosition, borderSize, innerBorderPosition, innerBorderSize, backgroundPosition, backgroundSize;
    CIwVec2 margin, padding, borderThickness;
    IIntSource *borderXSource, *borderYSource;
    IIntSource *marginXSource, *marginYSource;
    IIntSource *paddingXSource, *paddingYSource;
    std::string text;
    virtual void RenderImage(uint32 gameTime);

    void RecalculateTextData();
    void UpdatePositionData();

public:
    TextSprite(GameManager *gameManager, const char *_text);
    ~TextSprite();

    int32 GetWidth();
    int32 GetHeight();

    const char *GetText();
    void SetText(const char *);

    CIwColour GetColor();
    void SetColor(CIwColour color);
    void SetColorSource(IColorSource *colorSource);
    CIwColour GetBorderColor();
    void SetBorderColor(CIwColour color);
    void SetBorderColorSource(IColorSource *colorSource);
    CIwColour GetBackgroundColor();
    void SetBackgroundColor(CIwColour color);
    void SetBackgroundColorSource(IColorSource *colorSource);

    CIwVec2 GetBorderThickness();
    void SetBorderThickness(CIwVec2 thickness);
    void SetBorderThicknessSource(IIntSource *xSource, IIntSource *ySource);
    CIwVec2 GetMargin();
    void SetMargin(CIwVec2 _margin);
    void SetMarginSource(IIntSource *xSource, IIntSource *ySource);
    CIwVec2 GetPadding();
    void SetPadding(CIwVec2 _padding);
    void SetPaddingSource(IIntSource *xSource, IIntSource *ySource);

    CIw2DFont *GetFont();
    void SetFont(CIw2DFont *newFont);

    CIw2DFontAlign GetAlign();
    void SetAlign(CIw2DFontAlign newAlign);
};


