#ifndef CRISPY_DOOM_FILE_STREAM_HPP
#define CRISPY_DOOM_FILE_STREAM_HPP

#include "savegame/open_mode.hpp"
#include "savegame/save_file.hpp"

#include <array>
#include <sstream>

template <size_t N = 2> struct FileStream
{
    using Buffer = std::array<byte, N>;

    static std::ios::openmode toStd(OpenMode mode)
    {
        return mode == OpenMode::Read ? std::ios::in : std::ios::out;
    }

    FileStream(std::stringstream const& source, OpenMode openMode)
        : m_mode{openMode}
        , m_stream{source.str(), toStd(openMode) | std::ios::binary}
    {
    }

    FileStream(OpenMode openMode)
        : m_mode{openMode}
        , m_stream(toStd(openMode) | std::ios::binary)
    {
    }

    FileStream(Buffer buffer, OpenMode open_mode)
        : m_mode{open_mode}
        , m_stream{std::string{reinterpret_cast<char*>(buffer.data()),
                               buffer.size()},
                   toStd(open_mode) | std::ios::binary}
    {
    }

    [[nodiscard]] constexpr size_t size() const { return N; }

    void throwOnFail() { m_stream.exceptions(std::ios::failbit); }
    [[nodiscard]] std::stringstream& sstream() { return m_stream; }

    [[nodiscard]] operator SaveFile::FileVariant()
    {
        if (m_mode == OpenMode::Read)
            return static_cast<std::istream*>(&m_stream);
        return static_cast<std::ostream*>(&m_stream);
    }

    template <typename T> [[nodiscard]] T as() const
    {
        return *reinterpret_cast<T const*>(m_stream.str().data());
    }

    std::string str() const { return m_stream.str().c_str(); }

  private:
    OpenMode          m_mode;
    std::stringstream m_stream;
};

#endif // CRISPY_DOOM_FILE_STREAM_HPP
