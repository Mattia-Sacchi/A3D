#include "incompatibilitydialog.h"
#include "ui_incompatibilitydialog.h"
#include "chartaxisgeneralsettings.h"
#include "customwidgets.h"

IncompatibilityDialog::IncompatibilityDialog(QWidget* parent, std::vector<IndicatorInfo>& infos)
    : QDialog(parent),
      ui(new Ui::IncompatibilityDialog),
      m_choice(UC_UNDEFINED),
      // The first item must be checked before
      m_info(infos[0]) {
	ui->setupUi(this);

    QFont font;
    font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(FR_High));

    ui->label->setFont(font);
    ui->discardButton->setFont(font);
    ui->continueButton->setFont(font);

    connect(ui->discardButton, &QPushButton::clicked, this, &IncompatibilityDialog::discardButtonClicked);
    connect(ui->continueButton, &QPushButton::clicked, this, &IncompatibilityDialog::continueButtonClicked);

    for(size_t i = 0; i < infos.size(); i++) {
        IndicatorInfo& info = infos[i];
        size_t buttonWidth  = info.m_type == A3D::CHAXIND_MINOR_INDICATOR ? MinorWidth : MajorWidth;
        QString buttonName  = info.m_style.m_labelFont.family();
        CustomButton* btn   = new CustomButton(this, info.m_style.m_indicatorColor, buttonWidth);

        QFont font          = info.m_style.m_labelFont;
        FontResolutions res = ChartAxisGeneralSettings::getFontResoulution(font.pointSize());
        font.setPointSize(ChartAxisGeneralSettings::getDisplaySize(res));
        btn->setFont(font);
        btn->setText(buttonName, info.m_style.m_labelColor);

        connect(btn, &QPushButton::clicked, this, [this, info]() {
            m_choice = UC_CHOOSE;
            m_info   = info;
            accept();
        });
        ui->scrollAreaWidgetContents->layout()->addWidget(btn);
    }
}

void IncompatibilityDialog::continueButtonClicked() {
    m_choice = UC_CONTINUE;
    accept();
}

void IncompatibilityDialog::discardButtonClicked() {
    m_choice = UC_DISCARD;
    reject();
}

IncompatibilityDialog::~IncompatibilityDialog() {
	delete ui;
}
