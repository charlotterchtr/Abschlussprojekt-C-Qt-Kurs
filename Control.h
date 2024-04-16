#ifndef CONTROL_H
#define CONTROL_H

#include <QMediaPlayer>
#include <QWidget>


#include <QAudio>
#include <QBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QPushButton>

QT_BEGIN_NAMESPACE
class QAbstractButton;
class QAbstractSlider;
class QComboBox;
QT_END_NAMESPACE

class Controls : public QWidget
{
    Q_OBJECT

private:

    QAbstractButton * Play = nullptr;
    QAbstractButton * Stop = nullptr;
    QAbstractButton * Next = nullptr;
    QAbstractButton * Previous = nullptr;
    QAbstractButton * Mute = nullptr;
    QAbstractSlider * volumeSlider = nullptr;

    QMediaPlayer::PlaybackState playerState = QMediaPlayer::StoppedState;
    bool playerMuted = false;

    QComboBox * rateBox = nullptr;

public:
    Controls(QWidget *parent = nullptr);

    QMediaPlayer::PlaybackState state() const;

    float volume() const;
    bool isMuted() const;
    qreal playbackRate() const;

public slots:
    void setState(QMediaPlayer::PlaybackState state);
    void setVolume(float volume);
    void setMuted(bool muted);
    void setPlaybackRate(float rate);

signals:
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void changeVolume(float volume);
    void changeMuting(bool muting);
    void changeRate(qreal rate);

private slots:
    void playClicked();
    void muteClicked();
    void updateRate();
    void onVolumeSliderValueChanged();

};

#endif // CONTROL_H
