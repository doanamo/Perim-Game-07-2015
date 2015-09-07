#include "Precompiled.hpp"
#include "Message.hpp"
#include "Sink.hpp"
using namespace Logger;

Message::Message() :
    std::ostream(&m_text),
    m_source(),
    m_line(0)
{
}

Message::Message(Message&& other) :
    std::ostream(&m_text)
{
    m_text = std::move(other.m_text);
    m_source = std::move(other.m_source);

    m_line = other.m_line;
    other.m_line = 0;
}

Message::~Message()
{
}

Message& Message::SetText(const char* text)
{
    if(text != nullptr)
    {
        m_text.str(text);
    }
    else
    {
        m_text.str("");
    }

    return *this;
}

Message& Message::SetSource(const char* source)
{
    if(source != nullptr)
    {
        // Set source string.
        m_source = source;

        // Normalize source path separators.
        std::replace(m_source.begin(), m_source.end(), '\\', '/');

        // Remove base path to source directory.
        std::size_t position = m_source.find(Build::GetSourceDir());

        if(position != std::string::npos)
        {
            m_source.erase(position, Build::GetSourceDir().size());
        }
    }

    return *this;
}

Message& Message::SetLine(int line)
{
    if(line > 0)
    {
        m_line = line;
    }

    return *this;
}

std::string Message::GetText() const
{
    return m_text.str();
}

std::string Message::GetSource() const
{
    return m_source;
}

int Message::GetLine() const
{
    return m_line;
}

bool Message::IsEmpty() const
{
    return m_text.str().empty();
}

ScopedMessage::ScopedMessage(Logger::SinkBase* sink) :
    m_sink(sink)
{
}

ScopedMessage::ScopedMessage(ScopedMessage&& other)
{
    m_sink = other.m_sink;
    other.m_sink = nullptr;
}

ScopedMessage::~ScopedMessage()
{
    if(m_sink != nullptr)
    {
        m_sink->Write(*this);
    }
}