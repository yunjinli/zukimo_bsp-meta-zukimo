/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>


#define LOG_NONE  0x00
#define LOG_INFO  0x01
#define LOG_DEBUG 0x02
#define LOG_WARN  0x04
#define LOG_ERROR 0x08

#ifdef NDEBUG
    #define LOG_DEBUG_OPTION 0
#else
    #define LOG_DEBUG_OPTION LOG_DEBUG
#endif

const uint16_t LoggerLevelsDefault = (LOG_INFO | LOG_DEBUG_OPTION | LOG_WARN | LOG_ERROR);
const char LoggerDateTimeFormat[]  = "[%Y-%m-%d %H:%M:%S] ";
const char LoggerFileName[]        = "";


#define LOG(X)        LoggerInstance.log(LOG_##X)
#define LOG_LEVELS(X) LoggerInstance.setLevels(X)
#define LOG_BUFFER(X, buf, count)                                                                    \
    {                                                                                                \
        auto L = LoggerInstance.log(LOG_##X);                                                        \
        L << "buffer \"" << #buf << "\"";                                                            \
        for (uint_fast16_t i = 0; i < (count); i++)                                                  \
        {                                                                                            \
            L << " " << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>((buf)[i]); \
            if (!((i + 1) % 16))                                                                     \
            {                                                                                        \
                L << std::endl;                                                                      \
            }                                                                                        \
        }                                                                                            \
    }


/**
 * @brief Store the current time point as a global reference
 */
void tic();

/**
 * @brief Print to the log the time difference between making this call and the last call to tic()
 */
void toc();


class Logger
{
    class Line
    {
    public:
        Line(Logger *logger);
        Line(Line &&ref);
        ~Line();

        Line()       = delete;
        Line(Line &) = delete;

        Line &operator=(const Line &) = delete;
        Line &operator=(Line &&) = delete;

        template <typename T>
        Line &operator<<(const T &t);
        template <typename T>
        Line &write(const T *t, std::size_t count);

        // this is the signature for the std::endl function
        using EndLine = std::basic_ostream<char, std::char_traits<char>> &(*)(std::basic_ostream<char, std::char_traits<char>> &);

        // operator<< to take in std::endl
        Line &operator<<(EndLine);

    private:
        Logger *m_logger;
    };

public:
    Logger(uint16_t levels, const char *filename = nullptr);
    ~Logger();

    Logger()               = delete;
    Logger(Logger const &) = delete;

    Logger &operator=(const Logger &) = delete;
    Logger &operator=(Logger &&) = delete;

    void setLevels(uint16_t levels);

    Line log(uint16_t level);

protected:
    std::mutex m_lineLock;

private:
    uint16_t m_logLevels;
    std::ofstream m_outFile;
};


// template function defintions have to be known externally, so they have to be placed in header file instead of source file

template <typename T>
Logger::Line &Logger::Line::operator<<(const T &t)
{
    if (m_logger)
    {
        try
        {
            std::cout << t;
            if (m_logger->m_outFile.is_open())
            {
                m_logger->m_outFile << t;
            }
        }
        catch (...)
        {
        }
    }
    return *this;
}

template <typename T>
Logger::Line &Logger::Line::write(const T *t, std::size_t count)
{
    if (m_logger)
    {
        std::cout.write(t, count);
        if (m_logger->m_outFile.is_open())
        {
            m_logger->m_outFile.write(t, count);
        }
    }
    return *this;
}


extern Logger LoggerInstance;
