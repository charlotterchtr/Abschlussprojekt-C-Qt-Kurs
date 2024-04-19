#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QUrl>
#include <QDropEvent>
#include <QMimeData>
#include <QPalette>
#include <iostream>
#include <QMenu>
#include <QAction>

class PlaylistDialog : public QDialog
{
    Q_OBJECT

public:
    PlaylistDialog(QWidget * parent = nullptr);
    QListWidget * playlistWidget;
    QList<QPair<QUrl, int>>* playlist = nullptr;
    int* CurrentIndex;

    void getPlaylist(QList<QPair<QUrl, int>> * orig_playlist);
    void handleCurrentIndexChanged();
    void handleShowPlaylist();
    void getCurrentIndex(int * i);
    void highlightCurrentIndex();


    void handleItemMoved(QDropEvent * event);
    void dropEvent(QDropEvent * event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void createContextMenu(QMouseEvent *event);

signals:
    void show_playlist();
    void CurrentIndexChanged();
    void createMenu(QMouseEvent *event);

private:
    QVBoxLayout * layout;
};

#endif // PLAYLIST_H
