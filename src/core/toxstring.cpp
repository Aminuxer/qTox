/*
    Copyright © 2017-2019 by The qTox Project Contributors

    This file is part of qTox, a Qt-based graphical interface for Tox.

    qTox is libre software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    qTox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with qTox.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "toxstring.h"

#include <QByteArray>
#include <QString>

#include <cassert>
#include <climits>

/**
 * @class ToxString
 * @brief Helper to convert safely between strings in the c-toxcore representation and QString.
 */

/**
 * @brief Creates a ToxString from a QString.
 * @param string Input text.
 */
ToxString::ToxString(const QString& text)
    : ToxString(text.toUtf8())
{
}

/**
 * @brief Creates a ToxString from bytes in a QByteArray.
 * @param text Input text.
 */
ToxString::ToxString(const QByteArray& text)
    : string(text)
{
}

/**
 * @brief Creates a ToxString from the representation used by c-toxcore.
 * @param text Pointer to the beginning of the text.
 * @param length Number of bytes to read from the beginning.
 */
ToxString::ToxString(const uint8_t* text, size_t length)
{
    if (length > SIZE_MAX){
    	string = QByteArray("The string has invalid length!", 31);
    }
    // Some clients does not send the correct string end. Starting from  Qt6 we need to
    // make sure that we trim the string to its actual size.
    size_t actualLength = 0;
    for (actualLength=0; actualLength<length; actualLength++){
        if(*(text + actualLength) == 0)
            break;
    }
    string = QByteArray(reinterpret_cast<const char*>(text), actualLength);
}

/**
 * @brief Returns a pointer to the beginning of the string data.
 * @return Pointer to the beginning of the string data.
 */
const uint8_t* ToxString::data() const
{
    return reinterpret_cast<const uint8_t*>(string.constData());
}

/**
 * @brief Get the number of bytes in the string.
 * @return Number of bytes in the string.
 */
size_t ToxString::size() const
{
    return string.size();
}

/**
 * @brief Gets the string as QString.
 * @return QString representation of the string.
 */
QString ToxString::getQString() const
{
    auto decoder = QStringDecoder(QStringDecoder::Utf8);
    return decoder(string);
}

/**
 * @brief getBytes Gets the bytes of the string.
 * @return Bytes of the string as QByteArray.
 */
QByteArray ToxString::getBytes() const
{
    return QByteArray(string);
}
