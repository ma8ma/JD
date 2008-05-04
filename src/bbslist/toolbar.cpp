// ライセンス: GPL2

//#define _DEBUG
#include "jddebug.h"

#include "toolbar.h"
#include "bbslistadmin.h"

#include "skeleton/compentry.h"
#include "skeleton/view.h"

#include "command.h"
#include "controlid.h"
#include "session.h"
#include "global.h"

using namespace BBSLIST;


BBSListToolBar::BBSListToolBar() :
    SKELETON::ToolBar( BBSLIST::get_admin() ),
    m_button_toggle( m_label )
{
    if( m_button_toggle.get_arrow() ) set_tooltip( *m_button_toggle.get_arrow(),
                                                   "板一覧とお気に入りの切り替え\n\nマウスホイール回転でも切り替え可能" );

    m_label.set_alignment( Gtk::ALIGN_LEFT );
    std::vector< std::string > menu;
    menu.push_back( "板一覧" );
    menu.push_back( "お気に入り" );
    m_button_toggle.append_menu( menu );
    m_button_toggle.signal_selected().connect( sigc::mem_fun(*this, &BBSListToolBar::slot_toggle ) );
    m_button_toggle.signal_scroll_event().connect(  sigc::mem_fun( *this, &BBSListToolBar::slot_scroll_event ));
    m_button_toggle.set_enable_sig_clicked( false );
    m_tool_toggle.set_expand( true );
    m_tool_toggle.add( m_button_toggle );

    m_tool_label.set_icon_size( Gtk::ICON_SIZE_MENU );
    m_tool_label.set_toolbar_style( Gtk::TOOLBAR_ICONS );
    m_tool_label.append( m_tool_toggle );
    m_tool_label.append( *get_button_close() );
    pack_start( m_tool_label, Gtk::PACK_SHRINK );

    pack_buttons();
    add_search_mode( CONTROL::MODE_BBSLIST );
}


//
// ボタンのパッキング
//
// virtual
void BBSListToolBar::pack_buttons()
{
    int num = 0;
    for(;;){
        int item = SESSION::get_item_sidebar( num );
        if( item == ITEM_END ) break;
        switch( item ){

            case ITEM_SEARCHBOX:
                get_buttonbar().append( *get_entry_search() );
                break;

            case ITEM_SEARCH_NEXT:
                get_buttonbar().append( *get_button_down_search() );
                break;

            case ITEM_SEARCH_PREV:
                get_buttonbar().append( *get_button_up_search() );
                break;

            case ITEM_SEPARATOR:
                pack_separator();
                break;
        }
        ++num;
    }

    set_relief();
    show_all_children();
}



// タブが切り替わった時にDragableNoteBook::set_current_toolbar()から呼び出される( Viewの情報を取得する )
// virtual
void BBSListToolBar::set_view( SKELETON::View* view )
{
    ToolBar::set_view( view );

    if( view )  m_label.set_text( view->get_label() );
}


void BBSListToolBar::slot_toggle( int i )
{
#ifdef _DEBUG 	 
     std::cout << "BBSListToolBar::slot_toggle = " << get_url() << " i = " << i << std::endl;
#endif 	 
  	 
     switch( i ){
  	 
         case 0:
             if( get_url() != URL_BBSLISTVIEW ) CORE::core_set_command( "switch_sidebar", URL_BBSLISTVIEW ); 	 
             break; 	 
  	 
         case 1:
             if( get_url() != URL_FAVORITEVIEW ) CORE::core_set_command( "switch_sidebar", URL_FAVORITEVIEW ); 	 
             break; 	 
     }
}


bool BBSListToolBar::slot_scroll_event( GdkEventScroll* event )
{
    guint direction = event->direction;

#ifdef _DEBUG
    std::cout << "BBSListToolBar::slot_scroll_event dir = " << direction << std::endl;
#endif

    if( direction == GDK_SCROLL_UP ) slot_toggle( 0 );
    if( direction == GDK_SCROLL_DOWN ) slot_toggle( 1 );

    return true;
}
