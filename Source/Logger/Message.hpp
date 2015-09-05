#pragma once

#include "Precompiled.hpp"

//
// Message
//

namespace Logger
{
    // Message class.
    class Message : private NonCopyable, public std::ostream
    {
    public:
        Message();
        Message(Message&& other);
        virtual ~Message();

        // Sets message text.
        Message& SetText(const char* text);

        // Sets message source.
        Message& SetSource(const char* source);

        // Sets message line.
        Message& SetLine(int line);

        // Gets message text.
        std::string GetText() const;

        // Gets message source.
        std::string GetSource() const;

        // Gets message line.
        int GetLine() const;

    private:
        // Message state.
        std::stringbuf m_text;
        std::string    m_source;
        int            m_line;
    };
}

//
// Scoped Message
//

namespace Logger
{
    // Forward declarations.
    class Sink;

    // Scoped message class.
    class ScopedMessage : public Message
    {
    public:
        ScopedMessage(Logger::Sink* sink);
        ScopedMessage(ScopedMessage&& other);
        ~ScopedMessage();

    private:
        // Message sink output.
        Logger::Sink* m_sink;
    };
}
