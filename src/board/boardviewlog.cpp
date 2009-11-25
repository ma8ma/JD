// ライセンス: GPL2

//#define _DEBUG
#include "jddebug.h"

#include "boardadmin.h"
#include "boardviewlog.h"

#include "skeleton/msgdiag.h"

#include "dbtree/interface.h"
#include "dbtree/articlebase.h"

#include "searchmanager.h"
#include "session.h"
#include "global.h"

using namespace BOARD;


BoardViewLog::BoardViewLog( const std::string& url )
    : BoardViewBase( url, ( url == URL_ALLLOG ) )
{
    set_writeable( false );
    set_load_subject_txt( false );

    CORE::get_search_manager()->sig_search_fin().connect( sigc::mem_fun( *this, &BoardViewLog::slot_search_fin ) );

#ifdef _DEBUG
    std::cout << "BoardViewLog::BoardViewLog : url = " << get_url() << std::endl;
#endif
}


BoardViewLog::~BoardViewLog()
{
#ifdef _DEBUG
    std::cout << "BoardViewLog::~BoardViewLog : url = " << get_url() << std::endl;
#endif
}


//
// 検索停止
//
void BoardViewLog::stop()
{
    CORE::get_search_manager()->stop( get_url() );
}


//
// リロード
//
void BoardViewLog::reload()
{
    if( CORE::get_search_manager()->is_searching() ){
        SKELETON::MsgDiag mdiag( get_parent_win(), "他の検索スレッドが実行中です" );
        mdiag.run();
        return;
    }

    show_view();
}



//
// ビュー表示
//
void BoardViewLog::show_view()
{
#ifdef _DEBUG
    std::cout << "BoardViewLog::show_view " << get_url() << std::endl;
#endif

    if( ! SESSION::is_booting() ){
        const bool searchall = ( get_url() == URL_ALLLOG );
        CORE::get_search_manager()->search_log( get_url(), get_url_board(), "", false, searchall, false );
    }

    BoardViewBase::show_view();
}


//
// 検索終了
//
void BoardViewLog::slot_search_fin( const std::string& id )
{
    if( id != get_url() ) return;

#ifdef _DEBUG
    std::cout << "BoardViewLog::slot_search_fin size = " << CORE::get_search_manager()->get_list_article().size() << std::endl;
#endif

    update_view_impl( CORE::get_search_manager()->get_list_article() );
}


//
// 板名更新
//
void BoardViewLog::update_boardname()
{
    std::string title = "[ ログ一覧 ]";
    if( ! get_url_board().empty() ) title += " - " + DBTREE::board_name( get_url_board() );

    // ウィンドウタイトル表示
    set_title( title );
    BOARD::get_admin()->set_command( "set_title", get_url(), get_title() );

    // タブに名前をセット
    BOARD::get_admin()->set_command( "set_tablabel", get_url(), title );
}


//
// 特定の行だけの表示内容更新
//
// url : subject.txt のアドレス
// id : DAT の ID(拡張子付き)
//
void BoardViewLog::update_item( const std::string& url, const std::string& id )
{
    if( get_url() != URL_ALLLOG && get_url_board() != url ) return;
    if( CORE::get_search_manager()->is_searching( get_url() ) ) return;

    const std::string url_dat = DBTREE::url_datbase( url ) + id;

#ifdef _DEBUG
    std::cout << "BoardViewLog::update_item " << get_url() << std::endl
              << "url = " << url << " id = " << id << " url_dat = " << url_dat << std::endl;
#endif

    const Gtk::TreeModel::Row row = get_row_from_url( url_dat );

    if( id.empty() || row ) BoardViewBase::update_item( get_url_board(), id );

    // もし row が無く、かつキャッシュがあるならば行を追加
    else{ 

        DBTREE::ArticleBase* art = DBTREE::get_article( url_dat );
        if( art && art->is_cached() ){

#ifdef _DEBUG
            std::cout << "prepend\n";
#endif
            prepend_row( art );
            goto_top();
        }
    }
}