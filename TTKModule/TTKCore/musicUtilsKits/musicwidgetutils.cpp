#include "musicwidgetutils.h"

#include <QComboBox>
#include <QBitmap>
#include <QPainter>
#include <QBuffer>
#include <QFileDialog>

void MusicUtils::Widget::setLabelFontSize(QWidget *widget, int size)
{
    QFont font = widget->font();
    font.setPointSize(size);
    widget->setFont(font);
}

void MusicUtils::Widget::setLabelFontStyle(QWidget *widget, MusicObject::FontStyleMode type)
{
    QFont font = widget->font();
    switch(type)
    {
        case MusicObject::FT_Bold : font.setBold(true); break;
        case MusicObject::FT_Italic : font.setItalic(true); break;
        case MusicObject::FT_Underline : font.setUnderline(true); break;
        case MusicObject::FT_Overline : font.setOverline(true); break;
        case MusicObject::FT_StrikeOut : font.setStrikeOut(true); break;
        case MusicObject::FT_FixedPitch : font.setFixedPitch(true); break;
        case MusicObject::FT_Kerningt : font.setKerning(true); break;
        default: break;
    }
    widget->setFont(font);
}

QString MusicUtils::Widget::elidedText(const QFont &font, const QString &text,
                                       Qt::TextElideMode mode, int width)
{
    QFontMetrics ft(font);
    return ft.elidedText(text, mode, width);
}

void MusicUtils::Widget::setTransparent(QWidget *widget, int alpha)
{
    QPalette pal = widget->palette();
    pal.setBrush(QPalette::Base, QBrush(QColor(0xFF, 0xFF, 0xFF, alpha)));
    widget->setPalette(pal);
}

void MusicUtils::Widget::setComboBoxText(QComboBox *object, const QString &text)
{
    if(object->isEditable())
    {
        object->setEditText(text);
    }
    else
    {
        const int i = object->findText(text);
        if(i > -1)
        {
            object->setCurrentIndex(i);
        }
    }
}

void MusicUtils::Widget::widgetToRound(QWidget *w, int ratioX, int ratioY)
{
    w->setMask( getBitmapMask(w->rect(), ratioX, ratioY) );
}

void MusicUtils::Widget::fusionPixmap(QPixmap &bg, const QPixmap &fg, const QPoint &pt)
{
    if(fg.isNull())
    {
        return;
    }

    QPainter painter(&bg);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawPixmap(pt.x(), pt.y(), fg);
    painter.end();
}

QPixmap MusicUtils::Widget::pixmapToRound(const QPixmap &src, const QSize &size, int ratioX, int ratioY)
{
    return pixmapToRound(src, QRect(QPoint(0, 0), size), ratioX, ratioY);
}

QPixmap MusicUtils::Widget::pixmapToRound(const QPixmap &src, const QRect &rect, int ratioX, int ratioY)
{
    if(src.isNull())
    {
        return QPixmap();
    }

    QPixmap image = src.scaled(rect.size());
    image.setMask( getBitmapMask(rect, ratioX, ratioY) );
    return image;
}

QPixmap MusicUtils::Widget::pixmapToRound(const QPixmap &src, const QPixmap &mask, const QSize &size)
{
    if(src.isNull() || mask.isNull())
    {
        return QPixmap();
    }

    QPixmap image(mask);
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, src.scaled(size));
    painter.end();

    return image;
}

QBitmap MusicUtils::Widget::getBitmapMask(const QRect &rect, int ratioX, int ratioY)
{
    QBitmap mask(rect.size());
    QPainter painter(&mask);
    painter.fillRect(rect, Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.drawRoundedRect(rect, ratioX, ratioY);
    return mask;
}

QByteArray MusicUtils::Widget::getPixmapData(const QPixmap &pix)
{
    if(pix.isNull())
    {
        return QByteArray();
    }

    QByteArray data;
    QBuffer buffer(&data);
    if(buffer.open(QIODevice::WriteOnly))
    {
        pix.save(&buffer, JPG_FILE_PREFIX);
    }
    buffer.close();
    return data;
}

int MusicUtils::Widget::reRenderAlpha(int alpha, int value)
{
    return reRenderValue<int>(0xFF, alpha, value);
}

void MusicUtils::Widget::reRenderImage(int delta, const QImage *input, QImage *output)
{
    for(int w=0; w<input->width(); w++)
    {
        for(int h=0; h<input->height(); h++)
        {
            QRgb rgb = input->pixel(w, h);
            output->setPixel(w, h, qRgb(colorBurnTransform(qRed(rgb), delta),
                                        colorBurnTransform(qGreen(rgb), delta),
                                        colorBurnTransform(qBlue(rgb), delta)));
        }
    }
}

uint MusicUtils::Widget::colorBurnTransform(int c, int delta)
{
    if(0 > delta || delta > 0xFF)
    {
        return c;
    }

    int result = (c - (int)(c*delta)/(0xFF - delta));
    if(result > 0xFF)
    {
        result = 0xFF;
    }
    else if(result < 0)
    {
        result = 0;
    }
    return result;
}

QString MusicUtils::Widget::getOpenFileDialog(QWidget *obj, const QString &title, const QString &filter)
{
    return QFileDialog::getOpenFileName(obj, title, QDir::currentPath(), filter);
}

QString MusicUtils::Widget::getOpenFileDialog(QWidget *obj, const QString &filter)
{
    return getOpenFileDialog(obj, QObject::tr("choose a filename to open under"), filter);
}

QString MusicUtils::Widget::getOpenFileDialog(QWidget *obj)
{
    return getOpenFileDialog(obj, "Images (*.png *.bmp *.jpg)");
}

QStringList MusicUtils::Widget::getOpenFilesDialog(QWidget *obj, const QString &title, const QString &filter)
{
    return QFileDialog::getOpenFileNames(obj, title, QDir::currentPath(), filter);
}

QStringList MusicUtils::Widget::getOpenFilesDialog(QWidget *obj, const QString &filter)
{
    return getOpenFilesDialog(obj, QObject::tr("choose a filename to open under"), filter);
}

QStringList MusicUtils::Widget::getOpenFilesDialog(QWidget *obj)
{
    return getOpenFilesDialog(obj, "Images (*.png *.bmp *.jpg)");
}

QString MusicUtils::Widget::getSaveFileDialog(QWidget *obj, const QString &title, const QString &filter)
{
    return QFileDialog::getSaveFileName(obj, title, QDir::currentPath(), filter);
}

QString MusicUtils::Widget::getSaveFileDialog(QWidget *obj, const QString &filter)
{
    return getSaveFileDialog(obj, QObject::tr("choose a filename to save under"), filter);
}

QString MusicUtils::Widget::getSaveFileDialog(QWidget *obj)
{
    return getSaveFileDialog(obj, "Images (*.png *.bmp *.jpg)");
}
