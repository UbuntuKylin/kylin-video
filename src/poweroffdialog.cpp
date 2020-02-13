
#include "poweroffdialog.h"
#include "smplayer/images.h"

#include <QTimer>
#include <QMouseEvent>
#include <QDebug>
#include <QMessageBox>

PoweroffDialog::PoweroffDialog(QWidget * parent, Qt::WindowFlags f)
	: QDialog(parent, f)
    , m_dragState(NOT_DRAGGING)
    , m_startDrag(QPoint(0,0))
    , m_countdown(30)
    , m_mainLayout(new QVBoxLayout(this))
{
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setMinimumSize(QSize(438, 100));
    this->setStyleSheet("QDialog{border: 1px solid #121212;border-radius:1px;background-color: #ffffff;}");
    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")));
//    this->setWindowIcon(QIcon::fromTheme("kylin-video", QIcon(":/res/kylin-video.png")).pixmap(QSize(64, 64)).scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setAutoFillBackground(true);
    this->setMouseTracking(true);
    this->installEventFilter(this);

    m_text = tr("The computer will shut down in %1 seconds.") +"<br>"+ tr("Press <b>Cancel</b> to abort shutdown.");

    m_topWidget = new QWidget(this);
    m_topWidget->setFixedHeight(39);
    m_topWidget->setMouseTracking(true);
    m_topWidget->setAutoFillBackground(true);
    m_topWidget->setStyleSheet("QWidget{border:none;background-color:#2e2e2e;}");
//    QPalette palette;
//    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/res/about_bg.png")));
//    m_topWidget->setPalette(palette);

    QHBoxLayout *top_layout = new QHBoxLayout(m_topWidget);
    top_layout->setSpacing(0);
    top_layout->setMargin(0);
    m_closeBtn = new QPushButton(m_topWidget);
    m_closeBtn->setFixedSize(36, 36);
    m_closeBtn->setFocusPolicy(Qt::NoFocus);
    m_closeBtn->setStyleSheet("QPushButton{background-image:url(':/res/close_normal.png');border:0px;}QPushButton:hover{background:url(':/res/close_hover.png');}QPushButton:pressed{background:url(':/res/close_press.png');}");
    top_layout->addStretch();
    top_layout->addWidget(m_closeBtn, 0, Qt::AlignRight | Qt::AlignTop);
    connect(m_closeBtn, &QPushButton::clicked, this, &PoweroffDialog::close);

    m_centerFrame = new QFrame(this);
    m_centerFrame->setFrameShape(QFrame::HLine);
    m_centerFrame->setFrameShadow(QFrame::Sunken);

    QHBoxLayout *horizontalLayout = new QHBoxLayout(m_centerFrame);

    icon_label = new QLabel(this);
    icon_label->setMinimumSize(QSize(100, 0));
    icon_label->setAlignment(Qt::AlignCenter);
    icon_label->setPixmap(Images::icon("kylin-video"));
    horizontalLayout->addWidget(icon_label);

    m_textLabel = new QLabel(this);
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_textLabel->sizePolicy().hasHeightForWidth());
    m_textLabel->setSizePolicy(sizePolicy);
    m_textLabel->setWordWrap(true);
    m_textLabel->setText(m_text.arg(m_countdown));
    horizontalLayout->addWidget(m_textLabel);

    //QMessageBox::StandardButtons buttons = QMessageBox::Ok | QMessageBox::Cancel;
    //QMessageBox::StandardButton defaultButton = QMessageBox::Cancel;
    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setOrientation(Qt::Horizontal);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);//buttonBox->setStandardButtons(QDialogButtonBox::StandardButtons(int(buttons)));
    m_buttonBox->setContentsMargins(0, 0, 20, 0);

    m_mainLayout->setSpacing(0);
    m_mainLayout->setMargin(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 20);
    m_mainLayout->addWidget(m_topWidget, 0, Qt::AlignTop);
    m_mainLayout->addWidget(m_centerFrame);
    m_mainLayout->addWidget(m_buttonBox, 0, Qt::AlignBottom);

    QPushButton *okBtn = m_buttonBox->button(QDialogButtonBox::Ok);
    if (okBtn != NULL) {
        okBtn->setFixedSize(91, 25);
        okBtn->setText(tr("Ok"));
        okBtn->setFocusPolicy(Qt::NoFocus);
        okBtn->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #0a9ff5;color:#999999;}QPushButton:hover{background-color:#0a9ff5;border:1px solid #2db0f6;color:#ffffff;} QPushButton:pressed{background-color:#0993e3;border:1px solid #0a9ff5;color:#ffffff;}");
    }

    QPushButton *cancelBtn = m_buttonBox->button(QDialogButtonBox::Cancel);
    if (cancelBtn != NULL) {
        cancelBtn->setFixedSize(91, 25);
        cancelBtn->setText(tr("Cancel"));
        cancelBtn->setFocusPolicy(Qt::NoFocus);
        cancelBtn->setStyleSheet("QPushButton{font-size:12px;background:#0f0f0f;border:1px solid #000000;color:#999999;}QPushButton:hover{background-color:#1f1f1f;border:1px solid #0f0f0f;color:#ffffff;} QPushButton:pressed{background-color:#0d0d0d;border:1px solid #000000;color:#ffffff;}");
    }

    if (okBtn) { okBtn->setDefault(false); okBtn->setAutoDefault(false); }
    if (cancelBtn) { cancelBtn->setDefault(true); cancelBtn->setAutoDefault(true); }

    QObject::connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    this->adjustSize();

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateCountdown()));
    m_timer->start();
}

PoweroffDialog::~PoweroffDialog()
{

}

void PoweroffDialog::updateCountdown()
{
    m_countdown--;
    m_textLabel->setText(m_text.arg(m_countdown));
    if (m_countdown < 1) {
        this->accept();
    }
}

QSize PoweroffDialog::sizeHint () const
{
    return this->size();
}

void PoweroffDialog::moveDialog(QPoint diff) {
#if QT_VERSION >= 0x050000
    // Move the window with some delay.
    // Seems to work better with Qt 5

    static QPoint d;
    static int count = 0;

    d += diff;
    count++;

    if (count > 3) {
        QPoint new_pos = pos() + d;
        if (new_pos.y() < 0) new_pos.setY(0);
        if (new_pos.x() < 0) new_pos.setX(0);
        move(new_pos);
        count = 0;
        d = QPoint(0,0);
    }
#else
    move(pos() + diff);
#endif
}

bool PoweroffDialog::eventFilter( QObject * object, QEvent * event )
{
    QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress
        && type != QEvent::MouseButtonRelease
        && type != QEvent::MouseMove)
        return false;

    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent*>(event);
    if (!mouseEvent)
        return false;

    if (mouseEvent->modifiers() != Qt::NoModifier) {
        m_dragState = NOT_DRAGGING;
        return false;
    }

    if (type == QEvent::MouseButtonPress) {
        if (mouseEvent->button() != Qt::LeftButton) {
            m_dragState = NOT_DRAGGING;
            return false;
        }

        m_dragState = START_DRAGGING;
        m_startDrag = mouseEvent->globalPos();
        // Don't filter, so others can have a look at it too
        return false;
    }

    if (type == QEvent::MouseButtonRelease) {
        if (m_dragState != DRAGGING || mouseEvent->button() != Qt::LeftButton) {
            m_dragState = NOT_DRAGGING;
            return false;
        }

        // Stop dragging and eat event
        m_dragState = NOT_DRAGGING;
        event->accept();
        return true;
    }

    // type == QEvent::MouseMove
    if (m_dragState == NOT_DRAGGING)
        return false;

    // buttons() note the s
    if (mouseEvent->buttons() != Qt::LeftButton) {
        m_dragState = NOT_DRAGGING;
        return false;
    }

    QPoint pos = mouseEvent->globalPos();
    QPoint diff = pos - m_startDrag;
    if (m_dragState == START_DRAGGING) {
        // Don't start dragging before moving at least DRAG_THRESHOLD pixels
        if (abs(diff.x()) < 4 && abs(diff.y()) < 4)
            return false;

        m_dragState = DRAGGING;
    }
    this->moveDialog(diff);

    m_startDrag = pos;
    event->accept();
    return true;
}
