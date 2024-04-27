#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QDialog>
#include <QListWidget>
#include <QVBoxLayout>
#include <QUrl>
#include <QPalette>
#include <QMenu>
#include <QAction>
#include <QApplication>

class PlaylistDialog : public QDialog
{
    Q_OBJECT

private:
    QVBoxLayout * layout;
    QListWidget * playlistWidget = nullptr;
    QList<QUrl>* playlist = nullptr;
    int* CurrentIndex = nullptr;

private slots:
    void handleShowPlaylist();
    void highlightCurrentIndex();
    void deleteSelectedItem();
    void handleItemClicked();

public:
    PlaylistDialog(QWidget * parent = nullptr);
    ~PlaylistDialog();
    void getPlaylist(QList<QUrl> * orig_playlist);
    void getCurrentIndex(int * i);
    void handleCurrentIndexChanged();

signals:
    void show_playlist();
    void CurrentIndexChanged();
    void ItemDeleted(int index);

};

#endif // PLAYLIST_H
