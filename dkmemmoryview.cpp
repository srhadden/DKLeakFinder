#include "dkmemmoryview.h"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>

DKMemmoryView::DKMemmoryView(QWidget *parent) :
    QFrame(parent)
{
    p_memSize = 0;
    p_rectSize = QSize(10, 10);
    setStyleSheet("background: grey;");
    mem.clear();
    setMouseTracking(true);
}

void DKMemmoryView::setRectSize(QSize size)
{
    p_rectSize = size;
    calcSize();
}

void DKMemmoryView::setMemSize(uint32_t bytes)
{
    p_memSize = bytes;
    calcSize();
}

void DKMemmoryView::DoMalloc(uint32_t addr, uint32_t size)
{
    if (mem.contains(addr))
    {
        qDebug() << "Malloc Memmory leak!" << addr << size;
        return;
    }
    if (mem.contains(size))
    {
        qDebug() << "Zero-size memmory allocation!";
        return;
    }
    mem[addr] = size;
    update();
}

void DKMemmoryView::DoFree(uint32_t addr)
{
    if (!mem.contains(addr))
    {
        qDebug() << "Free Memmory leak!" << addr;
        return;
    }
    mem.remove(addr);
    update();
}

void DKMemmoryView::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    //
    QHashIterator<uint32_t, uint32_t> i(mem);
    while (i.hasNext())
    {
        i.next();
        //

        for (uint32_t p = i.key(); p < i.key()+i.value(); p++)
        {
            int row = p/p_viewSize.width();
            int column = p%p_viewSize.width();
            QBrush brush;
            if (selectedRegion == i.key())
                brush.setColor(QColor("red"));
            else
                brush.setColor(QColor("green"));
            if (highlightedRegion == i.key())
            {
                QColor col = brush.color();
                col.setBlue(240);
                brush.setColor(col);
            }
            brush.setStyle(Qt::SolidPattern);
            painter.setBrush(brush);
            painter.drawRect(
                        column*p_rectSize.height(),
                        row*p_rectSize.width(),
                        p_rectSize.width(),
                        p_rectSize.height()
                        );
        }

    }
    //
    painter.end();
    QFrame::paintEvent(ev);
}

void DKMemmoryView::calcSize()
{
    int mw = size().width();
    qDebug() << mw;
    p_viewSize.setWidth((mw/p_rectSize.width())-1);
    p_viewSize.setHeight(p_memSize/p_viewSize.width());
    if (p_memSize%p_viewSize.width())
        p_viewSize.setHeight(p_viewSize.height()+1);
    p_viewSize.setHeight(p_viewSize.height()+1);
    qDebug() << p_rectSize << p_memSize;
    qDebug() << p_viewSize;
    setMinimumHeight(p_viewSize.height()*p_rectSize.height());
    setMaximumHeight(p_viewSize.height()*p_rectSize.height());
}

void DKMemmoryView::resizeEvent(QResizeEvent *ev)
{
    calcSize();
}

void DKMemmoryView::mousePressEvent(QMouseEvent *ev)
{
    QHashIterator<uint32_t, uint32_t> i(mem);
    while (i.hasNext())
    {
        i.next();
        //

        for (uint32_t p = i.key(); p <= i.key()+i.value(); p++)
        {
            int row = p/p_viewSize.width();
            int column = p%p_viewSize.width();

            QRect rect(column*p_rectSize.height(),
                       row*p_rectSize.width(),
                       p_rectSize.width(),
                       p_rectSize.height()
                       );

            if (rect.contains(ev->pos().x(), ev->pos().y()))
            {
                if (selectedRegion != i.key())
                    emit selctionChanged(i.key());
                selectedRegion = i.key();
                repaint();
                return;
            }
        }
    }
}

void DKMemmoryView::mouseMoveEvent(QMouseEvent *ev)
{
    QHashIterator<uint32_t, uint32_t> i(mem);
    while (i.hasNext())
    {
        i.next();
        //

        for (uint32_t p = i.key(); p <= i.key()+i.value(); p++)
        {
            int row = p/p_viewSize.width();
            int column = p%p_viewSize.width();

            QRect rect(column*p_rectSize.height(),
                       row*p_rectSize.width(),
                       p_rectSize.width(),
                       p_rectSize.height()
                       );

            if (rect.contains(ev->pos().x(), ev->pos().y()))
            {
                highlightedRegion = i.key();
                repaint();
                return;
            }
        }
    }
}
