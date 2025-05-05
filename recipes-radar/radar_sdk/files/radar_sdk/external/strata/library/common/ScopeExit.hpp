/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#pragma once

#include <utility>


namespace stdext
{
    template <typename FunctionType>
    class ScopeExitImpl;

    template <typename FunctionType>
    ScopeExitImpl<typename std::decay<FunctionType>::type> ScopeExit(FunctionType &&function)
    {
        return ScopeExitImpl<typename std::decay<FunctionType>::type>(std::forward<FunctionType>(function));
    }

    template <typename FunctionType>
    class ScopeExitImpl
    {
        friend ScopeExitImpl<typename std::decay<FunctionType>::type> ScopeExit<FunctionType>(FunctionType &&function);

    public:
        ~ScopeExitImpl()
        {
            m_function();
        }

        ScopeExitImpl(ScopeExitImpl &&other) :
            m_function {std::move(other.m_function)},
            m_active {other.m_active}
        {
            other.m_active = false;
        }

    private:
        ScopeExitImpl()                      = delete;
        ScopeExitImpl(const ScopeExitImpl &) = delete;
        ScopeExitImpl &operator=(const ScopeExitImpl &) = delete;
        ScopeExitImpl &operator=(ScopeExitImpl &&) = delete;

        ScopeExitImpl(const FunctionType &function) :
            m_function {function},
            m_active {true}
        {}

        ScopeExitImpl(FunctionType &&function) :
            m_function {std::move(function)},
            m_active {true}
        {}

        FunctionType m_function;
        bool m_active;
    };
}
