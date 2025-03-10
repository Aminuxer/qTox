/*
    Copyright © 2014-2019 by The qTox Project Contributors

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

#include "croppinglabel.h"
#include <QKeyEvent>
#include <QLineEdit>
#include <QResizeEvent>
#include <QTextDocument>
#include <QRegularExpression>

CroppingLabel::CroppingLabel(QWidget* parent)
    : QLabel(parent)
    , blockPaintEvents(false)
    , editable(false)
    , elideMode(Qt::ElideRight)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    class LineEdit : public QLineEdit
    {
    public:
        explicit LineEdit(QWidget* parent = nullptr)
            : QLineEdit(parent)
        {}

    protected:
        void keyPressEvent(QKeyEvent* event) override
        {
            if (event->key() == Qt::Key_Escape) {
                undo();
                clearFocus();
            }

            QLineEdit::keyPressEvent(event);
        }
    };

    textEdit = new LineEdit(this);
    textEdit->hide();
    textEdit->setInputMethodHints(Qt::ImhNoAutoUppercase | Qt::ImhNoPredictiveText | Qt::ImhPreferLatin);

    connect(textEdit, &QLineEdit::editingFinished, this, &CroppingLabel::editingFinished);
}

void CroppingLabel::editBegin()
{
    showTextEdit();
    textEdit->selectAll();
}

void CroppingLabel::setEditable(bool editable_)
{
    editable = editable_;

    if (editable)
        setCursor(Qt::PointingHandCursor);
    else
        unsetCursor();
}

void CroppingLabel::setElideMode(Qt::TextElideMode elide)
{
    elideMode = elide;
}

void CroppingLabel::setText(const QString& text)
{
    origText = text.trimmed();
    setElidedText();
}

void CroppingLabel::setPlaceholderText(const QString& text)
{
    textEdit->setPlaceholderText(text);
    setElidedText();
}

void CroppingLabel::resizeEvent(QResizeEvent* ev)
{
    setElidedText();
    textEdit->resize(ev->size());

    QLabel::resizeEvent(ev);
}

QSize CroppingLabel::sizeHint() const
{
    return QSize(0, QLabel::sizeHint().height());
}

QSize CroppingLabel::minimumSizeHint() const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    return QSize(fontMetrics().horizontalAdvance("..."), QLabel::minimumSizeHint().height());
#else
    return QSize(fontMetrics().width("..."), QLabel::minimumSizeHint().height());
#endif
}

void CroppingLabel::mouseReleaseEvent(QMouseEvent* e)
{
    if (editable)
        showTextEdit();

    emit clicked();

    QLabel::mouseReleaseEvent(e);
}

void CroppingLabel::paintEvent(QPaintEvent* paintEvent)
{
    if (blockPaintEvents) {
        paintEvent->ignore();
        return;
    }
    QLabel::paintEvent(paintEvent);
}

void CroppingLabel::setElidedText()
{
    QString elidedText = fontMetrics().elidedText(origText, elideMode, width());
    if (elidedText != origText)
        setToolTip(Qt::convertFromPlainText(origText, Qt::WhiteSpaceNormal));
    else
        setToolTip(QString());
    if (!elidedText.isEmpty()) {
        QLabel::setText(elidedText);
    } else {
        // NOTE: it would be nice if the label had custom styling when it was default
        QLabel::setText(textEdit->placeholderText());
    }
}

void CroppingLabel::hideTextEdit()
{
    textEdit->hide();
    blockPaintEvents = false;
}

void CroppingLabel::showTextEdit()
{
    blockPaintEvents = true;
    textEdit->show();
    textEdit->setFocus();
    textEdit->setText(origText);
    textEdit->setFocusPolicy(Qt::ClickFocus);
}

/**
 * @brief Get original full text.
 * @return The un-cropped text.
 */
QString CroppingLabel::fullText()
{
    return origText;
}

void CroppingLabel::minimizeMaximumWidth()
{
    // This function chooses the smallest possible maximum width.
    // Text width + padding. Without padding, we'll have elipses.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
    setMaximumWidth(fontMetrics().horizontalAdvance(origText) + fontMetrics().horizontalAdvance("..."));
#else
    setMaximumWidth(fontMetrics().width(origText) + fontMetrics().width("..."));
#endif
}

void CroppingLabel::editingFinished()
{
    hideTextEdit();
    QString newText = textEdit->text().trimmed().remove(QRegularExpression("[\\t\\n\\v\\f\\r\\x0000]"));

    if (origText != newText)
        emit editFinished(textEdit->text());

    emit editRemoved();
}
