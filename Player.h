#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <iostream>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QPushButton>
#include <QGridLayout>
#include <QFileDialog>
#include <QLabel>
#include <QSlider>
#include <QListWidget>

#include "Playlist.h"
#include "Control.h"

class MusicPlayer : public QWidget
{
    Q_OBJECT

    QVBoxLayout * layout;
    QBoxLayout * controlLayout;
    QVBoxLayout * trackLayout;
    QHBoxLayout * durationLayout;

    QMediaPlayer * Player = nullptr;
    QAudioOutput * audioOutput = nullptr;
    QMediaPlayer::PlaybackState * status = nullptr;

    QList<QPair<QUrl, int>> * playlist = nullptr;
    int * CurrentIndex = nullptr;
    QPushButton * ViewPlaylist;

    PlaylistDialog * playlistDialog = nullptr;
    void handleViewPlaylist();
    void handlePlaylistUpdated();

    QSlider * slider;
    QLabel * duration;
    QLabel * titleLabel;
    QLabel * authorLabel;

    Controls * PlayerControl = nullptr;

    bool isPlayerAvailable() const;
    bool * Shuffle;

    void open();
    void deleteSong();
    void startPlayer();
    void playClicked();
    void previousClicked();
    void nextClicked();
    void handleCurrentIndexChanged(QMediaPlayer::PlaybackState * state);
    void updatePlaylist();
    void cleanupPlaylistDialog();
    void handleShuffle(bool IsShuffled);
    void handleItemDeleted(int index);
    void setStatus(QString const & status);

    qint64 c_duration;
    void handleStateChanged();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void seek(int mseconds);
    void updateDurationInfo(qint64 currentInfo);

    void metaDataChanged();
    void handleCursor(QMediaPlayer::MediaStatus status);
    void displayErrorMessage(QMediaPlayer::Error error, const QString & errorString);

signals:
    void CurrentIndexChanged(QMediaPlayer::PlaybackState * state);
    void FirstSongAdded();
    void SongsAdded();
    void mediaLoaded();
    void PlaylistChanged();
    void deleteItemsSelected(QList<QListWidgetItem *> selectedItems);
    void ClickNext();

public:
    MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();
};
#endif // PLAYER_H
