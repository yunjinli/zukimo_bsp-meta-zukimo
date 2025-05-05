/**
 * @copyright 2018 Infineon Technologies
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "Frame.hpp"
#include <common/Buffer.hpp>


Frame::Frame(IFramePool *owner, uint32_t bufferSize) :
    m_buffer {stdext::new_aligned<AlignmentType>(bufferSize)},
    m_owner {owner},
    m_offset {0},
    m_dataSize {0},
    m_bufferSize {bufferSize}
{
}

Frame::~Frame()
{
    delete[] m_buffer;
}

void Frame::resizeBuffer(uint32_t bufferSize)
{
    delete[] m_buffer;
    m_bufferSize = bufferSize;
    m_buffer     = stdext::new_aligned<AlignmentType>(bufferSize);
}

void Frame::unpool()
{
    m_owner = nullptr;
}

uint8_t *Frame::getData() const
{
    return reinterpret_cast<uint8_t *>(m_buffer) + m_offset;
}

uint32_t Frame::getDataSize() const
{
    return m_dataSize;
}

void Frame::setDataOffset(uint32_t offset)
{
    m_offset = offset;
}

void Frame::setDataSize(uint32_t dataSize)
{
    m_dataSize = dataSize;
}

uint32_t Frame::getDataOffset() const
{
    return m_offset;
}

uint8_t *Frame::getBuffer() const
{
    return reinterpret_cast<uint8_t *>(m_buffer);
}

uint32_t Frame::getBufferSize() const
{
    return m_bufferSize;
}

uint32_t Frame::getStatusCode() const
{
    return 0;
}

void Frame::queue()
{
    if (m_owner != nullptr)
    {
        m_owner->queueFrame(this);
    }
}
