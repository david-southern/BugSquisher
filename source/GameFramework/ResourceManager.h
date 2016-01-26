#pragma once


#include <string>
#include <vector>

#include "s3e.h"
#include "Iw2D.h"
#include "IwResManager.h"

#include "Constants.h"

class GameManager;

class ResourceManager
{
private :
    ResourceManager();
    ~ResourceManager();

    std::vector<CIwResGroup *> *groups;
    std::vector<CIw2DImage *> *images;
    std::vector<CIw2DFont *> *fonts;
    std::vector<int32> *audioSizes;
    std::vector<int16 *> *audioData;
    std::vector<std::string> *texts;

    bool fontInitialized;

public:
    void RegisterResourceGroup(std::string groupName);

    uint32 RegisterImage(std::string imageName);
    uint32 RegisterFont(std::string fontName);
    uint32 RegisterAudio(std::string audioName);
    uint32 RegisterText(std::string textName);

    CIw2DImage *GetImage(uint32 imageHandle);
    CIw2DFont* GetFont(uint32 fontHandle);
    std::string GetText(uint32 textHandle);

    void SelectFont(int fontHandle);
    void PlayAudio(uint32 audioHandle);


    friend class GameManager;
};

