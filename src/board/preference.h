// ライセンス: GPL2

#ifndef _BOARD_PREFERENCES_H
#define _BOARD_PREFERENCES_H

#include "skeleton/prefdiag.h"
#include "skeleton/editview.h"
#include "skeleton/label_entry.h"

namespace BOARD
{
    class ProxyFrame : public Gtk::Frame
    {
        Gtk::VBox m_vbox;
        Gtk::HBox m_hbox;
        Gtk::Label m_label_host;
        Gtk::Label m_label_port;

      public:

        Gtk::RadioButton rd_global, rd_disable, rd_local;
        Gtk::Entry entry_host;
        Gtk::Entry entry_port;

        ProxyFrame( const std::string& title )
        : m_label_host( "host" ), m_label_port( "port" ),
        rd_global( "全体設定を使用する" ), rd_disable( "全体設定を無効にする" ), rd_local( "ローカル設定を使用する" )
        {
            Gtk::RadioButton::Group grp = rd_global.get_group();
            rd_disable.set_group( grp );
            rd_local.set_group( grp );

            m_hbox.set_spacing( 8 );
            m_hbox.set_border_width( 8 );

            m_hbox.pack_start( m_label_host, Gtk::PACK_SHRINK );
            m_hbox.pack_start( entry_host );
            m_hbox.pack_start( m_label_port, Gtk::PACK_SHRINK );
            m_hbox.pack_start( entry_port, Gtk::PACK_SHRINK );

            m_vbox.set_spacing( 8 );
            m_vbox.set_border_width( 8 );
            m_vbox.pack_start( rd_global, Gtk::PACK_SHRINK );
            m_vbox.pack_start( rd_disable, Gtk::PACK_SHRINK );
            m_vbox.pack_start( rd_local, Gtk::PACK_SHRINK );
            m_vbox.pack_start( m_hbox, Gtk::PACK_SHRINK );

            set_label( title );
            set_border_width( 8 );
            add( m_vbox );
        }
    };

    class Preferences : public SKELETON::PrefDiag
    {
        Gtk::Notebook m_notebook;
        Gtk::VBox m_vbox;

        // クッキー & hana
        Gtk::Frame m_frame_cookie;
        Gtk::HBox m_hbox_cookie;
        SKELETON::EditView m_edit_cookies;
        Gtk::VBox m_vbox_cookie;
        Gtk::Button m_button_cookie;

        // 名無し書き込みチェック
        Gtk::CheckButton m_check_noname;

        // プロキシ
        Gtk::VBox m_vbox_proxy;
        Gtk::Label m_label_proxy;
        ProxyFrame m_proxy_frame;
        ProxyFrame m_proxy_frame_w;

        // 情報
        Gtk::VBox m_vbox_info;
        Gtk::Label m_label_name;
        SKELETON::LabelEntry m_label_url;
        SKELETON::LabelEntry m_label_cache;

        SKELETON::LabelEntry m_label_noname;
        SKELETON::LabelEntry m_label_line;
        SKELETON::LabelEntry m_label_byte;

        // あぼーん
        SKELETON::EditView m_edit_thread, m_edit_word, m_edit_regex;

        // SETTING.TXT
        SKELETON::EditView m_edit_settingtxt;

      public:
        Preferences( const std::string& url );

      private:
        void slot_delete_cookie();
        virtual void slot_ok_clicked();
    };

}

#endif
