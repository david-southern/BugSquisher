#include <vector>

#include "s3e.h"
#include "Iw2D.h"

#include "TextSprite.h"

TextSprite::TextSprite(GameManager *gameManager, const char *_text) : Sprite(gameManager)
{

    text = _text;
    textFont = null;
    horizontalAlignment = IW_2D_FONT_ALIGN_LEFT;

    textColorSource = null;
    backgroundColorSource = null;
    borderColorSource = null;
    borderXSource = null;
    borderYSource = null;
    marginXSource = null;
    marginYSource = null;
    paddingXSource = null;
    paddingYSource = null;

    margin = CIwVec2(0, 0);
    padding = CIwVec2(0, 0);
    borderThickness = CIwVec2(0, 0);

    textColor.Set(0, 0, 0);
    borderColor.Set(0, 0, 0, 0);
    backgroundColor.Set(0, 0, 0, 0);
    
    RecalculateTextData();
}

TextSprite::~TextSprite()
{
    if(textColorSource != null) delete textColorSource;
    if(backgroundColorSource != null) delete backgroundColorSource;
    if(borderColorSource != null) delete borderColorSource;
    if(borderXSource != null) delete borderXSource;
    if(borderYSource != null) delete borderYSource;
    if(marginXSource != null) delete marginXSource;
    if(marginYSource != null) delete marginYSource;
    if(paddingXSource != null) delete paddingXSource;
    if(paddingYSource != null) delete paddingYSource;
}

void TextSprite::RecalculateTextData()
{
    CIw2DFont *prevFont = Iw2DGetFont();

    if(textFont != null)
    {
        Iw2DSetFont(textFont);
    }
    else
    {
        textFont = prevFont;
    }

    int newlineCount = 1;
    char *checkChar = (char *)this->text.c_str();
    while(*checkChar != '\0')
    {
        if(*checkChar++ == '\n')
        {
            newlineCount++;
        }
    }

    textSize = SCREEN_TO_SUBPIXEL(CIwSVec2(Iw2DGetStringWidth(text.c_str()), textFont->GetHeight() * newlineCount));

    UpdatePositionData();

    if(prevFont != null)
    {
        Iw2DSetFont(prevFont);
    }
}

void TextSprite::UpdatePositionData()
{
    textPosition = margin + borderThickness + padding;

    innerBorderPosition = margin + borderThickness;
    innerBorderSize = textSize + padding + padding;

    borderPosition = margin;
    borderSize = textSize + padding + padding + borderThickness + borderThickness;

    backgroundPosition = CIwVec2(0, 0);
    backgroundSize = textSize + padding + padding + margin + margin + borderThickness + borderThickness;
}

int32 TextSprite::GetWidth()
{
    return SUBPIXEL_TO_SCREEN(backgroundSize.x);
}

int32 TextSprite::GetHeight()
{
    return SUBPIXEL_TO_SCREEN(backgroundSize.y);
}

const char *TextSprite::GetText()
{
    return text.c_str();
}

void TextSprite::SetText(const char *newText)
{
    text = newText;
    RecalculateTextData();
}

CIwColour TextSprite::GetColor()
{
    return textColor;
}

void TextSprite::SetColor(CIwColour color)
{
    textColor = color;
    SetColorSource(null);
}

void TextSprite::SetColorSource(IColorSource *colorSource)
{
    if(textColorSource != null)
    {
        delete textColorSource;
    }
    textColorSource = colorSource;
}

CIwColour TextSprite::GetBorderColor()
{
    return borderColor;
}

void TextSprite::SetBorderColor(CIwColour color)
{
    borderColor = color;
    SetBorderColorSource(null);
}

void TextSprite::SetBorderColorSource(IColorSource *colorSource)
{
    if(borderColorSource != null)
    {
        delete borderColorSource;
    }
    borderColorSource = colorSource;
}

CIwColour TextSprite::GetBackgroundColor()
{
    return backgroundColor;
}

void TextSprite::SetBackgroundColor(CIwColour color)
{
    backgroundColor = color;
    SetBackgroundColorSource(null);
}

void TextSprite::SetBackgroundColorSource(IColorSource *colorSource)
{
    if(backgroundColorSource != null)
    {
        delete backgroundColorSource;
    }
    backgroundColorSource = colorSource;
}

CIwVec2 TextSprite::GetBorderThickness()
{
    return SUBPIXEL_TO_SCREEN(borderThickness);
}

void TextSprite::SetBorderThickness(CIwVec2 thickness)
{
    borderThickness = SCREEN_TO_SUBPIXEL(thickness);
    SetBorderThicknessSource(null, null);
    UpdatePositionData();
}

void TextSprite::SetBorderThicknessSource(IIntSource *xSource, IIntSource *ySource)
{
    if((xSource == null) != (ySource == null))
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "SetBorderThickness: (xSource == null) != (ySource == null)");
        if(xSource != null) delete xSource;
        if(ySource != null) delete ySource;
        xSource = ySource = null;
    }

    if(borderXSource != null)
    {
        delete borderXSource;
        delete borderYSource;
    }

    borderXSource = xSource;
    borderYSource = ySource;
}


CIwVec2 TextSprite::GetMargin()
{
    return SUBPIXEL_TO_SCREEN(margin);
}

void TextSprite::SetMargin(CIwVec2 _margin)
{
    margin = SCREEN_TO_SUBPIXEL(_margin);
    SetMarginSource(null, null);
    UpdatePositionData();
}

void TextSprite::SetMarginSource(IIntSource *xSource, IIntSource *ySource)
{
    if((xSource == null) != (ySource == null))
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "SetMarginSource: (xSource == null) != (ySource == null)");
        if(xSource != null) delete xSource;
        if(ySource != null) delete ySource;
        xSource = ySource = null;
    }

    if(marginXSource != null)
    {
        delete marginXSource;
        delete marginYSource;
    }

    marginXSource = xSource;
    marginYSource = ySource;
}

CIwVec2 TextSprite::GetPadding()
{
    return SUBPIXEL_TO_SCREEN(padding);
}

void TextSprite::SetPadding(CIwVec2 _padding)
{
    padding = SCREEN_TO_SUBPIXEL(_padding);
    SetPaddingSource(null, null);
    UpdatePositionData();
}

void TextSprite::SetPaddingSource(IIntSource *xSource, IIntSource *ySource)
{
    if((xSource == null) != (ySource == null))
    {
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, "SetPaddingSource: (xSource == null) != (ySource == null)");
        if(xSource != null) delete xSource;
        if(ySource != null) delete ySource;
        xSource = ySource = null;
    }

    if(paddingXSource != null)
    {
        delete paddingXSource;
        delete paddingYSource;
    }

    paddingXSource = xSource;
    paddingYSource = ySource;
}

CIw2DFont *TextSprite::GetFont()
{
    return textFont;
}

void TextSprite::SetFont(CIw2DFont *newFont)
{
    textFont = newFont;
    RecalculateTextData();
}

CIw2DFontAlign TextSprite::GetAlign()
{
    return horizontalAlignment;
}

void TextSprite::SetAlign(CIw2DFontAlign newAlign)
{
    horizontalAlignment = newAlign;
    RecalculateTextData();
}

void TextSprite::RenderImage(uint32 gameTime)
{
    if(textColorSource != null) textColor = textColorSource->GetColor(gameTime);
    if(borderColorSource != null) borderColor = borderColorSource->GetColor(gameTime);
    if(backgroundColorSource != null) backgroundColor = backgroundColorSource->GetColor(gameTime);
    if(marginXSource != null) margin = SCREEN_TO_SUBPIXEL(CIwVec2(marginXSource->GetInt(gameTime), marginYSource->GetInt(gameTime)));
    if(paddingXSource != null) padding = SCREEN_TO_SUBPIXEL(CIwVec2(paddingXSource->GetInt(gameTime), paddingYSource->GetInt(gameTime)));
    if(borderXSource != null) borderThickness = SCREEN_TO_SUBPIXEL(CIwVec2(borderXSource->GetInt(gameTime), borderYSource->GetInt(gameTime)));

    if(marginXSource != null || paddingXSource != null || borderXSource != null)
    {
        UpdatePositionData();
    }

    CIw2DFont *prevFont = null;
    CIwColour prevColor = Iw2DGetColour();

    if(textFont != null)
    {
        prevFont = Iw2DGetFont();
        Iw2DSetFont(textFont);
    }

    if(backgroundColor.a > 0)
    {
        Iw2DSetColour(backgroundColor);
        Iw2DFillRect(CIwSVec2(backgroundPosition), CIwSVec2(backgroundSize));
    }

    if(borderColor.a > 0)
    {
        Iw2DSetColour(borderColor);
        Iw2DFillRect(CIwSVec2(borderPosition), CIwSVec2(borderSize));

        Iw2DSetColour(backgroundColor);
        Iw2DFillRect(CIwSVec2(innerBorderPosition), CIwSVec2(innerBorderSize));
    }

    Iw2DSetColour(textColor);
    
    const char *textPtr = text.c_str();

    Iw2DDrawString(textPtr, CIwSVec2(textPosition), CIwSVec2(textSize), horizontalAlignment, IW_2D_FONT_ALIGN_TOP);

    Iw2DSetColour(prevColor);

    if(prevFont != null)
    {
        Iw2DSetFont(prevFont);
    }
}