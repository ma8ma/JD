// ライセンス: GPL2

//#define _DEBUG
#include "jddebug.h"

#include "loadable.h"

#include "jdlib/loader.h"
#include "jdlib/misctime.h"
#include "jdlib/miscutil.h"

#include "httpcode.h"

using namespace SKELETON;

Loadable::Loadable()
    : m_loader( nullptr ),
      m_low_priority( false ),
      m_charcode( CHARCODE_UNKNOWN )
{
    clear_load_data();
}


Loadable::~Loadable()
{
    terminate_load(); // 一応デストラクタ内部でも実行しておく
}


// ロード強制停止
void Loadable::terminate_load()
{
#ifdef _DEBUG
    std::cout << "Loadable::terminate_load\n";
#endif

    // loadableを delete する前に terminate_load() を呼び出さないと
    // スレッド実行中にメモリなどが初期化されてしまうため落ちる時がある
    // デストラクタ内から terminate_load() しても落ちる時があるので
    // デストラクタの外から呼び出すこと
    set_dispatchable( false );

    delete_loader();
}


// 完全クリア
void Loadable::clear_load_data()
{
    m_code = HTTP_INIT;
    m_str_code = std::string();
    m_date_modified = std::string();
    m_cookies.clear();
    m_location = std::string();
    m_total_length = 0;
    m_current_length = 0;
}



bool Loadable::is_loading() const
{
    if( ! m_loader ) return false;

    // m_loader != nullptr ならローダ起動中ってこと
    return true;
}


//
// 更新時刻
//
time_t Loadable::get_time_modified()
{
    time_t time_out;
    time_out = MISC::datetotime( m_date_modified );
    if( time_out == 0 ) time_out = time( nullptr ) - 600;

#ifdef _DEBUG
    std::cout << "Loadable::get_time_modified " << m_date_modified << std::endl
              << " -> " << time_out << std::endl;
#endif

    return time_out; 
}




void Loadable::delete_loader()
{
    if( m_loader ) delete m_loader;
    m_loader = nullptr;
}




//
// ロード開始
//
bool Loadable::start_load( const JDLIB::LOADERDATA& data )
{
    if( is_loading() ) return false;

#ifdef _DEBUG
    std::cout << "Loadable::start_load url = " << data.url << std::endl;
#endif

    assert( m_loader == nullptr );
    m_loader = new JDLIB::Loader( m_low_priority );

    // 情報初期化
    // m_date_modified, m_cookie は初期化しない
    m_code = HTTP_INIT;
    m_str_code = std::string();
    m_location = std::string();
    m_total_length = 0;
    m_current_length = 0;

    if( !m_loader->run( this, data ) ){
        m_code = get_loader_code();
        m_str_code = get_loader_str_code();
        delete_loader();
        return false;
    }

    return true;
}



//
// ロード中止
//
void Loadable::stop_load()
{
    if( m_loader ) m_loader->stop();
}



// ローダーからコールバックされてコードなどを取得してから
// receive_data() を呼び出す
void Loadable::receive( const char* data, size_t size )
{
    m_code = get_loader_code();
    if( ! m_total_length && m_code != HTTP_INIT ) m_total_length = get_loader_length();
    m_current_length += size;

    const CharCode charcode = get_loader_content_charset();
    if( charcode != CHARCODE_UNKNOWN ) set_charcode( charcode );

    receive_data( data, size );
}


// 別スレッドで動いているローダからfinish()を呼ばれたらディスパッチして
// メインスレッドに制御を戻してから callback_dispatch()を呼び出す。
// そうしないと色々不具合が生じる
void Loadable::finish()
{
#ifdef _DEBUG
    std::cout << "Loadable::finish\n";
#endif

    dispatch();
}


//
// ローダを削除してreceive_finish()をコール
//
void Loadable::callback_dispatch()
{
#ifdef _DEBUG
    std::cout << "Loadable::callback_dispatch\n";
#endif

    // ローダを削除する前に情報保存
    m_code = get_loader_code();
    if( ! get_loader_str_code().empty() ) m_str_code = get_loader_str_code();
    if( ! get_loader_contenttype().empty() ) m_contenttype = get_loader_contenttype();
    if( ! get_loader_modified().empty() ) m_date_modified = get_loader_modified();
    if( ! get_loader_cookies().empty() ) m_cookies = get_loader_cookies();
    if( ! get_loader_location().empty() ) m_location = get_loader_location();

    const CharCode charcode = get_loader_content_charset();
    if( charcode != CHARCODE_UNKNOWN ) set_charcode( charcode );

#ifdef _DEBUG
    std::cout << "delete_loader\n";
#endif    

    delete_loader();

#ifdef _DEBUG
    std::cout << "code = " << m_code << std::endl;
    std::cout << "str_code = " << m_str_code << std::endl;
    std::cout << "contenttype = " << m_contenttype << std::endl;
    std::cout << "modified = " << m_date_modified << std::endl;
    std::cout << "location = " << m_location << std::endl;
    std::cout << "total_length = " << m_total_length << std::endl;
    std::cout << "current length = " << m_current_length << std::endl;
    std::cout << "charset = " << get_charcode() << std::endl;
#endif

    receive_finish();
}



// ローダから各種情報の取得

int Loadable::get_loader_code() const
{
    if( ! m_loader ) return HTTP_INIT;

    return m_loader->data().code;
}


std::string Loadable::get_loader_str_code() const
{
    if( ! m_loader ) return std::string();

    return m_loader->data().str_code;
}


std::string Loadable::get_loader_contenttype() const
{
    if( ! m_loader ) return std::string();

    return m_loader->data().contenttype;
}


std::string Loadable::get_loader_modified() const
{
    if( ! m_loader ) return std::string();

    return m_loader->data().modified;
}


std::list< std::string > Loadable::get_loader_cookies() const
{
    if( ! m_loader ) return std::list< std::string >();

    return m_loader->data().list_cookies;
}


std::string Loadable::get_loader_location() const
{
    if( ! m_loader ) return std::string();

    return m_loader->data().location;
}


size_t Loadable::get_loader_length() const
{
    if( ! m_loader ) return 0;

    return m_loader->data().length;
}


CharCode Loadable::get_loader_content_charset() const
{
    CharCode charcode = CHARCODE_UNKNOWN;

    if( m_loader ){
        const std::string& contenttype = m_loader->data().contenttype;
        size_t pos = contenttype.find( "charset=" );
        if( pos != std::string::npos ){
            const std::string raw_charset =
                MISC::toupper_str( MISC::remove_space( contenttype.substr( pos + 8, std::string::npos ) ) );
            const std::string tmp_charset = MISC::remove_str( MISC::remove_str( raw_charset, "_" ), "-" );
            if( tmp_charset == "UTF8" ) charcode = CHARCODE_UTF8;
            else if( tmp_charset == "SHIFTJIS" || tmp_charset == "XSJIS"
                    || tmp_charset == "WINDOWS31J" ) charcode = CHARCODE_SJIS;
            else if( tmp_charset == "EUCJP" || tmp_charset == "XEUCJP" ) charcode = CHARCODE_EUCJP;
            // else charset = "iso-8859-1"; // その他は無視する
        }
    }

    return charcode;
}
