#include "save_file.hpp"

#include <iostream>

OpenMode SaveFile::openMode() const
{
    return doOnFile<OpenMode>([](std::istream*) { return OpenMode::Read; },
                              [](std::ostream*) { return OpenMode::Write; });
}

bool SaveFile::stream_error() const noexcept
{
    return doOnFile<bool>([](auto const& stream) { return stream->fail(); });
}

long SaveFile::currentPosition() const noexcept
{
    using pos_type = std::ostream::pos_type;

    return doOnFile<pos_type>(
        [](std::ostream* write) { return write->tellp(); },
        [](std::istream* read) { return read->tellg(); });
}

void SaveFile::seekFromStart(long offset)
{
    doOnFile([offset](std::ostream* w) { w->seekp(offset, std::ios::beg); },
             [offset](std::istream* r) { r->seekg(offset, std::ios::beg); });
}

void SaveFile::seekFromEnd(long offset)
{
    doOnFile([offset](std::ostream* w) { w->seekp(offset, std::ios::end); },
             [offset](std::istream* r) { r->seekg(offset, std::ios::end); });
}

template <typename R, typename... Visitor>
R SaveFile::doOnFile(Visitor&&... v) const
{
    return std::visit(checked_overload<R>(std::forward<Visitor>(v)...), m_file);
}
