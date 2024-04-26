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
#include <QApplication>

class PlaylistDialog : public QDialog
{
    Q_OBJECT

public:
    PlaylistDialog(QWidget * parent = nullptr);
    QListWidget * playlistWidget = nullptr;
    QList<QPair<QUrl, int>>* playlist = nullptr;
    int* CurrentIndex = nullptr;

    void getPlaylist(QList<QPair<QUrl, int>> * orig_playlist);
    void handleCurrentIndexChanged();
    void handleShowPlaylist();
    void getCurrentIndex(int * i);
    void highlightCurrentIndex();
    void deleteSelectedItem();


    //void handleItemMoved(QDropEvent * event);
    //void dropEvent(QDropEvent * event) override;
    //void mousePressEvent(QMouseEvent *event) override;
    //void createContextMenu(QMouseEvent *event);

signals:
    void show_playlist();
    void CurrentIndexChanged();
    void createMenu(QMouseEvent *event);
    void ItemDeleted(int index);

private:
    QVBoxLayout * layout;
};

#endif // PLAYLIST_H
