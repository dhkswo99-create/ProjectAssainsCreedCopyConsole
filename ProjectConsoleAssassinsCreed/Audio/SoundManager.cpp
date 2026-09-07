#include "SoundManager.h"
#include <cstring>
#include <fstream>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "ole32.lib")

namespace
{
    WORD U16(const BYTE* p) { return WORD(p[0] | (WORD(p[1]) << 8)); }
    DWORD U32(const BYTE* p)
    {
        return DWORD(p[0]) | (DWORD(p[1]) << 8) |
            (DWORD(p[2]) << 16) | (DWORD(p[3]) << 24);
    }
}

SoundManager::~SoundManager() { Shutdown(); }

bool SoundManager::Fail(const std::wstring& message)
{
    lastError_ = message;
    OutputDebugStringW((L"[Audio] " + message + L"\n").c_str());
    return false;
}

bool SoundManager::Initialize()
{
    if (engine_) return true;
    const HRESULT co = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(co) && co != RPC_E_CHANGED_MODE)
        return Fail(L"CoInitializeEx failed: " + std::to_wstring(co));
    comInitialized_ = SUCCEEDED(co);
    HRESULT hr = XAudio2Create(engine_.GetAddressOf());
    if (SUCCEEDED(hr)) hr = engine_->CreateMasteringVoice(&master_);
    if (FAILED(hr))
    {
        Shutdown();
        return Fail(L"XAudio2 initialization failed: " + std::to_wstring(hr));
    }
    return true;
}

std::filesystem::path SoundManager::Resolve(const wchar_t* filename) const
{
    namespace fs = std::filesystem;
    const fs::path name(filename);
    std::error_code ec;
    if (name.is_absolute()) return name;
    wchar_t exe[32768]{};
    const DWORD length = GetModuleFileNameW(nullptr, exe, 32768);
    if (length > 0 && length < 32768)
    {
        const auto dir = fs::path(exe).parent_path();
        // Existing project copies Assets next to the configuration directory.
        for (const auto& root : { dir / L"Sound", dir / L"../../../sound", dir / L"../Assets/Sound" })
        {
            auto path = (root / name).lexically_normal();
            if (fs::is_regular_file(path, ec)) return path;
        }
    }
    for (const auto& root : { fs::path(L"../Assets/Sound"), fs::path(L"../sound"), fs::path(L"sound"), fs::path(L"Assets/Sound") })
    {
        auto path = root / name;
        if (fs::is_regular_file(path, ec)) return path;
    }
    return fs::path(L"../Assets/Sound") / name;
}

std::shared_ptr<SoundManager::SoundData> SoundManager::ReadWav(const std::filesystem::path& path)
{
    const auto invalid = [&]() -> std::shared_ptr<SoundData>
        {
            Fail(L"Invalid/unsupported WAV (use PCM or Float): " + path.wstring());
            return nullptr;
        };
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) { Fail(L"Cannot open WAV: " + path.wstring()); return nullptr; }
    const auto length = file.tellg();
    // Short effects are preloaded, not streamed.
    if (length < 12 || length > 128 * 1024 * 1024) return invalid();
    std::vector<BYTE> bytes(static_cast<size_t>(length));
    file.seekg(0);
    if (!file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()))) return invalid();
    if (std::memcmp(bytes.data(), "RIFF", 4) || std::memcmp(bytes.data() + 8, "WAVE", 4)) return invalid();
    const uint64_t limit = uint64_t(U32(bytes.data() + 4)) + 8;
    if (limit < 12 || limit > bytes.size()) return invalid();
    auto sound = std::make_shared<SoundData>();
    bool hasFormat = false, hasData = false;
    for (uint64_t p = 12; p + 8 <= limit;)
    {
        const BYTE* chunk = bytes.data() + static_cast<size_t>(p);
        const DWORD size = U32(chunk + 4);
        const uint64_t end = p + 8 + size;
        if (end > limit) return invalid();
        const BYTE* data = chunk + 8;
        if (!std::memcmp(chunk, "fmt ", 4) && !hasFormat)
        {
            if (size < 16) return invalid();
            auto& fmt = sound->format.Format;
            fmt.wFormatTag = U16(data);
            fmt.nChannels = U16(data + 2);
            fmt.nSamplesPerSec = U32(data + 4);
            fmt.nAvgBytesPerSec = U32(data + 8);
            fmt.nBlockAlign = U16(data + 12);
            fmt.wBitsPerSample = U16(data + 14);
            WORD type = fmt.wFormatTag;
            if (type == WAVE_FORMAT_EXTENSIBLE)
            {
                if (size < 40 || U16(data + 16) < 22 || DWORD(U16(data + 16)) + 18 > size) return invalid();
                std::memcpy(&sound->format, data, 40);
                fmt.cbSize = 22;
                const BYTE tail[] = { 0,0,0,0,16,0,128,0,0,170,0,56,155,113 };
                if (std::memcmp(data + 26, tail, sizeof(tail))) return invalid();
                type = U16(data + 24);
                if (!sound->format.Samples.wValidBitsPerSample || sound->format.Samples.wValidBitsPerSample > fmt.wBitsPerSample) return invalid();
            }
            else fmt.cbSize = 0;
            if (type != WAVE_FORMAT_PCM && type != WAVE_FORMAT_IEEE_FLOAT) return invalid();
            if (type == WAVE_FORMAT_PCM && fmt.wBitsPerSample != 8 && fmt.wBitsPerSample != 16 && fmt.wBitsPerSample != 24 && fmt.wBitsPerSample != 32) return invalid();
            if (type == WAVE_FORMAT_IEEE_FLOAT && fmt.wBitsPerSample != 32) return invalid();
            if (!fmt.nChannels || fmt.nChannels > XAUDIO2_MAX_AUDIO_CHANNELS || fmt.nSamplesPerSec < XAUDIO2_MIN_SAMPLE_RATE || fmt.nSamplesPerSec > XAUDIO2_MAX_SAMPLE_RATE) return invalid();
            if (fmt.nBlockAlign != fmt.nChannels * (fmt.wBitsPerSample / 8) || fmt.nAvgBytesPerSec != fmt.nSamplesPerSec * fmt.nBlockAlign) return invalid();
            hasFormat = true;
        }
        else if (!std::memcmp(chunk, "data", 4) && !hasData)
        {
            sound->samples.assign(data, data + size);
            hasData = true;
        }
        p = end + (size & 1);
    }
    if (!hasFormat || !hasData || sound->samples.empty() || sound->samples.size() % sound->format.Format.nBlockAlign) return invalid();
    return sound;
}

bool SoundManager::Load(const wchar_t* filename)
{
    if (!filename || !*filename) return Fail(L"Empty filename");
    if (sounds_.contains(filename)) return true;
    auto data = ReadWav(Resolve(filename));
    if (!data) return false;
    sounds_.emplace(filename, std::move(data));
    return true;
}

bool SoundManager::Play(const wchar_t* filename)
{
    if (!engine_) return Fail(L"XAudio2 not initialized");
    if (!filename) return Fail(L"Empty filename");
    const auto it = sounds_.find(filename);
    if (it == sounds_.end()) return Fail(L"Sound was not loaded: " + std::wstring(filename));
    Update();
    for (auto& slot : playing_)
    {
        if (slot.voice) continue;
        slot.data = it->second;
        HRESULT hr = engine_->CreateSourceVoice(&slot.voice, &slot.data->format.Format);
        XAUDIO2_BUFFER buffer{};
        buffer.AudioBytes = static_cast<UINT32>(slot.data->samples.size());
        buffer.pAudioData = slot.data->samples.data();
        buffer.Flags = XAUDIO2_END_OF_STREAM;
        if (SUCCEEDED(hr)) hr = slot.voice->SubmitSourceBuffer(&buffer);
        if (SUCCEEDED(hr)) hr = slot.voice->Start();
        if (FAILED(hr))
        {
            if (slot.voice) slot.voice->DestroyVoice();
            slot = {};
            return Fail(L"Voice playback failed: " + std::to_wstring(hr));
        }
        return true;
    }
    return false; // All 32 voices busy: leave existing effects playing.
}

void SoundManager::Update()
{
    for (auto& slot : playing_)
    {
        if (!slot.voice) continue;
        XAUDIO2_VOICE_STATE state{};
        slot.voice->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
        if (!state.BuffersQueued)
        {
            slot.voice->DestroyVoice();
            slot = {};
        }
    }
}

void SoundManager::Shutdown()
{
    for (auto& slot : playing_)
    {
        if (slot.voice) slot.voice->DestroyVoice();
        slot = {};
    }
    sounds_.clear();
    if (master_) { master_->DestroyVoice(); master_ = nullptr; }
    engine_.Reset();
    if (comInitialized_) { CoUninitialize(); comInitialized_ = false; }
}
