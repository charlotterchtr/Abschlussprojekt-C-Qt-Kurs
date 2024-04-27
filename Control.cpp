#include "Control.h"

Controls::Controls(QWidget *parent) : QWidget(parent)
{

    //implement and connect buttons

    openButton = new QPushButton(tr("Add"), this);
    connect(openButton, &QPushButton::clicked, this, &Controls::openClicked);

    Play = new QToolButton(this);
    Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(Play, &QAbstractButton::clicked, this, &Controls::playClicked);

    Stop = new QToolButton(this);
    Stop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    Stop->setEnabled(false);
    connect(Stop, &QAbstractButton::clicked, this, &Controls::stop);

    Next = new QToolButton(this);
    Next->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    connect(Next, &QAbstractButton::clicked, this, &Controls::next);

    Previous = new QToolButton(this);
    Previous->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    connect(Previous, &QAbstractButton::clicked, this, &Controls::previous);

    Mute = new QToolButton(this);
    Mute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    connect(Mute, &QAbstractButton::clicked, this, &Controls::muteClicked);

    Shuffle = new QPushButton(this);
    Shuffle->setText("Shuffle");
    Shuffle->setCheckable(true);
    connect(Shuffle, &QPushButton::clicked, this, &Controls::ShuffleClicked);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    QSizePolicy sp = volumeSlider->sizePolicy();
    sp.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
    volumeSlider->setSizePolicy(sp);
    connect(volumeSlider, &QSlider::valueChanged, this, &Controls::onVolumeSliderValueChanged);

    rateBox = new QComboBox(this);
    rateBox->addItem("0.5x", QVariant(0.5));
    rateBox->addItem("1.0x", QVariant(1.0));
    rateBox->addItem("2.0x", QVariant(2.0));
    rateBox->setCurrentIndex(1);
    connect(rateBox, QOverload<int>::of(&QComboBox::activated), this, &Controls::updateRate);

    // implement layout

    QBoxLayout *layout = new QHBoxLayout;

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(openButton);
    layout->addWidget(Stop);
    layout->addWidget(Previous);
    layout->addWidget(Play);
    layout->addWidget(Next);
    layout->addWidget(Shuffle);
    layout->addWidget(Mute);
    layout->addWidget(volumeSlider);
    layout->addWidget(rateBox);

    setLayout(layout);
}

QMediaPlayer::PlaybackState Controls::state() const
{
    return playerState;
}

//set Icons depending on PlaybackState
void Controls::setState(QMediaPlayer::PlaybackState state)
{
    if (state != playerState) {
        playerState = state;

        switch (state) {
        case QMediaPlayer::StoppedState:
            Stop->setEnabled(false);
            Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        case QMediaPlayer::PlayingState:
            Stop->setEnabled(true);
            Play->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        case QMediaPlayer::PausedState:
            Stop->setEnabled(true);
            Play->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
        }
    }
}

void Controls::openClicked(){
    emit opened();
}

float Controls::volume() const
{
    qreal linearVolume =
        QAudio::convertVolume(volumeSlider->value() / qreal(100),
                              QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);

    return linearVolume;
}

void Controls::setVolume(float volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume, QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);

    volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

bool Controls::isMuted() const
{
    return playerMuted;
}

void Controls::setMuted(bool muted)
{
    if (muted != playerMuted) {
        playerMuted = muted;

        Mute->setIcon(style()->standardIcon(muted ? QStyle::SP_MediaVolumeMuted
                                                          : QStyle::SP_MediaVolume));
    }
}

void Controls::ShuffleClicked(bool shuffled){
        IsShuffled = shuffled;
        emit shuffle(IsShuffled);
}

void Controls::playClicked()
{
    switch (playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    }
}

void Controls::muteClicked()
{
    emit changeMuting(!playerMuted);
}

qreal Controls::playbackRate() const
{
    return rateBox->itemData(rateBox->currentIndex()).toDouble();
}

void Controls::setPlaybackRate(float rate)
{
    for (int i = 0; i < rateBox->count(); ++i) {
        if (qFuzzyCompare(rate, float(rateBox->itemData(i).toDouble()))) {
            rateBox->setCurrentIndex(i);
            return;
        }
    }

    rateBox->addItem(QStringLiteral("%1x").arg(rate), QVariant(rate));
    rateBox->setCurrentIndex(rateBox->count() - 1);
}

void Controls::updateRate()
{
    emit changeRate(playbackRate());
}

void Controls::onVolumeSliderValueChanged()
{
    emit changeVolume(volume());
}

