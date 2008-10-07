// ライセンス: GPL2

//
// アイコンの管理クラス
//

#ifndef _ICONNAGER_H
#define _ICONNAGER_H

#include <gtkmm.h>
#include <vector>

namespace ICON
{
    enum
    {
        NONE = -1,

        JD16 = 0,
        JD32,
        JD48,
        JD96,

        BKMARK_UPDATE,
        BKMARK,
        BKMARK_THREAD,

        UPDATE,
        NEWTHREAD,
        NEWTHREAD_HOUR,
        CHECK,
        DOWN,
        WRITE,
        POST,
        POST_REFER,
        LOADING,
        LOADING_STOP,

        DIR,
        FAVORITE,

        BOARD,
        BOARD_UPDATE,
        BOARD_UPDATED,

        THREAD,
        THREAD_UPDATE,
        THREAD_UPDATED,
        THREAD_OLD,

        IMAGE,
        LINK,

#if GTKMMVER <= 240   // 2.4 以前は Gtk::Stock::MEDIA_PLAY が無い
        PLAY,
#endif

        TRANSPARENT,

        NUM_ICONS
    };

    class ICON_Manager
    {
        std::vector< Glib::RefPtr< Gdk::Pixbuf > > m_list_icons;

      public:

        ICON_Manager();
        virtual ~ICON_Manager();

        Glib::RefPtr< Gdk::Pixbuf > get_icon( int id );
    };

    ///////////////////////////////////////
    // インターフェース

    ICON_Manager* get_icon_manager();
    void delete_icon_manager();

    Glib::RefPtr< Gdk::Pixbuf > get_icon( int id );
}

#endif
