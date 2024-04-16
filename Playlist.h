#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QUrl>
#include <QDropEvent>
#include <QMimeData>

class PlaylistDialog : public QDialog
{
    Q_OBJECT

public:
    PlaylistDialog(QWidget * parent = nullptr);
    QList<QPair<QUrl, int>>* playlist = nullptr;
    int* CurrentIndex;

    void getPlaylist(QList<QPair<QUrl, int>> * orig_playlist);
    void handleCurrentIndexChanged();
    void handleShowPlaylist();


    void handleItemMoved(QDropEvent *event);
    void dropEvent(QDropEvent *event) override;

signals:
    void show_playlist();

private:
    QListWidget * playlistWidget;
    QVBoxLayout * layout;
};

#endif // PLAYLIST_H
