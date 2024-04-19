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
#include <QStatusBar>
#include <QLabel>
#include <QSlider>
#include <QListWidget>
#include <QDropEvent>
#include <QMimeData>
#include <QMessageBox>

#include "Playlist.h"
#include "Control.h"

class MusicPlayer : public QWidget
{
    Q_OBJECT

    QVBoxLayout * layout;
    QBoxLayout * controlLayout;
    QVBoxLayout * trackLayout;
    QHBoxLayout * durationLayout;

    QMediaPlayer * Player;
    QAudioOutput * audioOutput;

    QList<QPair<QUrl, int>> * playlist = nullptr;
    int * CurrentIndex;
    QPushButton * ViewPlaylist;
    QPushButton * openButton;
    QPushButton * deleteButton;
    QListWidget * playlistWidget;

    PlaylistDialog * playlistDialog;
    void handleViewPlaylist();
    void handlePlaylistUpdated();
    //void handleItemMoved(QDropEvent *event);
    //void dropEvent(QDropEvent *event) override;

    QSlider * slider;
    QLabel * duration;
    QLabel * titleLabel;
    QLabel * authorLabel;

    Controls * PlayerControl;

    bool isPlayerAvailable() const;

    void open();
    void deleteSong();
    void startPlayer();
    void playClicked();
    void previousClicked();
    void nextClicked();
    void handleCurrentIndexChanged();
    void updatePlaylist();
    void cleanupPlaylistDialog();

    qint64 c_duration;
    void handleStateChanged();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void seek(int mseconds);
    void updateDurationInfo(qint64 currentInfo);

    void metaDataChanged();
    void tracksChanged();

    //void statusChanged(QMediaPlayer::MediaStatus status);
    void handleCursor(QMediaPlayer::MediaStatus status);
    void setStatusInfo(const QString &info);
    void bufferingProgress(float progress);

    void selectAudioStream();
    void selectSubtitleStream();
    void displayErrorMessage();
    void audioOutputChanged(int);

    void handleDelete(QList<QListWidgetItem *> selectedItems);

signals:
    void CurrentIndexChanged();
    void FirstSongAdded();
    void SongsAdded();
    void mediaLoaded();
    void PlaylistChanged();
    void deleteItemsSelected(QList<QListWidgetItem *> selectedItems);

public:
    MusicPlayer(QWidget *parent = nullptr);
    ~MusicPlayer();
};
#endif // PLAYER_H
