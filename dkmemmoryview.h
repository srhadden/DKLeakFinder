#ifndef DKMEMMORYVIEW_H
#define DKMEMMORYVIEW_H

#include <QFrame>
#include <QHash>
#include <inttypes.h>

class DKMemmoryView : public QFrame
{
    Q_OBJECT
public:
    explicit DKMemmoryView(QWidget *parent = 0);
    void setRectSize(QSize size);
    void setMemSize(uint32_t bytes);

    void DoMalloc(uint32_t addr, uint32_t size);
    void DoFree(uint32_t addr);

    uint32_t selectedRegion;
    uint32_t highlightedRegion;
    
signals:
    void selctionChanged(uint32_t addr);
    
public slots:

protected:
    void paintEvent(QPaintEvent *ev);
    void calcSize();
    void resizeEvent(QResizeEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    
private:
    QSize p_viewSize;
    QSize p_rectSize;
    uint32_t p_memSize;
    QHash<uint32_t, uint32_t> mem;
};

#endif // DKMEMMORYVIEW_H
