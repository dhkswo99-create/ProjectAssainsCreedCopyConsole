#pragma once
#include <Windows.h>
#include <xaudio2.h>
#include <wrl/client.h>
#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Call only from the game thread. Audio processing runs on XAudio2 threads.
class SoundManager
{
public:
    SoundManager() = default;
    ~SoundManager();
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;
    bool Initialize();
    bool Load(const wchar_t* filename);
    bool Play(const wchar_t* filename);
    void Update();
    void Shutdown();
    const std::wstring& GetLastError() const { return lastError_; }
private:
    struct SoundData
    {
        WAVEFORMATEXTENSIBLE format{};
        std::vector<BYTE> samples;
    };
    struct Instance
    {
        IXAudio2SourceVoice* voice = nullptr;
        std::shared_ptr<const SoundData> data;
    };
    bool Fail(const std::wstring& message);
    std::filesystem::path Resolve(const wchar_t* filename) const;
    std::shared_ptr<SoundData> ReadWav(const std::filesystem::path& path);
    Microsoft::WRL::ComPtr<IXAudio2> engine_;
    IXAudio2MasteringVoice* master_ = nullptr;
    std::unordered_map<std::wstring, std::shared_ptr<const SoundData>> sounds_;
    std::array<Instance, 32> playing_{};
    std::wstring lastError_;
    bool comInitialized_ = false;
};
