#ifndef GENERALPREVIEW_H
#define GENERALPREVIEW_H

#include <QWidget>

namespace Ui {
class GeneralPreview;
}

class GeneralPreview : public QWidget {
	Q_OBJECT

public:
    explicit GeneralPreview(QWidget* parent = nullptr);
	~GeneralPreview();

private slots:
    void onItemSelectionChanged();

private:
    virtual bool isAddEnabled()       = 0;
    virtual bool isEditEnabled()      = 0;
    virtual bool isMultiEditEnabled() = 0;
    virtual bool isRemoveEnabled()    = 0;
    Ui::GeneralPreview* ui;
};

#endif // GENERALPREVIEW_H
