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
signals:
    void removedItem(size_t index);
public slots:
    void onItemSelectionChanged();
private slots:
    void onRemoveButtonClicked();

private:
    virtual bool isAddEnabled() const       = 0;
    virtual bool isEditEnabled() const      = 0;
    virtual bool isMultiEditEnabled() const = 0;
    virtual bool isRemoveEnabled() const    = 0;

protected:
    Ui::GeneralPreview* ui;
};

#endif // GENERALPREVIEW_H
