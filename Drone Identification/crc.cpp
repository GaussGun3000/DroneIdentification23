#include "crc.h"
#include "qdebug.h"

quint32 crc::crcTable[256] = {};

crc::crc()
{
    generateCrcTable();
}

void crc::generateCrcTable()
{
    const quint32 IEEE80211_POLY = 0xedb88320;
    for (int i = 0; i < 256; i++)
    {
        uint32_t c = i;
        for (int j = 0; j < 8; j++)
        {
            c = (c & 1) ? (IEEE80211_POLY ^ (c >> 1)) : (c >> 1);
        }
        this->crcTable[i] = c;
    }
}

const bool crc::checksumCRC(std::unique_ptr<QByteArray>& frame)
{
    if (frame->isEmpty())
    {
        qWarning() << "Could not extract frame hex from frame data!";
        return false;
    }
    uint32_t fcs = *(reinterpret_cast<const uint32_t*>(frame->right(fcsLen).constData()));
    QByteArray frameContent = frame->left(frame->size() - fcsLen);

    uint8_t* dataPtr = reinterpret_cast<uint8_t*>(frameContent.data());

    quint32 checksum = calculateCRC32(frameContent);
    return checksum == fcs;
}


quint32 crc::calculateCRC32(const QByteArray& frameContent)
{
    quint32 crc = 0xFFFFFFFF;

    for (const char byte : frameContent)
        crc = (crc >> 8) ^ crcTable[(crc ^ byte) & 0xFF];

    return crc ^ 0xFFFFFFFF;
}
