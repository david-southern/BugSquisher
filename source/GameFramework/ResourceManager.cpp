#include "ResourceManager.h"

#include <exception>

ResourceManager::ResourceManager()
{
    // Setting default frequency at which all channels will play at, in Hz.
    s3eSoundSetInt(S3E_SOUND_DEFAULT_FREQ, 12000);

    groups = new std::vector<CIwResGroup *>;
    images = new std::vector<CIw2DImage *>;
    fonts = new std::vector<CIw2DFont *>;
    audioSizes = new std::vector<int32>;
    audioData = new std::vector<int16 *>;
    texts = new std::vector<std::string>;

    images->reserve(100);
    texts->reserve(100);

    // Consume the first slot of all vectors so that we can report index 0 as an error condition
    images->push_back(null);
    fonts->push_back(null);
    audioSizes->push_back(0);
    audioData->push_back(null);
    texts->push_back("");

    fontInitialized = false;
}

ResourceManager::~ResourceManager()
{
    uint32 index;

    for(index = 0; index < images->size(); index++)
    {
        delete images->at(index);
    }

    for(index = 0; index < fonts->size(); index++)
    {
        delete fonts->at(index);
    }

    for(index = 0; index < audioData->size(); index++)
    {
        s3eFreeBase(audioData->at(index));
    }

    for(index = 0; index < groups->size(); index++)
    {
        IwGetResManager()->DestroyGroup(groups->at(index));
    }

    delete groups;
    delete images;
    delete fonts;
    delete audioSizes;
    delete audioData;
    delete texts;
}

void ResourceManager::RegisterResourceGroup(std::string groupName)
{
    groups->push_back(IwGetResManager()->LoadGroup(groupName.c_str()));
}

uint32 ResourceManager::RegisterImage(std::string imageName)
{
    CIw2DImage *image = Iw2DCreateImageResource(imageName.c_str());

    if(image == null)
    {
        std::string errMsg = "Unknown image resource requested: '";
        errMsg += imageName + "'";
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, errMsg.c_str());
        return 0;
    }

    int handle = images->size();
    images->push_back(image);
    return handle;
}

uint32 ResourceManager::RegisterFont(std::string fontName)
{
    CIw2DFont *font = Iw2DCreateFontResource(fontName.c_str());

    if(font == null)
    {
        std::string errMsg = "Unknown font resource requested: '";
        errMsg += fontName + "'";
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, errMsg.c_str());
        return 0;
    }

    int handle = fonts->size();
    fonts->push_back(font);

    if(!fontInitialized)
    {
        Iw2DSetFont(font);
        fontInitialized = true;
    }

    return handle;
}

uint32 ResourceManager::RegisterAudio(std::string audioName)
{
    s3eFile *fileHandle = s3eFileOpen(audioName.c_str(), "rb");

    if(fileHandle == null)
    {
        std::string errMsg = "Unknown audio resource requested: '";
        errMsg += audioName + "'";
        s3eDebugAssertShow(S3E_MESSAGE_CONTINUE, errMsg.c_str());
        return 0;
    }

    int soundSize = s3eFileGetSize(fileHandle);
    int16 *soundData = (int16*)s3eMallocBase(soundSize);  
    memset(soundData, 0, soundSize);
    s3eFileRead(soundData, soundSize, 1, fileHandle);  
    s3eFileClose(fileHandle);

    int handle = audioData->size();
    audioData->push_back(soundData);
    audioSizes->push_back(soundSize);

    return handle;
}

uint32 ResourceManager::RegisterText(std::string localizedText)
{
    int handle = texts->size();
    texts->push_back(localizedText);
    return handle;
}

CIw2DImage *ResourceManager::GetImage(uint32 imageHandle)
{
    if(imageHandle == 0 || imageHandle >= images->size())
    {
        return null;
    }

    return images->at(imageHandle);
}

CIw2DFont* ResourceManager::GetFont(uint32 fontHandle)
{
    if(fontHandle == 0 || fontHandle >= fonts->size())
    {
        return null;
    }

    return fonts->at(fontHandle);
}

void ResourceManager::SelectFont(int fontHandle)
{
    Iw2DSetFont(GetFont(fontHandle));
}

void ResourceManager::PlayAudio(uint32 audioHandle)
{
    if(audioHandle == 0 || audioHandle >= audioData->size() || audioData->at(audioHandle) == null)
    {
        return;
    }

    int channel = s3eSoundGetFreeChannel();

    s3eSoundChannelPlay(channel, audioData->at(audioHandle), audioSizes->at(audioHandle)/2, 1, 0);
}

std::string ResourceManager::GetText(uint32 textHandle)
{
    if(textHandle == 0 || textHandle >= texts->size())
    {
        return null;
    }

    return texts->at(textHandle);
}

